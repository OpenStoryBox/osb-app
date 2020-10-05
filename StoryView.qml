import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import "."

Rectangle {
    id: idStoryView
    anchors.fill: parent
    color: "#45ADAF"
    Screen.orientationUpdateMask:  Qt.LandscapeOrientation | Qt.PortraitOrientation

    Screen.onPrimaryOrientationChanged: {
        console.log("orinetation changed, width: " + width )
    }

    //property bool isPortrait: Screen.primaryOrientation === Qt.PortraitOrientation || Screen.primaryOrientation === Qt.InvertedPortraitOrientation

    property bool isPortrait: idStoryView.width < idStoryView.height
    property int sectionWidth: idStoryView.isPortrait ? idStoryView.width : idStoryView.width / 3

    property int sectionWidth1: idStoryView.isPortrait ? idStoryView.width : idStoryView.width * 2 / 5
    property int sectionWidth2: idStoryView.isPortrait ? idStoryView.width : idStoryView.width * 2 / 5
    property int sectionWidth3: idStoryView.isPortrait ? idStoryView.width : idStoryView.width / 5

    property int sectionHeight: idStoryView.isPortrait ? idStoryView.height / 3 : idStoryView.height

    function setImage(img) {
        imageDisplay.source = img;
    }

    Component.onCompleted: {
        sunii.openFile();
    }

    Connections
    {
        target: lcd
        function onSigShowImage() {
            setImage(lcd.getImage());
            console.log("hey")
        }
    }

    Rectangle {
        x: 0
        y: 0
        width: sectionWidth1
        height: sectionHeight
        color: "transparent"

        Dial {
            id: idFrontDial
            width: idStoryView.isPortrait ? parent.height * 2 / 3 : parent.width * 2 / 3
            height: width
            anchors.centerIn: parent
            background: Rectangle {
                width: idFrontDial.width
                height: idFrontDial.height
                color: "#f0bc61"
                radius: width / 2
           //     border.color: idFrontDial.pressed ? "#E0B05C" : "#f0bc61"
            //    opacity: idFrontDial.enabled ? 1 : 0.3
            }

            handle: Rectangle {
                id: handleItem
                x: idFrontDial.background.x + idFrontDial.background.width / 2 - width / 2
                y: idFrontDial.background.y + idFrontDial.background.height / 2 - height / 2
                width: 16
                height: 16
                color: idFrontDial.pressed ? "#F3C565" : "#F3C565"
                radius: 8
                antialiasing: true
                opacity: idFrontDial.enabled ? 1 : 0.3
                transform: [
                    Translate {
                        y: -Math.min(idFrontDial.background.width, idFrontDial.background.height) * 0.4 + handleItem.height / 2
                    },
                    Rotation {
                        angle: idFrontDial.angle
                        origin.x: handleItem.width / 2
                        origin.y: handleItem.height / 2
                    }
                ]
            }
        }
    }

    Rectangle {
        x: idStoryView.isPortrait ? 0 : idStoryView.sectionWidth2
        y: idStoryView.isPortrait ? idStoryView.sectionHeight : 0
        width: sectionWidth2
        height: sectionHeight
        color: "transparent"

        Rectangle {
            anchors.centerIn: parent
            width: idStoryView.isPortrait ?  height * 4 / 3 : idStoryView.sectionWidth2  * 4 / 5
            height: idStoryView.isPortrait ? idStoryView.sectionHeight  * 4 / 5 :  width * 3 / 4

            color: "#38A0A2"

            Image {
                z: 1000
                id: imageDisplay
                fillMode: Image.PreserveAspectFit
                anchors.fill: parent
            }
        }
    }

    Rectangle {
        id: idFrontButtons
        x: idStoryView.isPortrait ? 0 : idStoryView.sectionWidth1 + idStoryView.sectionWidth2
        y: idStoryView.isPortrait ? idStoryView.sectionHeight * 2 : 0
        width: sectionWidth3
        height: sectionHeight
        color: "transparent"

        FrontButton {
            width: idFrontButtons.width  / 3
            x: (idFrontButtons.width - width) / 2
            y: (idFrontButtons.height / 4) - (width / 2)
            iconName: "icons/home.png"
        }

        FrontButton {
            width: idFrontButtons.width  / 3
            x: (idFrontButtons.width - width) / 2
            y: (idFrontButtons.height * 2 / 4) - (width / 2)
            iconName: "icons/pause.png"
        }

        FrontButton {
            width: idFrontButtons.width  / 2
            x: (idFrontButtons.width - width) / 2
            y: (idFrontButtons.height * 3 / 4) - (width / 2)
            iconName: "icons/ok.png"
        }

    }


}
