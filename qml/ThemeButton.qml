import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Button {
    highlighted: false
    flat: true
    width: 50
    height: 50

    background: Rectangle {
        Image {
            anchors.centerIn: parent
            width: 25
            height: 25
            source: current_scheme.icon_path
        }

        MouseArea {
            anchors.centerIn: parent
            anchors.fill: parent

            cursorShape: Qt.PointingHandCursor

        }

        color: current_scheme.background

        radius: 100
        border.pixelAligned: true
        border.width: 2
        border.color: parent.hovered ? current_scheme.theme_button : current_scheme.background
    }

    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.rightMargin: 20
    anchors.bottomMargin: 20
    onClicked: { toggleColorScheme(); }
}