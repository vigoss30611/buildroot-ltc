import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

//ApplicationWindow
//{
//    visible: true
//    width: 640
//    height: 480
//    title: qsTr("Hello World!")
//}

Rectangle{
    visible: true
    id: mainRect
    signal quit()
    signal positionUpdate()
    width: 640
    height: 480
    color: "transparent"

    MouseArea{
        //propagateComposedEvents: true
       drag.target: mainRect
       anchors.fill: parent
       onPressed: {
           if(tabView.count < 4)
            tabView.addTab("Tab4", Qt.createComponent("component.qml"))
           //else
           //    btnLoader.source = "application.qml"

           console.log("Mouse area1 cliced!")
       }
       onPositionChanged: {
           console.log("pos changed")
          positionUpdate()
       }
    }
    Rectangle{
        id: closeBtn
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 10
        anchors.rightMargin: 10
        width: 20
        height: 20
        gradient: Gradient {
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 1.0; color: "red" }
        }
        radius: 4
        MouseArea{
           anchors.fill: parent
           onClicked: {
               console.log("clicked")
               quit()
           }
           onPressed: {
               text.font.pixelSize = 40
           }
           onReleased: {
               text.font.pixelSize = 20
           }
        }
    }
    TabView {
        id: tabView
        anchors.fill: parent
        anchors.margins: 10

        style: TabViewStyle {
                frameOverlap: 1
                tab: Rectangle {
                    color: styleData.selected ? "transparent" :"transparent"
                    border.color:  "transparent"
                    implicitWidth: Math.max(text.width + 4, 80)
                    implicitHeight: 20
                    radius: 2
                    Text {
                        id: text
                        anchors.centerIn: parent
                        text: styleData.title
                        font.pixelSize: 20
                        color: styleData.selected ? "red" : "yellow"
                    }
                }
                frame: Rectangle { color: "transparent" }
            }

        Tab {
            title: "Tab1"
            active: true
            Rectangle {
                radius: 10
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "transparent" }
                    GradientStop { position: 1.0; color: "blue" }
                }
                Row{
                    anchors.centerIn: parent
                    SpinBox{
                        objectName: "spin1"
                        maximumValue: 2000
                        minimumValue: 0
                    }
                    SpinBox{
                        objectName: "spin2"
                        maximumValue: 2000
                        minimumValue: 0
                    }
                }
            }
        }
        Tab {
            title: "Tab2"
            active: true
            Rectangle {
                radius: 5
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "transparent" }
                    GradientStop { position: 1.0; color: "green" }
                }
                Row{
                    anchors.centerIn: parent
                    TextField{
                        objectName: "text1"

                    }
                    TextField{
                        objectName: "text2"

                    }
                    Loader{
                        id: btnLoader
                        source: "component.qml"
                    }
                }
            }
        }
        Tab {
            title: "Tab3"
            active: true
            Rectangle {
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "transparent" }
                    GradientStop { position: 1.0; color: "red" }
                }
            }
        }
    }
}
