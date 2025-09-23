cmake_minimum_required(VERSION 3.16)
project(a_hexagon 
    VERSION 1.0
    DESCRIPTION "Template CMakeLists for hexagon projects"
    LANGUAGES CXX
    )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_ZMQ "Build and link against bundled libzmq" ON)
option(USE_DOXYGEN "Generate doxygen documentation" ON)
option(COVERAGE_REPORT "Generate coverage reports" ON)
option(PRINT_INFORMATION "Print compiler and system information" ON)

#This segment prints useful information about the build environment (probably will be useful later, maybe never..)
if (PRINT_INFORMATION)
    if(CMAKE_CXX_COMPILER_LOADED)
        message(STATUS "C++ Compiler Version: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
        if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "8.5")
            #If GNU CXX 8.5 is a must, change this to FATAL_ERROR
            message(WARNING "Recommended compiler version is 8.5, but found ${CMAKE_CXX_COMPILER_VERSION}")
        endif()
    else()
        message(FATAL_ERROR "No C++ compiler found!")
    endif()
endif()

#If user did not specify a build type, default to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

# MISRA-friendly warning flags
set(MISRA_FLAGS
    -Wall                # Enable most common warnings
    -Wextra              # Enable extra warnings not covered by -Wall
    -Wpedantic           # Warn on non-standard language extensions
    -Wconversion         # Warn on implicit type conversions (e.g. int -> float)
    -Wsign-conversion    # Warn on implicit signed/unsigned conversions
    -Wdouble-promotion   # Warn when float values are implicitly promoted to double
    -Wcast-qual          # Warn when casting away const/volatile qualifiers
    -Wcast-align         # Warn on pointer casts that can change alignment
    -Wuseless-cast       # Warn on unnecessary (no-op) casts
    -Wold-style-cast     # Warn on use of C-style casts
    -Wnull-dereference   # Warn about potential null pointer dereference
    -Wlogical-op         # Warn on suspicious use of logical operators (&&, ||)
    -Wduplicated-cond    # Warn if the same condition appears in an if/else-if chain
    -Wnon-virtual-dtor   # Warn when a class with virtual functions has no virtual destructor
    -Woverloaded-virtual # Warn when a virtual function is hidden by an overload
    -Wshadow             # Warn when a variable declaration shadows another variable
    -Wswitch-default     # Warn if a switch lacks a default label
    -Wswitch-enum        # Warn if a switch does not cover all enum values
    -Wfloat-equal        # Warn when comparing floating-point values with == or !=
    -Wredundant-decls    # Warn on redundant declarations
    -Wextra-semi         # Warn on unnecessary semicolons
    -Wmissing-declarations # Warn if a global function is defined without prior declaration
    -Wstrict-overflow=5  # Warn on code that may depend on signed integer overflow
    -Wundef              # Warn if an undefined macro is used in #if or #elif
    -Wmissing-include-dirs # Warn if an -I path is invalid
    -Wformat=2           # Strong format string checks for printf/scanf
)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
set(INCLUDE_DIRECTORY "${CMAKE_SOURCE_DIR}/include")

file(GLOB_RECURSE DOMAIN_FILES "${CMAKE_SOURCE_DIR}/src/domain/*.cpp")
file(GLOB_RECURSE DOMAIN_HEADERS "${CMAKE_SOURCE_DIR}/include/domain/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRECTORY})
target_compile_options(${PROJECT_NAME} PRIVATE ${MISRA_FLAGS})

if(USE_ZMQ)
    message(STATUS "Using libzmq")
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(WITH_PERF_TOOL OFF CACHE BOOL "" FORCE)
    set(ENABLE_DRAFTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(third_party/libzmq EXCLUDE_FROM_ALL)
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/libzmq/include)
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/cppzmq)
    target_link_libraries(${PROJECT_NAME} PRIVATE libzmq)
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND AND USE_DOXYGEN)
    message(STATUS "Doxygen found: ${DOXYGEN_EXECUTABLE}")
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    add_custom_target(doxy
        ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
else()
    message(STATUS "Doxygen not found, 'doxy' target will not be available")
endif()

enable_testing()

file(GLOB_RECURSE TEST_FILES "${CMAKE_SOURCE_DIR}/tests/*.cpp")

add_library(domainlib STATIC ${DOMAIN_FILES})
target_include_directories(domainlib PRIVATE ${INCLUDE_DIRECTORY}) #domain headers

set(COVERAGE_FLAGS -g -O0 --coverage)

foreach(test ${TEST_FILES})
    get_filename_component(test_name ${test} NAME_WE)
    add_executable(${test_name} ${test})
    if(USE_ZMQ)
        target_link_libraries(${test_name} PRIVATE domainlib libzmq)
    else()
        target_link_libraries(${test_name} PRIVATE domainlib)
    endif()
    target_compile_options(${test_name} PRIVATE ${COVERAGE_FLAGS})
    target_link_options(${test_name} PRIVATE ${COVERAGE_FLAGS})
    target_include_directories(${test_name} PRIVATE ${INCLUDE_DIRECTORY})
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach(test ${TEST_FILES})

find_program(GCOVR_EXECUTABLE NAMES gcovr)

#generate coverage reports
if(GCOVR_EXECUTABLE)
    set(COVERAGE_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/build/coverage_reports)
    add_custom_target(coverage
        ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_OUTPUT_DIR}
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND ${GCOVR_EXECUTABLE} -r ${CMAKE_SOURCE_DIR}
                --html --html-details
                -o ${COVERAGE_OUTPUT_DIR}/coverage.html
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build
        COMMENT "Running coverage report..."
    )
endif()

find_program(CPPCHECK_EXECUTABLE NAMES cppcheck)

#generate cppcheck report
if(CPPCHECK_EXECUTABLE)
    set(CPPCHECK_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/build/cppcheck_reports)
    add_custom_target(cppcheck
        ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CPPCHECK_OUTPUT_DIR}
        COMMAND ${CPPCHECK_EXECUTABLE} --enable=all --inconclusive --std=c++17 --force --quiet
                --output-file=${CPPCHECK_OUTPUT_DIR}/cppcheck_report.txt ${CMAKE_SOURCE_DIR}/src
        COMMAND ${CPPCHECK_EXECUTABLE} 
                --enable=all
                --inconclusive
                --std=c++17
                --force
                --template=gcc
                --suppress=missingIncludeSystem
                --xml
                --xml-version=2
                --output-file=${CPPCHECK_OUTPUT_DIR}/cppcheck_report.xml
                ${CMAKE_SOURCE_DIR}/src
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src
        COMMENT "Running cppcheck static analysis..."
    )
endif()

find_program(VALGRIND_EXECUTABLE NAMES valgrind)

if(VALGRIND_EXECUTABLE)
    set(VALGRIND_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/build/valgrind_reports)
    # Uncomment the following lines to add a memcheck target for the main executable. main executable is not suitable for valgrind testing for the time being.
    # add_custom_target(memcheck
    #     ALL
    #     COMMAND ${CMAKE_COMMAND} -E make_directory ${VALGRIND_OUTPUT_DIR}
    #     COMMAND ${VALGRIND_EXECUTABLE}
    #         --tool=memcheck
    #         --leak-check=full
    #         --show-leak-kinds=all
    #         --track-origins=yes
    #         --error-exitcode=1
    #         --xml=yes
    #         --xml-file=${VALGRIND_OUTPUT_DIR}/valgrind_report.xml
    #         ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    #     COMMENT "Running memory check with Valgrind..."
    # )

    # Add valgrind test for each test executable
    foreach(test ${TEST_FILES})
        get_filename_component(test_name ${test} NAME_WE)
        add_custom_target(${test_name}_memcheck
            ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory ${VALGRIND_OUTPUT_DIR}
            COMMAND ${VALGRIND_EXECUTABLE}
                --tool=memcheck
                --leak-check=full
                --show-leak-kinds=all
                --track-origins=yes
                --error-exitcode=1
                --xml=yes
                --xml-file=${VALGRIND_OUTPUT_DIR}/${test_name}_valgrind.xml
                ${CMAKE_BINARY_DIR}/${test_name}
            COMMAND ${VALGRIND_EXECUTABLE}
                --tool=memcheck
                --leak-check=full
                --show-leak-kinds=all
                --track-origins=yes
                --error-exitcode=1
                ${CMAKE_BINARY_DIR}/${test_name}
                > ${VALGRIND_OUTPUT_DIR}/${test_name}_valgrind.txt 2>&1
            COMMENT "Running memory check for ${test_name} with Valgrind..."
        )
    endforeach()
endif()