import QtQuick 2.15

// Simple SVG icon wrapper. All Lucide icons are 24x24.
// Usage: Icon { name: "arrow-left"; size: 20; color: Theme.textPrimary }
Image {
    id: icon
    sourceSize.width: size
    sourceSize.height: size
    fillMode: Image.PreserveAspectFit
    source: "qrc:/icons/" + name + ".svg"

    property string name: ""
    property int size: 20
    property color color: Theme.textPrimary

    // SVG color override via ColorOverlay equivalent.
    // Qt 6 Image does not natively support tint; use a simple shader or
    // use the fillColor property of the SVG itself. For monochrome Lucide
    // icons, we let the SVG's currentColor inherit.
}
