import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Button {
    id: button
    property int button_width: 110
    property int button_height: 28
    property string button_color: "black"
    property string button_hover_color: button_color
    property string button_text: "text"
    property string text_color: "white"

    property string current_color: button_color

    flat: true
    highlighted: false
    implicitWidth: button_width
    implicitHeight: button_height

    background: Rectangle {
        color: button.hovered ? button_hover_color : button_color
        border.color: "transparent"
        radius: 4

        Behavior on color {
            ColorAnimation {
                duration: 100
                easing.type: Easing.Linear
            }
        }

        MouseArea {
            anchors.centerIn: parent
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

        }
    }
    Text {
        anchors.centerIn: parent
        text: button_text
        color: text_color
        font.pointSize: font_sizes.info
    }
}
