import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../components"


Item {
    id: root
    anchors.fill: parent

    required property var theme

    // ===== State =====
    property int camerId: 0
    property string rtspUrl: "rtsp://admin:admin@192.168.1.10:554/11"
    property bool isPhysicalCamera: true

    signal applyClicked()
    signal cancelClicked()
    signal camerListRequest()
    signal cameraSelected(int cameraId)
    signal cameraUrlSelected(string url)

    // ===== Card =====
    Rectangle {
        anchors.fill: parent
        radius: theme.radius.md
        color: theme.surface
        border.color: theme.border
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // ===== Header =====
            Rectangle {
                Layout.fillWidth: true
                height: theme.headerHeight
                color: theme.surface

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: theme.spacing.sm

                    Text {
                        text: "Settings"
                        font.pixelSize: theme.fontSize.md
                        font.bold: true
                        color: theme.textPrimary
                        Layout.fillWidth: true
                    }
                }
            }

            // ===== Content =====
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: theme.spacing.md
                spacing: theme.spacing.md

                // Camera Type Selection
                ColumnLayout {
                    spacing: theme.spacing.xs
                    Text {
                        text: "Camera Type"
                        color: theme.textSecondary
                        font.pixelSize: theme.fontSize.xs
                    }
                    RowLayout {
                        spacing: theme.spacing.md

                        RadioButton {
                            id: physicalRadio
                            text: "Physical Camera"
                            checked: true
                            onCheckedChanged: {
                                if (checked) {
                                    isPhysicalCamera = true
                                }
                            }
                        }

                        RadioButton {
                            id: urlRadio
                            text: "RTSP URL"
                            onCheckedChanged: {
                                if (checked) {
                                    isPhysicalCamera = false
                                }
                            }
                        }
                    }
                }

                // Physical Camera List
                ColumnLayout {
                    visible: isPhysicalCamera
                    spacing: theme.spacing.xs
                    Text {
                        text: "Camer list"
                        color: theme.textSecondary
                        font.pixelSize: theme.fontSize.xs
                    }
                    ComboBox {
                        id: camerCombo
                        Layout.fillWidth: true
                        //model: audioBackend.inputDevicesModel

                        Component.onCompleted: ensureSelection()
                        onCountChanged: ensureSelection()

                        function ensureSelection() {
                            if (count > 0 && currentIndex === -1) {
                                currentIndex = 0
                                camerId = 0
                            }
                        }

                        currentIndex: camerId
                        onCurrentIndexChanged: camerId = currentIndex
                        textRole: "description"
                    }
                }

                // RTSP URL Input
                ColumnLayout {
                    visible: !isPhysicalCamera
                    spacing: theme.spacing.xs
                    Text {
                        text: "RTSP URL"
                        color: theme.textSecondary
                        font.pixelSize: theme.fontSize.xs
                    }
                    CTextField {
                        id: urlField
                        Layout.fillWidth: true
                        theme: root.theme
                        placeholderText: "rtsp://admin:admin@192.168.1.10:554/11"
                        text: rtspUrl
                        onTextChanged: rtspUrl = text
                    }
                }
            }

            // ===== Footer =====
            Rectangle {
                Layout.fillWidth: true
                height: 72
                color: theme.surface

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    // شبیه border.top
                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: theme.border
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.margins: theme.spacing.sm
                        spacing: theme.spacing.sm

                        CButton {
                            text: "Cancel"
                            theme: root.theme
                            Layout.fillWidth: true
                            onClicked: {
                                root.cancelClicked()
                                camerListRequest()
                            }
                        }

                        CButton {
                            text: "Apply"
                            theme: root.theme
                            Layout.fillWidth: true
                            onClicked:{
                                if (isPhysicalCamera) {
                                    cameraSelected(camerId)
                                } else {
                                    cameraUrlSelected(rtspUrl)
                                }
                                camerListRequest()
                                root.applyClicked()
                            }
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        camerListRequest.connect(myBackend.onCamerListRequest)
        cameraSelected.connect(myBackend.onCameraSelected)
        cameraUrlSelected.connect(myBackend.onCameraUrlSelected)

        camerListRequest()
    }

    Connections{
        target: myBackend

        function onCameraListResponse(camerasModel){
            camerCombo.model = camerasModel
        }
    }
}
