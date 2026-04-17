import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "components"
import "theme"

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Vosk Test")

    signal qmlLoaded()
    signal startStop()

    Theme { id: appTheme }

    background: Rectangle {
        color: appTheme.background
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ===================== TOP PANEL =====================
        Rectangle {
            id: topPanel
            Layout.fillWidth: true
            Layout.preferredHeight: 70
            color: appTheme.surface
            border.color: appTheme.border
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 12

                Rectangle {
                    width: 120
                    Layout.fillHeight: true
                    radius: 8
                    color: appTheme.surface
                    Layout.alignment: Qt.AlignVCenter

                    Image {
                        id: imgLogo
                        anchors.centerIn: parent
                        //source: "qrc:/Images/120x120.png"
                        //fillMode: Image.PreserveAspectFit
                        width: parent.width
                        height: parent.height
                    }
                }

                Text {
                    text: qsTr("Vosk Control Panel")
                    color: appTheme.textPrimary ? appTheme.textPrimary : "white"
                    font.pixelSize: 22
                    font.bold: true
                    Layout.alignment: Qt.AlignVCenter
                }

                Item {
                    Layout.fillWidth: true
                }

                CButton {
                    id: menuButton
                    theme: appTheme
                    text: "\u2630"
                    implicitWidth: 48
                    Layout.alignment: Qt.AlignVCenter
                    backgroundColor: appTheme.surface

                    onClicked: {
                        if (rightPanel.width > 10) {
                            // بستن
                            rightPanel.visible = true    // اول visible کن تا انیمیشن اجرا بشود
                            rightPanelAnim.to = 0
                            rightPanelAnim.start()

                            // بعد از تمام شدن انیمیشن invisible شود
                            rightPanelAnim.onStopped.connect(function() {
                                if (rightPanel.width === 0)
                                    rightPanel.visible = false
                            })

                        } else {
                            // باز کردن
                            rightPanel.visible = true
                            rightPanelAnim.to = rightPanel.openWidth
                            rightPanelAnim.start()
                        }
                    }
                }
            }
        }

        // ===================== MAIN AREA =====================
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // -------- IMAGE PANEL --------
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "black"
                border.color: appTheme.border
                border.width: 1

                ScrollView {
                    id: scrollArea
                    anchors.fill: parent
                    clip: true

                    TextArea {
                        id: commandText
                        color: appTheme.textPrimary
                        readOnly: true
                    }
                }
            }

            // -------- RIGHT PANEL --------
            Rectangle {
                id: rightPanel
                property int openWidth: 260
                width: openWidth
                Layout.preferredWidth: width
                Layout.fillHeight: true
                color: appTheme.surface
                border.color: appTheme.border
                border.width: 1
                Layout.alignment: Qt.AlignVCenter

                ScrollView{
                    anchors.fill: parent
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 16
                        //spacing: 12

                        Text {
                            text: qsTr("Controls")
                            font.pixelSize: 20
                            font.bold: true
                            color: appTheme.textPrimary ? appTheme.textPrimary : "white"
                        }

                        CButton {
                            id: startStopEngine
                            theme: appTheme
                            Layout.fillWidth: true
                            text: qsTr("Start")
                            visible: false
                            onClicked: {
                                startStop()
                            }
                        }

                        ListView {
                            id: grammerView
                            Layout.fillWidth: true
                            height: 50
                            spacing: 6
                            clip: true

                            model: grammerModel

                            delegate:Row {
                                width: grammerView.width
                                spacing: 2

                                Text {
                                    text: dateTime
                                    color: "white"
                                    //wrapMode: Text.Wrap

                                }

                                Text {
                                    text: stringValue
                                    color: "white"
                                    wrapMode: Text.Wrap
                                    textFormat: Text.RichText
                                }
                            }

                            // رفتن اتوماتیک به انتهای لیست:
                            onCountChanged: positionViewAtEnd()
                        }

                        RowLayout {
                            Layout.fillWidth: true

                            CTextField{
                                id: newGrammer
                                theme: appTheme
                                width: 100
                                placeholderText: qsTr("write new grammer")
                            }
                            CButton{
                                id: addGrammer
                                theme: appTheme
                                width: 100
                                text: qsTr("add")
                                onClicked: {
                                    grammerModel.addItem(Qt.formatDateTime(new Date(), "yyyy/MM/dd hh:mm:ss"),newGrammer.text)
                                    newGrammer.text = qsTr("")
                                }
                            }
                        }

                        CButton {
                            id: clearGrammer
                            theme: appTheme
                            Layout.fillWidth: true
                            text: "Clear Grammer"
                            onClicked: {
                                grammerModel.clear()
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                NumberAnimation on width {
                    id: rightPanelAnim
                    duration: 220
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }

    Toast{
        id: mainToast
        themeManager: appTheme
    }


    Connections{
        target: myBackend

        function onVoiceEngineChangeState(isInit,isStarted){
            startStopEngine.visible = isInit
            if(isStarted)
            {
                startStopEngine.text = qsTr("Stop")
            }
            else
            {
                startStopEngine.text = qsTr("Start")
            }
        }

        function onNewResult(cmd){
            commandText.append(cmd)
        }

        function onShowMessage(success,msg){
            mainToast.showMessage(success,msg)
        }
    }
    Component.onCompleted: {
        qmlLoaded.connect(myBackend.onQmlLoaded)
        startStop.connect(myBackend.onStartStop)

        qmlLoaded()
    }
}
