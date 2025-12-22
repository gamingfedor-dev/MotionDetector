import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: root
    visible: true
    width: 1200
    height: 800
    title: "Motion Detector"
    color: "#1a1a2e"

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // Video display area
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#16213e"
            radius: 8

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                // Video frame
                Image {
                    id: videoFrame
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    fillMode: Image.PreserveAspectFit
                    cache: false
                    
                    property var imageService: component.frame
                    onImageServiceChanged: {
                        source = ""
                        source = "image://frame/current?" + Date.now()
                    }
                }

                // Stats bar
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    color: "#0f3460"
                    radius: 4

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 20

                        Label {
                            text: "Motion: " + component.motionScore.toFixed(2) + "%"
                            color: "#e94560"
                            font.pixelSize: 14
                            font.bold: true
                        }
                        Label {
                            text: "Objects: " + component.objectCount
                            color: "#00fff5"
                            font.pixelSize: 14
                            font.bold: true
                        }
                        Label {
                            text: "Frame: " + component.frameId
                            color: "#ffffff"
                            font.pixelSize: 14
                        }
                    }
                }
            }
        }

        // Controls panel
        Rectangle {
            Layout.preferredWidth: 250
            Layout.fillHeight: true
            color: "#16213e"
            radius: 8

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 15

                Label {
                    text: "Controls"
                    color: "#ffffff"
                    font.pixelSize: 18
                    font.bold: true
                }

                // ROI X
                ControlSlider {
                    label: "ROI X"
                    value: component.roiX
                    onValueChanged: component.roiX = value
                }

                // ROI Y
                ControlSlider {
                    label: "ROI Y"
                    value: component.roiY
                    onValueChanged: component.roiY = value
                }

                // ROI Width
                ControlSlider {
                    label: "ROI Width"
                    from: 5
                    value: component.roiWidth
                    onValueChanged: component.roiWidth = value
                }

                // ROI Height
                ControlSlider {
                    label: "ROI Height"
                    from: 5
                    value: component.roiHeight
                    onValueChanged: component.roiHeight = value
                }

                // Threshold
                ControlSlider {
                    label: "Threshold"
                    from: 1
                    value: component.threshold
                    onValueChanged: component.threshold = value
                }

                // Min Area
                ControlSlider {
                    label: "Min Area (x100)"
                    from: 1
                    value: component.minArea
                    onValueChanged: component.minArea = value
                }

                Item { Layout.fillHeight: true }
            }
        }
    }
}