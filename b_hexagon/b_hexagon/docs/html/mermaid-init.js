// Initialize Mermaid with hexagonal architecture color theme
document.addEventListener('DOMContentLoaded', function() {
    mermaid.initialize({
        startOnLoad: true,
        theme: 'base',
        themeVariables: {
            primaryColor: '#2e7d32',
            primaryTextColor: '#ffffff',
            primaryBorderColor: '#1b5e20',
            lineColor: '#333333',
            sectionBkgColor: '#f5f5f5',
            altSectionBkgColor: '#ffffff',
            gridColor: '#e0e0e0',
            secondaryColor: '#1565c0',
            tertiaryColor: '#ef6c00'
        },
        flowchart: {
            htmlLabels: true,
            curve: 'basis'
        },
        sequence: {
            diagramMarginX: 50,
            diagramMarginY: 10,
            actorMargin: 50,
            width: 150,
            height: 65,
            boxMargin: 10,
            boxTextMargin: 5,
            noteMargin: 10,
            messageMargin: 35,
            mirrorActors: true,
            bottomMarginAdj: 1,
            useMaxWidth: true
        },
        classDiagram: {
            useMaxWidth: true
        }
    });

    // Define custom CSS classes for hexagonal architecture components
    const style = document.createElement('style');
    style.textContent = `
        .hexagon-domain { fill: #2e7d32 !important; stroke: #1b5e20 !important; }
        .hexagon-port { fill: #1565c0 !important; stroke: #0d47a1 !important; }
        .hexagon-adapter { fill: #ef6c00 !important; stroke: #e65100 !important; }
        .hexagon-infrastructure { fill: #616161 !important; stroke: #424242 !important; }
        .hexagon-external { fill: #6a1b9a !important; stroke: #4a148c !important; }
    `;
    document.head.appendChild(style);
});
