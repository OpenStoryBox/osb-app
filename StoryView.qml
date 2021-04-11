import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtQuick.Dialogs 1.3

import "."

Rectangle {
    id: idStoryView
    anchors.fill: parent
    color: '#5586a4' // vert lunni: "#45ADAF"
    Screen.orientationUpdateMask:  Qt.LandscapeOrientation | Qt.PortraitOrientation

    Screen.onPrimaryOrientationChanged: {
        console.log("orinetation changed, width: " + width )
    }

    //property bool isPortrait: Screen.primaryOrientation === Qt.PortraitOrientation || Screen.primaryOrientation === Qt.InvertedPortraitOrientation

    property bool isPortrait: idStoryView.width < idStoryView.height
    property int sectionWidth: idStoryView.isPortrait ? idStoryView.width : idStoryView.width / 2

    property int sectionWidth1: idStoryView.isPortrait ? idStoryView.width : idStoryView.width * 2 / 3
    property int sectionWidth2: idStoryView.isPortrait ? idStoryView.width : idStoryView.width * 1 / 3

    property int sectionHeight: idStoryView.isPortrait ? idStoryView.height / 2 : idStoryView.height

    property string packPath: ''

    function setImage(img) {
        imageDisplay.source = img;
    }

    function saveCurrentData() {
    }

    Component.onCompleted: {
        storyTeller.openFile();
    }

    Connections
    {
        target: storyTeller
        function onSigShowImage() {
            setImage(storyTeller.getImage());
        }

        function onSigClearScreen() {
            setImage("default_image.bmp");
        }
    }

    Rectangle {
        x: 0
        y: idStoryView.isPortrait ? idStoryView.sectionHeight : 0
        width: sectionWidth1
        height: sectionHeight
        color: "transparent"

        Rectangle {
            anchors.centerIn: parent
            width: idStoryView.isPortrait ?  height * 4 / 3 : idStoryView.sectionWidth1  * 4 / 5
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
        x: idStoryView.isPortrait ? 0 : idStoryView.sectionWidth1
        y: idStoryView.isPortrait ? idStoryView.sectionHeight * 2 : 0
        width: sectionWidth2
        height: sectionHeight
        color: "transparent"

        Button {
            text: "\u2630";
            x: idStoryView.isPortrait ? 0 : idStoryView.sectionWidth1 + idStoryView.sectionWidth2 - 45
            width: 40
            onClicked:  folderDialog.open()
        }

        FileDialog {
            id: folderDialog
            selectFolder: true
            onAccepted: {
//                console.log("You chose: " + folderDialog.fileUrl)
                idStoryView.packPath = folderDialog.fileUrl;
                idStoryView.saveCurrentData();
            }
        }

        FrontButton {
            width: idFrontButtons.width  / 4
            x: (idFrontButtons.width - width) / 2
            y: (idFrontButtons.height / 4) - (width / 2)
            iconName: "icons/home.png"
        }

        FrontButton {
            width: idFrontButtons.width  / 4
            x: (idFrontButtons.width - width) / 2
            y: (idFrontButtons.height * 2 / 4) - (width / 2)
            iconName: "icons/pause.png"
        }

        FrontButton {
            width: idFrontButtons.width  / 3
            x: (idFrontButtons.width - width) / 2
            y: (idFrontButtons.height * 3 / 4) - (width / 2)
            iconName: "icons/ok.png"

            MouseArea {
                anchors.fill: parent
                onClicked: storyTeller.okButton()
            }
        }
    }


}
