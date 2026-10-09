import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

MenuItem {
    property string inner_text: "..."

    width: parent.width
    background: Rectangle {
        anchors.fill: parent
        implicitHeight: 35
        color: parent.hovered ? current_scheme.button_hover : current_scheme.accent
        radius: 5

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true

            cursorShape: Qt.PointingHandCursor
        }

        Behavior on color {
            ColorAnimation {
                duration: 80
                easing.type: Easing.Linear
            }
        }
    }

    contentItem: Text {
        anchors.topMargin: 10
        anchors.centerIn: parent
        text: inner_text

        font.pointSize: 10
        color: current_scheme.button_text
    }
}