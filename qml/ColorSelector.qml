import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Layouts

Rectangle {
    property int selector_width: 200
    property int selector_height: 40
    property string selector_text: "Choose a color"
    property string selected_color: "#66d75d"

    width: selector_width
    height: selector_height
    color: current_scheme.accent
    radius: 5

    RowLayout {
        Layout.alignment: Qt.AlignHCenter
        anchors.fill: parent
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        spacing: 10

        Text {
            text: selector_text
            verticalAlignment: Text.AlignVCenter
            Layout.leftMargin: 10
            color: current_scheme.text
            font.pixelSize: 14
        }

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.rightMargin: 10
            width: 30
            height: 30
            color: selected_color
            radius: 5
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            parent.selected_color = manager.GetColor(selected_color);
        }
    }
}
