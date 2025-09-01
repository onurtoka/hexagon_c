# Enhanced Doxygen Documentation Setup

This project includes a comprehensive Doxygen configuration with advanced features for generating professional C++ documentation.

## Features Enabled

### 1. Automatic UML Class Diagrams
- **HAVE_DOT = YES**: Enables Graphviz integration
- **CLASS_DIAGRAMS = YES**: Generates class hierarchy diagrams
- **UML_LOOK = YES**: Creates UML-style diagrams
- **TEMPLATE_RELATIONS = YES**: Shows template relationships

### 2. Call Graphs and Dependencies
- **CALL_GRAPH = YES**: Shows function call relationships
- **CALLER_GRAPH = YES**: Shows reverse call dependencies
- **INCLUDE_GRAPH = YES**: Displays include file dependencies
- **INCLUDED_BY_GRAPH = YES**: Shows reverse include relationships

### 3. Interactive SVG Output
- **DOT_IMAGE_FORMAT = svg**: Uses SVG format for scalable graphics
- **INTERACTIVE_SVG = YES**: Enables clickable, zoomable diagrams

### 4. Mermaid Diagram Support
- **USE_MERMAID = YES**: Enables Mermaid diagram embedding
- Custom Mermaid diagrams can be embedded using `@mermaid` blocks

### 5. Enhanced Documentation Style
- **JAVADOC_AUTOBRIEF = YES**: Automatically extracts brief descriptions
- **QT_AUTOBRIEF = YES**: Qt-style brief extraction
- **MARKDOWN_SUPPORT = YES**: Full Markdown support in comments

## Documentation Styles Supported

### Javadoc-Style Comments
```cpp
/**
 * @brief Brief description of the function
 * @param paramName Description of parameter
 * @return Description of return value
 * @throws ExceptionType When this exception is thrown
 * @see RelatedFunction
 * @note Important note about usage
 * @warning Critical warning
 */
```

### Mermaid Diagrams
```cpp
/**
 * @mermaid
 * graph TD
 *     A[Start] --> B[Process]
 *     B --> C[End]
 * @endmermaid
 */
```

### Class Documentation
```cpp
/**
 * @brief Brief description of the class
 * 
 * Detailed description with multiple paragraphs.
 * 
 * @section usage Usage Example
 * @code{.cpp}
 * MyClass obj(param);
 * obj.doSomething();
 * @endcode
 * 
 * @mermaid
 * classDiagram
 *     class MyClass {
 *         +method()
 *         -privateVar: int
 *     }
 * @endmermaid
 */
```

## Dependencies

### Required Tools
1. **Doxygen** (version 1.9.0 or later for Mermaid support)
   ```bash
   sudo apt-get install doxygen
   ```

2. **Graphviz** (for UML diagrams and call graphs)
   ```bash
   sudo apt-get install graphviz
   ```

3. **Optional: PlantUML** (for advanced UML diagrams)
   ```bash
   sudo apt-get install plantuml
   ```

## Building Documentation

### Basic Documentation Generation
```bash
# Navigate to project root
cd /workspaces/b_hexagon/b_hexagon

# Generate documentation
doxygen Doxyfile

# Open documentation in browser
firefox docs/html/index.html
# or
$BROWSER docs/html/index.html
```

### Using the Build Script
```bash
# Make build script executable
chmod +x build_docs.sh

# Run documentation build
./build_docs.sh
```

## Generated Output

After running `doxygen Doxyfile`, you'll find:

### HTML Documentation
- **Location**: `docs/html/`
- **Main Page**: `docs/html/index.html`
- **Features**:
  - Interactive class hierarchy diagrams
  - Clickable call graphs
  - Searchable documentation
  - Mobile-responsive design

### Generated Diagrams
1. **Class Diagrams**: Show inheritance relationships
2. **Call Graphs**: Display function call dependencies
3. **Include Graphs**: Show file dependency relationships
4. **Custom Mermaid Diagrams**: Embedded flowcharts and sequence diagrams

## Example Files

### 1. Basic Documentation Example
See `src/examples/example_documented_code.cpp` for comprehensive documentation examples including:
- Javadoc-style comments
- Mermaid diagrams
- Class relationships
- Factory pattern documentation

### 2. Hexagonal Architecture Documentation
The main project demonstrates:
- Interface documentation (`domain/ports/`)
- Adapter documentation (`adapters/`)
- Service documentation (`application/`)

## Advanced Configuration

### Customizing Diagrams
Edit `Doxyfile` to modify:
```makefile
# Diagram appearance
DOT_FONTNAME           = Helvetica
DOT_FONTSIZE           = 10
DOT_GRAPH_MAX_NODES    = 50

# Output format
DOT_IMAGE_FORMAT       = svg
INTERACTIVE_SVG        = YES
```

### Adding Custom CSS
```makefile
HTML_EXTRA_STYLESHEET  = custom.css
```

### Excluding Files
```makefile
EXCLUDE_PATTERNS       = */build/* \
                         */third_party/* \
                         */.git/*
```

## Troubleshooting

### Common Issues

1. **Graphviz not found**
   ```
   Error: dot not found in PATH
   ```
   **Solution**: Install Graphviz and ensure `dot` is in PATH

2. **SVG diagrams not interactive**
   **Solution**: Ensure `INTERACTIVE_SVG = YES` and view in modern browser

3. **Mermaid diagrams not rendering**
   **Solution**: Requires Doxygen 1.9.0+ and modern browser with JavaScript enabled

### Verification Commands
```bash
# Check Doxygen version
doxygen --version

# Check Graphviz installation
dot -V

# Test configuration
doxygen -g test.config
```

## Integration with VS Code

### Recommended Extensions
- **Doxygen Documentation Generator**: Auto-generates comment templates
- **C/C++ Documentation Comments**: IntelliSense for documentation
- **Mermaid Preview**: Preview Mermaid diagrams in VS Code

### Tasks Configuration
The project includes VS Code tasks for building documentation:
```json
{
    "label": "Build Documentation",
    "type": "shell",
    "command": "doxygen",
    "args": ["Doxyfile"],
    "group": "build"
}
```

## Best Practices

1. **Document Interfaces First**: Start with public APIs and interfaces
2. **Use Consistent Style**: Stick to Javadoc or Qt style throughout
3. **Include Examples**: Add `@code` blocks for complex functions
4. **Diagram Strategically**: Use Mermaid for workflows, Graphviz for relationships
5. **Update Regularly**: Keep documentation in sync with code changes

## Next Steps

1. **Enhance Main Project Documentation**:
   - Add more detailed class documentation
   - Include sequence diagrams for data flow
   - Document design patterns used

2. **Custom Styling**:
   - Create custom CSS themes
   - Add company branding
   - Optimize for mobile viewing

3. **CI/CD Integration**:
   - Auto-generate docs on commits
   - Deploy to GitHub Pages
   - Quality checks for documentation coverage
