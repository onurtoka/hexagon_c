# Hexagonal Architecture Project Makefile
# Build, test, and run targets for the track data processing application

# Project Configuration
PROJECT_NAME = a_hexagon
VERSION = 1.0.0

# Compiler Settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -DZMQ_BUILD_DRAFT_API
RELEASE_FLAGS = -O2 -DNDEBUG
DEBUG_FLAGS = -O0 -g -DDEBUG_MODE

# ZeroMQ Configuration (using third_party)
THIRD_PARTY_DIR = third_party
LIBZMQ_DIR = $(THIRD_PARTY_DIR)/libzmq
CPPZMQ_DIR = $(THIRD_PARTY_DIR)/cppzmq

# ZMQ include and library paths
ZMQ_INCLUDE_DIRS = -I$(LIBZMQ_DIR)/include -I$(CPPZMQ_DIR)
ZMQ_LIB_DIRS = -L$(LIBZMQ_DIR)/build/lib
ZMQ_STATIC_LIB = $(LIBZMQ_DIR)/build/lib/libzmq.a
ZMQ_LIBS = $(ZMQ_STATIC_LIB)

# ZMQ flags
ZMQ_CFLAGS = $(ZMQ_INCLUDE_DIRS) -DZMQ_BUILD_DRAFT_API=1

# Project Directories
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build
BIN_DIR = bin
OBJ_DIR = $(BUILD_DIR)/obj
TEST_OBJ_DIR = $(BUILD_DIR)/test_obj

# Include Directories
INCLUDES = -I$(SRC_DIR) -I$(SRC_DIR)/domain -I$(SRC_DIR)/adapters $(ZMQ_CFLAGS)

# Libraries to link
LIBS = $(ZMQ_LIBS) -lpthread -lpgm

# Source Files
DOMAIN_SOURCES = $(wildcard $(SRC_DIR)/domain/*/*.cpp) $(wildcard $(SRC_DIR)/domain/*/*/*.cpp)
ADAPTER_SOURCES = $(wildcard $(SRC_DIR)/adapters/*/*.cpp)
UTILITIES_SOURCES = $(wildcard $(SRC_DIR)/utilities/*.cpp)
APP_SOURCES = $(SRC_DIR)/application/main.cpp
TEST_SOURCES = $(wildcard $(TEST_DIR)/*/*.cpp) $(wildcard $(TEST_DIR)/*/*/*.cpp)

# Object Files
DOMAIN_OBJECTS = $(DOMAIN_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
ADAPTER_OBJECTS = $(ADAPTER_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
UTILITIES_OBJECTS = $(UTILITIES_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
APP_OBJECTS = $(APP_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(TEST_OBJ_DIR)/%.o)

# Library Files
DOMAIN_LIB = $(BUILD_DIR)/libdomain.a
ADAPTER_LIB = $(BUILD_DIR)/libadapter.a

# Executables
MAIN_EXEC = $(BIN_DIR)/main
TEST_EXEC = $(BIN_DIR)/test_extrapolator

# Default target
.DEFAULT_GOAL := all

# Create necessary directories
$(OBJ_DIR) $(TEST_OBJ_DIR) $(BIN_DIR) $(BUILD_DIR):
	@mkdir -p $@
	@mkdir -p $(dir $(DOMAIN_OBJECTS))
	@mkdir -p $(dir $(ADAPTER_OBJECTS))
	@mkdir -p $(dir $(UTILITIES_OBJECTS))
	@mkdir -p $(dir $(APP_OBJECTS))
	@mkdir -p $(dir $(TEST_OBJECTS))

# Build object files for domain layer
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "🔨 Compiling $<"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build object files for tests
$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp | $(TEST_OBJ_DIR)
	@echo "🧪 Compiling test $<"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build domain library
$(DOMAIN_LIB): $(DOMAIN_OBJECTS) | $(BUILD_DIR)
	@echo "📚 Creating domain library"
	@ar rcs $@ $^

# Build adapter library
$(ADAPTER_LIB): $(ADAPTER_OBJECTS) $(UTILITIES_OBJECTS) $(DOMAIN_LIB) | $(BUILD_DIR)
	@echo "🔌 Creating adapter library"
	@ar rcs $@ $(ADAPTER_OBJECTS) $(UTILITIES_OBJECTS)

# Build main application
$(MAIN_EXEC): $(APP_OBJECTS) $(ADAPTER_LIB) $(DOMAIN_LIB) | $(BIN_DIR)
	@echo "🚀 Linking main application"
	@$(CXX) $(CXXFLAGS) $< -L$(BUILD_DIR) -ladapter -ldomain $(LIBS) -o $@

# Build test executable
$(TEST_EXEC): $(TEST_OBJECTS) $(ADAPTER_LIB) $(DOMAIN_LIB) | $(BIN_DIR)
	@echo "🧪 Linking test executable"
	@$(CXX) $(CXXFLAGS) $^ -L$(BUILD_DIR) -ladapter -ldomain $(LIBS) -o $@

# Main targets
.PHONY: all clean test run debug release install help

# Build everything
all: $(MAIN_EXEC) $(TEST_EXEC)
	@echo "✅ Build completed successfully!"
	@echo "   Main executable: $(MAIN_EXEC)"
	@echo "   Test executable: $(TEST_EXEC)"

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "✅ Clean completed!"

# Run tests
test: $(TEST_EXEC)
	@echo "🧪 Running tests..."
	@./$(TEST_EXEC)
	@echo "✅ Tests completed!"

# Run main application
run: $(MAIN_EXEC)
	@echo "🚀 Running main application..."
	@./$(MAIN_EXEC)

# Run application with timeout
run-timeout: $(MAIN_EXEC)
	@echo "🚀 Running main application (10s timeout)..."
	@timeout 10s ./$(MAIN_EXEC) || echo "⏰ Application stopped after 10 seconds"

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: clean all
	@echo "🐛 Debug build completed!"

# Release build
release: CXXFLAGS += $(RELEASE_FLAGS)
release: clean all
	@echo "🚀 Release build completed!"

# Install (copy to system)
install: release
	@echo "📦 Installing application..."
	@sudo cp $(MAIN_EXEC) /usr/local/bin/hexagon_main
	@sudo chmod +x /usr/local/bin/hexagon_main
	@echo "✅ Installation completed! Run with: hexagon_main"

# Build and test everything
build-test: all test
	@echo "✅ Build and test completed!"

# Build, test, and run
full: all test run-timeout
	@echo "✅ Full build, test, and run cycle completed!"

# Show project information
info:
	@echo "📋 Project Information:"
	@echo "   Name: $(PROJECT_NAME)"
	@echo "   Version: $(VERSION)"
	@echo "   Compiler: $(CXX)"
	@echo "   Flags: $(CXXFLAGS)"
	@echo "   ZeroMQ: $(ZMQ_CFLAGS) $(ZMQ_LIBS)"
	@echo "   Sources: $(words $(DOMAIN_SOURCES) $(ADAPTER_SOURCES)) files"
	@echo "   Tests: $(words $(TEST_SOURCES)) files"

# Generate documentation (if Doxygen is available)
docs:
	@if command -v doxygen >/dev/null 2>&1; then \
		echo "📖 Generating documentation..."; \
		doxygen Doxyfile; \
		echo "✅ Documentation generated in html/"; \
	else \
		echo "❌ Doxygen not found. Please install doxygen to generate documentation."; \
	fi

# Package for distribution
package: release
	@echo "📦 Creating distribution package..."
	@mkdir -p dist/$(PROJECT_NAME)-$(VERSION)
	@cp -r src test README.md LICENSE Doxyfile dist/$(PROJECT_NAME)-$(VERSION)/
	@cp $(MAIN_EXEC) dist/$(PROJECT_NAME)-$(VERSION)/
	@cd dist && tar -czf $(PROJECT_NAME)-$(VERSION).tar.gz $(PROJECT_NAME)-$(VERSION)
	@echo "✅ Package created: dist/$(PROJECT_NAME)-$(VERSION).tar.gz"

# Development cycle (commonly used during development)
dev: clean all test
	@echo "🔄 Development cycle completed!"

# Watch for changes and rebuild (requires inotify-tools)
watch:
	@if command -v inotifywait >/dev/null 2>&1; then \
		echo "👁  Watching for changes... (Ctrl+C to stop)"; \
		while inotifywait -r -e modify $(SRC_DIR) $(TEST_DIR); do \
			make dev; \
		done; \
	else \
		echo "❌ inotifywait not found. Install inotify-tools for watch functionality."; \
	fi

# Help target
help:
	@echo "🔧 Available targets:"
	@echo ""
	@echo "Build targets:"
	@echo "  all            - Build all components (default)"
	@echo "  debug          - Build with debug flags"
	@echo "  release        - Build with release optimization"
	@echo "  clean          - Remove build artifacts"
	@echo ""
	@echo "Test targets:"
	@echo "  test           - Run unit tests"
	@echo "  build-test     - Build and run tests"
	@echo ""
	@echo "Run targets:"
	@echo "  run            - Run main application"
	@echo "  run-timeout    - Run with 10s timeout"
	@echo "  full           - Build, test, and run"
	@echo ""
	@echo "Development targets:"
	@echo "  dev            - Clean, build, and test"
	@echo "  watch          - Watch files and rebuild on changes"
	@echo ""
	@echo "Distribution targets:"
	@echo "  install        - Install to system (/usr/local/bin)"
	@echo "  package        - Create distribution package"
	@echo "  docs           - Generate documentation"
	@echo ""
	@echo "Utility targets:"
	@echo "  info           - Show project information"
	@echo "  help           - Show this help message"

# Debugging target to show variables
show-vars:
	@echo "DOMAIN_SOURCES: $(DOMAIN_SOURCES)"
	@echo "ADAPTER_SOURCES: $(ADAPTER_SOURCES)"
	@echo "APP_SOURCES: $(APP_SOURCES)"
	@echo "TEST_SOURCES: $(TEST_SOURCES)"
	@echo "ZMQ_CFLAGS: $(ZMQ_CFLAGS)"
	@echo "ZMQ_LIBS: $(ZMQ_LIBS)"
