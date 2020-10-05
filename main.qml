import QtQuick 2.15
import QtQuick.Controls 2.15

import "."

ApplicationWindow {
    id: window
    visible: true
    width: 720
    height: 405
    title: qsTr("SuniiApp")

    // ==========================  COMPOSANTS DYNAMIQUES ==========================

    Component {
        id: idCompStoryView
        StoryView {

        }
    }

    StackView {
        id: stackView
        initialItem: idCompStoryView
        anchors.fill: parent
    }
}
