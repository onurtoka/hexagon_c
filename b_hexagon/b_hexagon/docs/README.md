# Hexagonal C++ Documentation Build Guide

This guide explains how to build comprehensive documentation for the Hexagonal C++ project using Doxygen + Graphviz for auto-generated diagrams and Mermaid for custom architectural diagrams.

## Prerequisites

### Install Required Tools

#### Ubuntu/Debian
```bash
# Install Doxygen and Graphviz
sudo apt update
sudo apt install doxygen graphviz

# Verify installations
doxygen --version
dot -V
```

#### macOS
```bash
# Using Homebrew
brew install doxygen graphviz

# Verify installations
doxygen --version
dot -V
```

#### Windows
1. Download Doxygen from: https://www.doxygen.nl/download.html
2. Download Graphviz from: https://graphviz.org/download/
3. Add both to your PATH environment variable

## Project Structure

The project follows hexagonal architecture with documentation integration:

```
b_hexagon/
├── src/                          # Source code (documented with Doxygen)
│   ├── domain/                   # Core business logic (Green)
│   ├── ports/                    # Interface definitions (Blue)  
│   ├── adapters/                 # External integrations (Orange)
│   │   ├── incoming/            # Input adapters
│   │   └── outgoing/            # Output adapters
│   └── infrastructure/          # Technical concerns (Gray)
├── docs/                        # Documentation files
│   ├── architecture.md         # Mermaid diagrams & arch overview
│   ├── README.md               # This file
│   ├── hexagonal_architecture_generic.svg      # Complete architectural overview
│   ├── hexagonal_architecture_code_flow.svg    # Detailed code flow diagram
│   ├── b_hexagon_detailed_code_flow.svg        # Class/function level execution (16 steps)
│   ├── data_processing_flow.svg                # RADIO-DISH pattern visualization
│   └── html/                   # Generated documentation (after build)
```
│   └── html/                   # Generated documentation (after build)
├── assets/                      # Documentation assets
│   ├── header.html             # Custom Doxygen header with Mermaid
│   ├── mermaid.min.js         # Mermaid library
│   ├── mermaid-init.js        # Mermaid initialization
│   └── mermaid.css            # Hexagonal architecture styling
├── Doxyfile                    # Doxygen configuration
└── CMakeLists.txt             # Build configuration
```

## Documentation Features

### Auto-Generated Diagrams (Doxygen + Graphviz)
- **Class Diagrams**: Inheritance and composition relationships
- **Call Graphs**: Function call hierarchies  
- **Include Graphs**: Header dependency visualization
- **Collaboration Diagrams**: Class interaction patterns

### Custom Diagrams (Mermaid)
- **High-Level Architecture**: Hexagonal component overview with color coding
- **Data Flow Diagrams**: End-to-end processing workflows
- **Sequence Diagrams**: Detailed interaction sequences
- **State Diagrams**: Component lifecycle management

### SVG Architecture Visualizations
The `docs/` directory contains comprehensive SVG diagrams showing the complete B_Hexagon architecture:

#### 🏗️ **Architectural Overview**
- **`hexagonal_architecture_generic.svg`**: Complete system architecture with all layers
  - Hexagons for adapters and domain core
  - Diamonds for ports (interfaces)
  - Color-coded components (green=adapters, blue=ports, gold=domain)
  - Data flow arrows and implementation relationships

#### ⚡ **Code Execution Flow**
- **`hexagonal_architecture_code_flow.svg`**: High-level code flow through architecture
- **`b_hexagon_detailed_code_flow.svg`**: Detailed 16-step execution flow
  - Program start: main() → ZeroMQDataSender() → CalculatorService()
  - Runtime processing: socket.recv() → parseJson() → onDataReceived() → calculations
  - Complete loop showing initialization and per-message processing

#### 📊 **Data Processing**
- **`data_processing_flow.svg`**: RADIO-DISH pattern implementation
  - Input processing (ZMQ DISH receives → Parse JSON → Validate)
  - Business logic (Calculate delays → Apply rules → Enrich data)
  - Output processing (Serialize JSON → Add metadata → Send RADIO)

These diagrams provide:
- **Visual Consistency**: Same shapes and colors across all diagrams
- **Technical Detail**: File paths, function names, exact execution order
- **Interactive Elements**: Scalable SVG format for zoom/pan
- **Complete Coverage**: From architectural overview to line-by-line execution

### Color Legend (Consistent Across All Diagrams)
- 🟢 **Domain/Core**: `#2e7d32` - Business logic, entities, value objects
- 🔵 **Ports**: `#1565c0` - Interface definitions and contracts
- 🟠 **Adapters**: `#ef6c00` - External integrations and I/O
- ⚫ **Infrastructure**: `#616161` - Framework and technical concerns
- 🟣 **External Systems**: `#6a1b9a` - Third-party services and databases

## Build Documentation

### Step 1: Build Project (Optional)
```bash
cd b_hexagon
mkdir -p build
cd build
cmake ..
make
cd ..
```

### Step 2: Generate Documentation
```bash
# From the b_hexagon directory
doxygen Doxyfile
```

### Step 3: View Documentation
```bash
# Open the generated documentation
# Linux/macOS
xdg-open docs/html/index.html     # Linux
open docs/html/index.html         # macOS

# Windows
start docs/html/index.html        # Windows
```

## Documentation Configuration

### Doxyfile Key Settings

The `Doxyfile` is configured for optimal hexagonal architecture documentation:

```ini
# Project Information
PROJECT_NAME           = "Hexagonal C++ Example"
PROJECT_BRIEF          = "C++17 Hexagonal Architecture Track Delay Calculator"

# Input Sources
INPUT                  = src/ docs/
RECURSIVE              = YES
MARKDOWN_SUPPORT       = YES

# Diagram Generation
HAVE_DOT               = YES
CLASS_DIAGRAMS         = YES
CALL_GRAPH             = YES
CALLER_GRAPH           = YES
DOT_GRAPH_MAX_NODES    = 200
DOT_TRANSPARENT        = YES
GENERATE_TREEVIEW      = YES

# Mermaid Integration
HTML_HEADER            = assets/header.html
HTML_EXTRA_STYLESHEET  = assets/mermaid.css
HTML_EXTRA_FILES       = assets/mermaid.min.js assets/mermaid-init.js
```

### Custom Styling

The documentation includes custom CSS for hexagonal architecture:

- **Consistent Color Scheme**: All diagrams use the same color legend
- **Responsive Design**: Works on desktop and mobile devices
- **Interactive Elements**: Clickable diagrams and navigation
- **Professional Layout**: Clean, modern documentation style

## Development Workflow

### Adding New Components

When adding new components to the architecture:

1. **Document the Code**: Use Doxygen comments
```cpp
/**
 * @brief Processes track data according to business rules
 * @param trackData The input track data to process
 * @return ProcessedTrack The result of processing
 * @throws ValidationException if input data is invalid
 */
ProcessedTrack processTrack(const TrackData& trackData);
```

2. **Update Architecture Diagrams**: Modify `docs/architecture.md` with new Mermaid diagrams

3. **Rebuild Documentation**: Run `doxygen Doxyfile` to regenerate

### Diagram Best Practices

#### Mermaid Diagrams
- Use consistent color classes: `:::hexagon-domain`, `:::hexagon-port`, etc.
- Include clear legends and titles
- Keep diagrams focused on single concerns
- Use meaningful node labels

#### Auto-Generated Diagrams
- Add Doxygen comments to improve graph clarity
- Use `@note` and `@see` for cross-references
- Group related classes with `@defgroup`

## Troubleshooting

### Common Issues

1. **Graphviz Not Found**
   ```
   Error: dot not found in PATH
   ```
   Solution: Install Graphviz and ensure `dot` is in your PATH

2. **Mermaid Diagrams Not Rendering**
   - Check browser console for JavaScript errors
   - Verify `mermaid.min.js` is properly downloaded
   - Ensure proper syntax in Mermaid code blocks

3. **Missing Files in Generated Docs**
   ```
   Warning: source file not found
   ```
   Solution: Check `INPUT` paths in Doxyfile and ensure files exist

### Advanced Configuration

#### Custom Themes
Modify `assets/mermaid-init.js` to change the color scheme:
```javascript
mermaid.initialize({
    theme: 'base',
    themeVariables: {
        primaryColor: '#your-color',
        // ... other theme variables
    }
});
```

#### Additional Formats
Enable other output formats in Doxyfile:
```ini
GENERATE_LATEX         = YES
GENERATE_XML           = YES
GENERATE_RTF           = YES
```

## Integration with IDEs

### VS Code
1. Install "Doxygen Documentation Generator" extension
2. Use `/**` to auto-generate comment templates
3. Install "Mermaid Markdown Syntax Highlighting" for diagram editing

### CLion
1. Enable Doxygen support in Settings → Tools → Doxygen
2. Use built-in comment templates
3. Configure external tools for documentation generation

## Continuous Integration

### GitHub Actions Example
```yaml
name: Documentation
on: [push, pull_request]
jobs:
  docs:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - name: Install Dependencies
      run: sudo apt install doxygen graphviz
    - name: Generate Documentation
      run: doxygen Doxyfile
    - name: Deploy to GitHub Pages
      uses: peaceiris/actions-gh-pages@v3
      with:
        github_token: ${{ secrets.GITHUB_TOKEN }}
        publish_dir: ./docs/html
```

## Resources

- [Doxygen Manual](https://www.doxygen.nl/manual/)
- [Graphviz Documentation](https://graphviz.org/documentation/)
- [Mermaid Documentation](https://mermaid.js.org/)
- [Hexagonal Architecture](https://alistair.cockburn.us/hexagonal-architecture/)
- [C++ Documentation Best Practices](https://github.com/isocpp/CppCoreGuidelines)
- Visualizes data flow from adapters through application to domain layers
- Located in: **Classes → CalculatorService**

### 2. Timing Calculation Flow (`TrackCalculations`)
- Illustrates the step-by-step timing calculation process
- Shows how first and second hop delays are computed
- Located in: **Classes → TrackCalculations**

### 3. ZeroMQ Data Reception Flow (`ZeroMQDataReceiver`)
- Demonstrates the JSON parsing and validation process
- Shows error handling paths for invalid data
- Located in: **Classes → ZeroMQDataReceiver**

### 4. Application Main Flow (`main.cpp`)
- Displays the application startup and configuration process
- Shows dependency injection and service initialization
- Located in: **Files → main.cpp**

## Building Documentation

To regenerate the documentation:

```bash
./build_docs.sh
```

Or manually:

```bash
doxygen Doxyfile
```

## Requirements

- **Doxygen**: For documentation generation
- **Graphviz**: For flowchart rendering (`dot` command)

Install on Ubuntu:
```bash
sudo apt-get install doxygen graphviz
```

## Output

- **HTML Documentation**: `docs/html/index.html`
- **Flowcharts**: Embedded as SVG images in the HTML pages
- **Call Graphs**: Automatic function call relationships
- **Class Diagrams**: UML-style inheritance and collaboration diagrams

## Features

- Interactive SVG flowcharts
- Cross-referenced code navigation
- Search functionality
- Mobile-responsive design
- Syntax-highlighted source code
- Automatic dependency graphs

## Navigation Tips

1. Use the **Classes** tab to find flowcharts in class documentation
2. **Files** tab shows file-level flowcharts and call graphs
3. Search bar supports fuzzy matching for quick navigation
4. Click on flowchart elements for navigation to related code

The flowcharts use color coding:
- **Light Grey**: Input adapters
- **Light Blue**: Application layer  
- **Light Yellow**: Domain layer
- **Light Green**: Output adapters
- **Orange**: Calculation steps
- **Red**: Error paths
