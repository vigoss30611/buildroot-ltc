import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

    Rectangle{
        id: newTab
        //anchors.fill: parent

        gradient: Gradient {
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 1.0; color: "yellow" }
        }
        MouseArea{
           anchors.fill: parent
           propagateComposedEvents: true
           onClicked: {
               console.log("Mouse area2 cliced!")
               mouse.accepted = false
           }
        }
        Button{
            anchors.centerIn: parent
            text: "ClickMe"
            style: ButtonStyle {
                    background: Rectangle {
                        implicitWidth: 200
                        implicitHeight: 50
                        border.width: control.activeFocus ? 2 : 1
                        border.color: "#888"
                        radius: 4
                        gradient: Gradient {
                            GradientStop { position: 0 ; color: control.pressed ? "transparent" : "transparent" }
                            GradientStop { position: 1 ; color: control.pressed ? "blue" : "steelblue" }
                        }
                    }
                }
        }
    }
