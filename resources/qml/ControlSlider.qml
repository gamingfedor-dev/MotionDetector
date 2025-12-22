import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ColumnLayout {
    id: root
    Layout.fillWidth: true
    spacing: 5

    property string label: "Slider"
    property int from: 0
    property int to: 100
    property int value: 50

    Label {
        text: root.label + ": " + slider.value
        color: "#a0a0a0"
        font.pixelSize: 12
    }

    Slider {
        id: slider
        Layout.fillWidth: true
        from: root.from
        to: root.to
        value: root.value
        stepSize: 1

        onValueChanged: root.value = value

        background: Rectangle {
            x: slider.leftPadding
            y: slider.topPadding + slider.availableHeight / 2 - height / 2
            width: slider.availableWidth
            height: 4
            radius: 2
            color: "#0f3460"

            Rectangle {
                width: slider.visualPosition * parent.width
                height: parent.height
                color: "#e94560"
                radius: 2
            }
        }

        handle: Rectangle {
            x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
            y: slider.topPadding + slider.availableHeight / 2 - height / 2
            width: 16
            height: 16
            radius: 8
            color: slider.pressed ? "#ffffff" : "#e94560"
        }
    }
}