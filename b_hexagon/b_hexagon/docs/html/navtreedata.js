/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Hexagonal C++ Example", "index.html", [
    [ "Hexagonal C++ Documentation Build Guide", "index.html", "index" ],
    [ "B_Hexagon API Documentation", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html", [
      [ "Overview", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md1", null ],
      [ "Core Domain Models", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md2", [
        [ "TrackData", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md3", null ],
        [ "DelayCalculatedTrackData", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md4", null ]
      ] ],
      [ "Calculation Logic", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md5", [
        [ "TrackCalculations", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md6", null ]
      ] ],
      [ "Port Interfaces", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md7", [
        [ "IDataReceiver (Inbound Port)", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md8", null ],
        [ "IDataSender (Outbound Port)", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md9", null ]
      ] ],
      [ "Application Service", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md10", [
        [ "CalculatorService", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md11", null ]
      ] ],
      [ "ZeroMQ Adapters", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md12", [
        [ "ZeroMQDataReceiver", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md13", null ],
        [ "ZeroMQDataSender", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md14", null ]
      ] ],
      [ "Error Handling", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md15", [
        [ "Exception Types", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md16", null ],
        [ "MISRA Exception Safety", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md17", null ]
      ] ],
      [ "Testing Support", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md18", [
        [ "Mock Implementations", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md19", null ],
        [ "Unit Test Example", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md20", null ]
      ] ],
      [ "Configuration", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md21", [
        [ "ZeroMQ Endpoints", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md22", null ],
        [ "Production Configuration", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md23", null ]
      ] ],
      [ "Time Handling", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md24", [
        [ "Units and Precision", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md25", null ],
        [ "Time Conversion Utilities", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md26", null ]
      ] ],
      [ "Performance Considerations", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md27", [
        [ "Memory Management", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md28", null ],
        [ "Threading", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md29", null ],
        [ "Latency Optimization", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md30", null ]
      ] ],
      [ "MISRA C++ 2023 Compliance", "md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md31", null ]
    ] ],
    [ "Hexagonal Architecture Documentation", "md_docs_2architecture.html", [
      [ "Overview", "md_docs_2architecture.html#autotoc_md33", null ],
      [ "Architecture Layers", "md_docs_2architecture.html#autotoc_md34", [
        [ "Color Legend", "md_docs_2architecture.html#autotoc_md35", null ]
      ] ],
      [ "High-Level Component Diagram", "md_docs_2architecture.html#autotoc_md36", null ],
      [ "End-to-End Data Flow", "md_docs_2architecture.html#autotoc_md37", null ],
      [ "Sequence Diagram: Track Processing Use Case", "md_docs_2architecture.html#autotoc_md38", null ],
      [ "Component Responsibilities", "md_docs_2architecture.html#autotoc_md39", [
        [ "Domain Core (Green)", "md_docs_2architecture.html#autotoc_md40", null ],
        [ "Ports (Blue)", "md_docs_2architecture.html#autotoc_md41", null ],
        [ "Adapters (Orange)", "md_docs_2architecture.html#autotoc_md42", null ],
        [ "Infrastructure (Gray)", "md_docs_2architecture.html#autotoc_md43", null ],
        [ "External Systems (Purple)", "md_docs_2architecture.html#autotoc_md44", null ]
      ] ],
      [ "Benefits of This Architecture", "md_docs_2architecture.html#autotoc_md45", null ],
      [ "Testing Strategy", "md_docs_2architecture.html#autotoc_md46", null ]
    ] ],
    [ "Enhanced Doxygen Documentation Setup", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html", [
      [ "Features Enabled", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md48", [
        [ "1. Automatic UML Class Diagrams", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md49", null ],
        [ "2. Call Graphs and Dependencies", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md50", null ],
        [ "3. Interactive SVG Output", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md51", null ],
        [ "4. Mermaid Diagram Support", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md52", null ],
        [ "5. Enhanced Documentation Style", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md53", null ]
      ] ],
      [ "Documentation Styles Supported", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md54", [
        [ "Javadoc-Style Comments", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md55", null ],
        [ "Mermaid Diagrams", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md56", null ],
        [ "Class Documentation", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md57", null ]
      ] ],
      [ "Dependencies", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md58", [
        [ "Required Tools", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md59", null ]
      ] ],
      [ "Building Documentation", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md60", [
        [ "Basic Documentation Generation", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md61", null ],
        [ "Using the Build Script", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md62", null ]
      ] ],
      [ "Generated Output", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md63", [
        [ "HTML Documentation", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md64", null ],
        [ "Generated Diagrams", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md65", null ]
      ] ],
      [ "Example Files", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md66", [
        [ "1. Basic Documentation Example", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md67", null ],
        [ "2. Hexagonal Architecture Documentation", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md68", null ]
      ] ],
      [ "Advanced Configuration", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md69", [
        [ "Customizing Diagrams", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md70", null ],
        [ "Adding Custom CSS", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md71", null ],
        [ "Excluding Files", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md72", null ]
      ] ],
      [ "Troubleshooting", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md73", [
        [ "Common Issues", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md74", null ],
        [ "Verification Commands", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md75", null ]
      ] ],
      [ "Integration with VS Code", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md76", [
        [ "Recommended Extensions", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md77", null ],
        [ "Tasks Configuration", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md78", null ]
      ] ],
      [ "Best Practices", "md_docs_2_d_o_x_y_g_e_n___s_e_t_u_p.html#autotoc_md79", null ]
    ] ],
    [ "MISRA C++ 2023 Compliance Implementation Report", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html", [
      [ "Overview", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md81", null ],
      [ "Key MISRA Rules Implemented", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md82", [
        [ "Rule 16-2-3: Header Guards", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md83", null ],
        [ "Rule 12-1-4: Explicit Constructors", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md84", null ],
        [ "Rule 8-5-1: Variable Initialization", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md85", null ],
        [ "Rule 15-3-4: Exception Safety", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md86", null ],
        [ "Rule 7-1-2: Const Correctness", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md87", null ],
        [ "Rule 5-0-5: Type Safety", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md88", null ]
      ] ],
      [ "Files Modified for MISRA Compliance", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md89", [
        [ "1. ZeroMQDataSender.hpp", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md90", null ],
        [ "2. ZeroMQDataSender.cpp", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md91", null ],
        [ "3. ZeroMQDataReceiver.hpp", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md92", null ],
        [ "4. ZeroMQDataReceiver.cpp", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md93", null ],
        [ "5. TrackData.hpp", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md94", null ],
        [ "6. DelayCalculatedTrackData.hpp", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md95", null ]
      ] ],
      [ "MISRA Compliance Summary", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md96", null ],
      [ "Build Configuration for MISRA", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md97", null ],
      [ "Remaining Considerations", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md98", null ],
      [ "Conclusion", "md_docs_2_m_i_s_r_a___c_o_m_p_l_i_a_n_c_e___i_m_p_l_e_m_e_n_t_a_t_i_o_n.html#autotoc_md99", null ]
    ] ],
    [ "MISRA C++ 2023 Compliance Guide", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html", [
      [ "Overview", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md101", null ],
      [ "Key MISRA C++ 2023 Rules Applied", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md102", [
        [ "1. Header Guards (Rule 16-2-3)", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md103", null ],
        [ "2. Explicit Constructors (Rule 12-1-4)", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md104", null ],
        [ "3. Resource Management (Rules 15-1-1, 15-3-3)", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md105", null ],
        [ "4. Exception Safety (Rule 15-3-4)", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md106", null ],
        [ "5. Const Correctness (Rule 7-1-2)", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md107", null ],
        [ "6. Initialization (Rule 8-5-1)", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md108", null ],
        [ "7. Type Safety (Rule 5-0-21)", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md109", null ],
        [ "8. Memory Management (Rule 18-4-1)", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md110", null ],
        [ "9. Function Parameter Passing (Rule 8-4-2)", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md111", null ],
        [ "10. Namespace Usage (Rule 7-3-1)", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md112", null ]
      ] ],
      [ "Refactoring Summary", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md113", [
        [ "Files Modified for MISRA Compliance:", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md114", null ],
        [ "Build System Updates:", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md115", null ]
      ] ],
      [ "Benefits of MISRA C++ 2023 Compliance", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md116", null ],
      [ "Static Analysis Tools", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md117", null ],
      [ "Continuous Integration", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md118", null ],
      [ "Conclusion", "md_docs_2_m_i_s_r_a___c_p_p__2023___c_o_m_p_l_i_a_n_c_e.html#autotoc_md119", null ]
    ] ],
    [ "MISRA C++ 2023 Refactoring Summary", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html", [
      [ "Project Overview", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md121", null ],
      [ "Refactoring Summary", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md122", [
        [ "1. Header Guards Implementation (Rule 16-2-3)", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md123", null ],
        [ "2. In-Class Initialization (Rule 8-5-1)", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md124", null ],
        [ "3. Exception Safety Specifications (Rule 15-3-4)", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md125", null ],
        [ "4. Smart Pointer Usage (Rule 15-1-1)", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md126", null ],
        [ "5. Explicit Constructors (Rule 12-1-4)", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md127", null ],
        [ "6. Const Correctness (Rule 7-1-2)", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md128", null ],
        [ "7. Modern Cast Usage (MISRA encourages modern C++ practices)", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md129", null ],
        [ "8. Interface Design (Rule 12-1-1)", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md130", null ],
        [ "9. Final Classes (Modern C++ best practice)", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md131", null ],
        [ "10. Data Validation", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md132", null ]
      ] ],
      [ "Build System Enhancements", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md133", [
        [ "CMakeLists.txt Updates", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md134", null ]
      ] ],
      [ "Architecture Improvements", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md135", [
        [ "1. Resource Management", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md136", null ],
        [ "2. Exception Safety", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md137", null ],
        [ "3. Type Safety", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md138", null ]
      ] ],
      [ "Testing Verification", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md139", null ],
      [ "Benefits Achieved", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md140", [
        [ "1. Enhanced Safety", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md141", null ],
        [ "2. Improved Maintainability", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md142", null ],
        [ "3. Better Performance", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md143", null ],
        [ "4. Increased Portability", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md144", null ],
        [ "5. Enhanced Reliability", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md145", null ]
      ] ],
      [ "Code Quality Metrics", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md146", [
        [ "Before Refactoring:", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md147", null ],
        [ "After Refactoring:", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md148", null ]
      ] ],
      [ "Future Recommendations", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md149", [
        [ "1. Static Analysis Integration", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md150", null ],
        [ "2. Additional Tools", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md151", null ],
        [ "3. Documentation Standards", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md152", null ],
        [ "4. Code Review Process", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md153", null ]
      ] ],
      [ "Conclusion", "md_docs_2_m_i_s_r_a___r_e_f_a_c_t_o_r_i_n_g___c_o_m_p_l_e_t_e.html#autotoc_md154", null ]
    ] ],
    [ "MISRA C++ 2023 Compliance Analysis: ZeroMQDataSender.cpp", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html", [
      [ "Overview", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md156", null ],
      [ "MISRA C++ 2023 Rules Applied", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md157", [
        [ "Rule 16-2-1: Include Ordering and Dependencies", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md158", null ],
        [ "Rule 12-1-4: Explicit Constructors", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md159", null ],
        [ "Rule 8-5-1: Variable Initialization", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md160", null ],
        [ "Rule 6-4-1: Structured Conditionals", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md161", null ],
        [ "Rule 8-4-2: Parameter Passing", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md162", null ],
        [ "Rule 7-1-2: Const Correctness", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md163", null ],
        [ "Rule 15-3-4: Exception Safety", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md164", null ],
        [ "Rule 5-0-5: Type Safety and Explicit Conversions", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md165", null ],
        [ "Rule 7-3-1: Global Namespace Usage", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md166", null ]
      ] ],
      [ "Line-by-Line Code Explanation", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md167", [
        [ "File Header (Lines 1-16)", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md168", null ],
        [ "Constructor Implementation (Lines 25-40)", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md169", null ],
        [ "Send Method Implementation (Lines 50-97)", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md170", null ]
      ] ],
      [ "Memory Safety Analysis", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md171", [
        [ "Buffer Operations", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md172", null ],
        [ "Resource Management", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md173", null ]
      ] ],
      [ "Error Handling Analysis", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md174", [
        [ "ZMQ Operation Safety", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md175", null ]
      ] ],
      [ "Performance Considerations", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md176", [
        [ "JSON Serialization Efficiency", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md177", null ],
        [ "Memory Usage", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md178", null ]
      ] ],
      [ "Thread Safety", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md179", [
        [ "ZMQ Socket Usage", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md180", null ]
      ] ],
      [ "Conclusion", "md_docs_2_m_i_s_r_a___z_e_r_o_m_q___s_e_n_d_e_r___a_n_a_l_y_s_i_s.html#autotoc_md181", null ]
    ] ],
    [ "C++ Doxygen Documentation Setup - Project Complete ✅", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html", [
      [ "🎯 Project Summary", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md183", null ],
      [ "🔧 Enhanced Configuration Features", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md184", [
        [ "Doxygen Settings Applied", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md185", null ],
        [ "Advanced Features", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md186", null ]
      ] ],
      [ "📊 Generated Documentation Stats", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md187", null ],
      [ "📁 Project Structure", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md188", null ],
      [ "🚀 How to Use", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md189", [
        [ "1. Generate Documentation", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md190", null ],
        [ "2. View Results", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md191", null ],
        [ "3. Explore Features", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md192", null ]
      ] ],
      [ "🎨 Documentation Examples Created", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md193", [
        [ "1. Hexagonal Architecture Visualization", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md194", null ],
        [ "2. Advanced Example Code (<tt>src/examples/example_documented_code.cpp</tt>)", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md195", null ]
      ] ],
      [ "🔧 Tools & Dependencies", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md196", [
        [ "Required", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md197", null ],
        [ "Features Verified", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md198", null ]
      ] ],
      [ "📋 Troubleshooting Notes", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md199", [
        [ "Version Compatibility", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md200", null ],
        [ "Performance", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md201", null ]
      ] ],
      [ "🎯 Documentation Quality Metrics", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md202", [
        [ "Coverage", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md203", null ],
        [ "Visual Quality", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md204", null ]
      ] ],
      [ "📈 Next Steps & Recommendations", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md205", [
        [ "Immediate Use", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md206", null ],
        [ "Future Enhancements", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md207", null ]
      ] ],
      [ "🏆 Project Success Criteria - ALL MET ✅", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_e.html#autotoc_md209", null ]
    ] ],
    [ "Project Completion Summary", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html", [
      [ "Overview", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md211", null ],
      [ "✅ Completed Features", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md212", [
        [ "1. Architecture Implementation", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md213", null ],
        [ "2. Core Functionality", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md214", null ],
        [ "3. ZeroMQ Integration", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md215", null ],
        [ "4. MISRA C++ 2023 Compliance", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md216", null ],
        [ "5. Testing & Quality Assurance", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md217", null ],
        [ "6. Documentation", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md218", null ]
      ] ],
      [ "🏗️ Project Structure", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md219", null ],
      [ "🔧 Technical Specifications", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md220", [
        [ "Data Types & Units", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md221", null ],
        [ "ZeroMQ Configuration", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md222", null ],
        [ "Build System", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md223", null ]
      ] ],
      [ "📊 Quality Metrics", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md224", [
        [ "Code Quality", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md225", null ],
        [ "Performance Characteristics", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md226", null ]
      ] ],
      [ "🚀 Future Enhancements", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md227", [
        [ "Operational Improvements", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md228", null ],
        [ "Technical Enhancements", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md229", null ],
        [ "Quality Assurance", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md230", null ]
      ] ],
      [ "📝 Conclusion", "md_docs_2_p_r_o_j_e_c_t___c_o_m_p_l_e_t_i_o_n___s_u_m_m_a_r_y.html#autotoc_md231", null ]
    ] ],
    [ "Project Status Report - Updated from GitHub", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html", [
      [ "Summary", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md233", null ],
      [ "Changes Integrated from GitHub", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md234", [
        [ "New Files Added:", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md235", null ],
        [ "Files Cleaned Up:", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md236", null ]
      ] ],
      [ "Current Project Structure", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md237", null ],
      [ "JSON Schema Integration", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md238", null ],
      [ "MISRA C++ 2023 Compliance Status", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md239", null ],
      [ "Build and Test Status", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md240", null ],
      [ "Git Status", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md241", null ],
      [ "Next Steps", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md242", null ],
      [ "Files Verified Present", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md243", [
        [ "Core Implementation (All Present):", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md244", null ],
        [ "Documentation (All Present):", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md245", null ],
        [ "New Integration (All Present):", "md_docs_2_p_r_o_j_e_c_t___s_t_a_t_u_s___u_p_d_a_t_e.html#autotoc_md246", null ]
      ] ]
    ] ],
    [ "Topics", "topics.html", "topics" ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ],
        [ "Enumerations", "functions_enum.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_calculator_service_8cpp.html",
"md_docs_2_a_p_i___d_o_c_u_m_e_n_t_a_t_i_o_n.html#autotoc_md30"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';