#!/bin/bash

# Hexagonal C++ Documentation Build Script
# This script generates comprehensive documentation with Doxygen + Graphviz and Mermaid

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_color() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

print_header() {
    echo
    echo "=========================================="
    print_color $BLUE "$1"
    echo "=========================================="
    echo
}

# Check if we're in the right directory
if [[ ! -f "Doxyfile" ]]; then
    print_color $RED "Error: Doxyfile not found. Please run this script from the b_hexagon directory."
    exit 1
fi

print_header "Hexagonal C++ Documentation Builder"

# Check dependencies
print_color $YELLOW "Checking dependencies..."

# Check for Doxygen
if ! command -v doxygen &> /dev/null; then
    print_color $RED "Error: Doxygen is not installed."
    echo "Please install Doxygen:"
    echo "  Ubuntu/Debian: sudo apt install doxygen"
    echo "  macOS: brew install doxygen"
    echo "  Windows: Download from https://www.doxygen.nl/download.html"
    exit 1
fi

# Check for Graphviz (dot command)
if ! command -v dot &> /dev/null; then
    print_color $RED "Error: Graphviz is not installed."
    echo "Please install Graphviz:"
    echo "  Ubuntu/Debian: sudo apt install graphviz"
    echo "  macOS: brew install graphviz"
    echo "  Windows: Download from https://graphviz.org/download/"
    exit 1
fi

print_color $GREEN "✓ Doxygen $(doxygen --version)"
print_color $GREEN "✓ Graphviz $(dot -V 2>&1 | head -n1)"

# Check for Mermaid assets
if [[ ! -f "assets/mermaid.min.js" ]]; then
    print_color $YELLOW "Mermaid.js not found. Downloading..."
    curl -o assets/mermaid.min.js https://cdn.jsdelivr.net/npm/mermaid@10.6.1/dist/mermaid.min.js
    print_color $GREEN "✓ Mermaid.js downloaded"
else
    print_color $GREEN "✓ Mermaid.js found"
fi

# Clean previous documentation
print_header "Cleaning Previous Documentation"
if [[ -d "docs/html" ]]; then
    print_color $YELLOW "Removing existing documentation..."
    rm -rf docs/html/*
    print_color $GREEN "✓ Previous documentation cleaned"
else
    print_color $BLUE "No previous documentation found"
fi

# Generate documentation
print_header "Generating Documentation"
print_color $YELLOW "Running Doxygen..."

# Capture Doxygen output
if doxygen Doxyfile > doxygen_output.log 2>&1; then
    print_color $GREEN "✓ Documentation generated successfully"
    
    # Show summary of warnings (if any)
    warning_count=$(grep -c "warning:" doxygen_output.log || true)
    if [[ $warning_count -gt 0 ]]; then
        print_color $YELLOW "⚠️  Generated with $warning_count warnings (see doxygen_output.log for details)"
    fi
else
    print_color $RED "✗ Doxygen failed. Check doxygen_output.log for details."
    exit 1
fi

# Verify key files exist
print_header "Verifying Documentation"

key_files=(
    "docs/html/index.html"
    "docs/html/annotated.html"
    "docs/html/hierarchy.html"
    "docs/html/md_docs_2architecture.html"
)

all_good=true
for file in "${key_files[@]}"; do
    if [[ -f "$file" ]]; then
        print_color $GREEN "✓ $file"
    else
        print_color $RED "✗ Missing: $file"
        all_good=false
    fi
done

# Check for generated diagrams
diagram_count=$(find docs/html -name "*.svg" | wc -l)
print_color $GREEN "✓ Generated $diagram_count SVG diagrams"

if [[ $all_good == true ]]; then
    print_header "Documentation Build Complete! 🎉"
    
    echo "📁 Documentation Location:"
    echo "   File: $(pwd)/docs/html/index.html"
    echo
    echo "🌐 To view the documentation:"
    echo "   Linux:   xdg-open docs/html/index.html"
    echo "   macOS:   open docs/html/index.html"
    echo "   Windows: start docs/html/index.html"
    echo
    echo "� Documentation Features:"
    echo "   ✓ Auto-generated class diagrams (Graphviz)"
    echo "   ✓ Call graphs and include graphs"
    echo "   ✓ Custom Mermaid architecture diagrams"
    echo "   ✓ Hexagonal architecture color coding"
    echo "   ✓ MISRA C++ 2023 compliance documentation"
    echo "   ✓ API documentation with examples"
    echo
    print_color $GREEN "Build completed successfully!"
else
    print_color $RED "Build completed with errors. Please check the missing files."
    exit 1
fi

# Optional: Open documentation automatically
if [[ "${1:-}" == "--open" ]]; then
    print_color $BLUE "Opening documentation..."
    if command -v xdg-open &> /dev/null; then
        xdg-open docs/html/index.html
    elif command -v open &> /dev/null; then
        open docs/html/index.html
    elif command -v start &> /dev/null; then
        start docs/html/index.html
    else
        print_color $YELLOW "Could not automatically open documentation. Please open docs/html/index.html manually."
    fi
fi

# Clean up log file unless there were warnings
if [[ $warning_count -eq 0 ]]; then
    rm -f doxygen_output.log
fi
