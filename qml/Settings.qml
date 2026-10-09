import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Basic

Item {
    property bool is_closed: true
    property bool show_text: true
    property bool font_size_const: false

    function toggleSettings() {
        is_closed = !is_closed;
        settingsAnimation.from = settings.x;
        settingsAnimation.to = is_closed ? -settings.width : 10;
        settingsAnimation.start();

        burgermenuAnimation.from = burger_menu.x;
        burgermenuAnimation.to = !is_closed ? -burger_menu.width : 10;
        burgermenuAnimation.start();
    }

    implicitHeight: 360
    implicitWidth: 360

    ButtonTemplate {
        anchors.topMargin: 10
        anchors.top: parent.top
        x: 10
        button_width: 40
        button_height: 40

        id: burger_menu
        button_color: current_scheme.button
        button_hover_color: current_scheme.button_hover
        button_text: "\u2699"
        text_color: current_scheme.button_text
        onClicked: { toggleSettings() }

        PropertyAnimation {
            id: burgermenuAnimation
            target: burger_menu
            property: "x"
            duration: 250
            easing.type: Easing.OutExpo
        }
    }

    Rectangle {
        id: settings
        x: -settings.width
        anchors.verticalCenter: parent.verticalCenter
        implicitWidth: parent.width
        implicitHeight: parent.height * 0.9
        color: current_scheme.button
        radius: 5

        ScrollView {
            anchors.fill: parent
            clip: true

            ColumnLayout {
                implicitWidth: parent.width
                spacing: 10


                Label {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 20

                    text: "Settings"
                    font.pixelSize: 18
                    color: current_scheme.text
                }

                RowLayout {
                    spacing: 10
                    Layout.leftMargin: 15
                    Layout.topMargin: 20

                    Label {
                        text: "Show vertex indices"
                        font.pixelSize: 14
                        color: current_scheme.text
                    }

                    Switch {
                        id: show_text_switch
                        checked: show_text
                        onCheckedChanged: {
                            show_text = checked;
                            manager.ToggleShowLabelText();
                        }
                    }
                }

                RowLayout {
                    spacing: 10
                    Layout.leftMargin: 15
                    Layout.topMargin: 5

                    Label {
                        text: "Keep vertex label size constant"
                        font.pixelSize: 14
                        color: current_scheme.text
                    }

                    Switch {
                        id: font_size_const_switch
                        checked: font_size_const
                        onCheckedChanged: {
                            font_size_const = checked;
                            manager.ToggleConstTextFontSize();
                        }
                    }
                }

                RowLayout {
                    spacing: 10
                    Layout.leftMargin: 15
                    Layout.topMargin: 5

                    Label {
                        text: "Vertex label size"
                        font.pixelSize: 14
                        color: current_scheme.text
                    }

                    TextField {
                        id: font_size
                        width: 25
                        height: 35
                        placeholderText: "7-30"
                        Layout.alignment: Qt.AlignRight
                        text: "15"

                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text

                        validator: IntValidator {
                            bottom: 7
                            top: 30
                        }

                    }

                    ButtonTemplate {
                        button_text: "OK"
                        button_width: 40
                        button_color: current_scheme.button
                        button_hover_color: current_scheme.button_hover
                        text_color: current_scheme.text
                        onClicked: {
                            if (font_size.text.trim() !== "") {
                                manager.ChangeNodesFontSize(parseInt(font_size.text))
                            }
                        }
                    }

                    ButtonTemplate {
                        button_text: "\u21BA"
                        Layout.leftMargin: 5
                        button_width: 40
                        button_color: current_scheme.button
                        button_hover_color: current_scheme.button_hover
                        text_color: current_scheme.text
                        onClicked: {
                            font_size.text = "15";
                            manager.ChangeNodesFontSize(parseInt(font_size.text))
                        }
                    }
                }

                RowLayout {
                    spacing: 10
                    Layout.leftMargin: 15
                    Layout.topMargin: 5

                    Label {
                        text: "Edge width"
                        font.pixelSize: 14
                        color: current_scheme.text
                    }

                    TextField {
                        id: edges_width
                        width: 25
                        height: 35
                        placeholderText: "0.1-10.0"
                        Layout.alignment: Qt.AlignRight
                        text: "2"

                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text

                        validator: DoubleValidator {
                            bottom: 0.1
                            top: 10
                            decimals: 2
                        }

                    }

                    ButtonTemplate {
                        button_text: "OK"
                        button_width: 40
                        button_color: current_scheme.button
                        button_hover_color: current_scheme.button_hover
                        text_color: current_scheme.text
                        onClicked: {
                            if (edges_width.text.trim() !== "") {
                                manager.ChangeEdgesWidth(parseFloat(edges_width.text))
                            }
                        }
                    }

                    ButtonTemplate {
                        Layout.leftMargin: 5
                        button_text: "\u21BA"
                        button_width: 40
                        button_color: current_scheme.button
                        button_hover_color: current_scheme.button_hover
                        text_color: current_scheme.text
                        onClicked: {
                            edges_width.text = "2";
                            manager.ChangeEdgesWidth(parseFloat(edges_width.text))
                        }
                    }
                }

                RowLayout {
                    spacing: 10
                    Layout.leftMargin: 15
                    Layout.topMargin: 5

                    Label {
                        text: "Vertex radius"
                        font.pixelSize: 14
                        color: current_scheme.text
                    }

                    TextField {
                        id: nodes_radius
                        width: 25
                        height: 35
                        placeholderText: "0.1-20"
                        Layout.alignment: Qt.AlignRight
                        text: "6"

                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text

                        validator: DoubleValidator {
                            bottom: 0.1
                            top: 20
                            decimals: 2
                        }

                    }

                    ButtonTemplate {
                        button_text: "OK"
                        button_width: 35
                        button_color: current_scheme.button
                        button_hover_color: current_scheme.button_hover
                        text_color: current_scheme.text
                        onClicked: {
                            if (nodes_radius.text.trim() !== "") {
                                manager.ChangeNodesRadius(parseFloat(nodes_radius.text))
                            }
                        }
                    }

                    ButtonTemplate {
                        button_text: "\u21BA"
                        Layout.leftMargin: 5
                        button_width: 35
                        button_color: current_scheme.button
                        button_hover_color: current_scheme.button_hover
                        text_color: current_scheme.text
                        onClicked: {
                            nodes_radius.text = "6";
                            manager.ChangeNodesRadius(parseFloat(nodes_radius.text))
                        }
                    }
                }

            }
        }

        ButtonTemplate {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 5
            button_width: 35
            button_height: 35

            button_color: current_scheme.button
            button_hover_color: current_scheme.button_hover
            button_text: "✕"
            text_color: current_scheme.button_text
            onClicked: { toggleSettings() }
        }

        PropertyAnimation {
            id: settingsAnimation
            target: settings
            property: "x"
            duration: 250
            easing.type: Easing.OutExpo
        }
    }
}
