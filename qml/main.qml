import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.Basic


Page {
    anchors.fill: parent
    title: "Graph Visualizer"

    property var color_scheme: Definitions.color_scheme
    property var font_sizes: Definitions.font_sizes

    property var current_scheme: color_scheme.dark

    property var current_topology: "Arbitrary"

    property var topology_parameters_json: "{}"

    property bool is_dark_mode: true

    function toggleColorScheme() {
        is_dark_mode = !is_dark_mode;
        current_scheme = is_dark_mode ? color_scheme.dark : color_scheme.light;
        manager.ToggleDarkMode();
    }

    function loadPage(page_url) {
        page_loader.source = ""
        page_loader.sourceComponent = undefined
        page_loader.source = page_url
    }


    Loader {
        anchors.fill: parent
        id: page_loader
        objectName: "page_loader"

        FileDialog {
            id: import_file
            title: "Choose a file"
            nameFilters: ["JSON files (*.json)"]

            onAccepted: {
                if (manager.ValidateFile(import_file.selectedFile)) {
                    loadPage("main_page.qml");
                    manager.SetInputDataFromJson("file", import_file.selectedFile);
                    manager.SetMainPageProperties();
                }
            }
        }

        FileDialog {
            id: import_matrix
            title: "Choose a file"
            nameFilters: ["Adjacency Matrix files (*.txt)"]

            onAccepted: {
                loadPage("main_page.qml");
                manager.SetInputDataFromAdjacencyMatrixFile("topology_parameters", import_matrix.selectedFile);
                manager.SetMainPageProperties();
            }
        }

        Rectangle {
            anchors.fill: parent

            color: current_scheme.background

            Rectangle {
                id: container
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                height: parent.height
                color: parent.color

                clip: true

                ColumnLayout {
                    anchors.centerIn: parent
                    width: parent.width
                    clip: true
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Text {
                        text: "Graph Visualizer"
                        Layout.alignment: Qt.AlignHCenter
                        font.pointSize: font_sizes.header
                        font.bold: true
                        color: current_scheme.header_text
                    }

                    Text {
                        text: "Version 0.6.0"
                        Layout.alignment: Qt.AlignHCenter
                        font.pointSize: font_sizes.info
                        color: current_scheme.grayed_text
                    }

                    RowLayout {
                        Layout.topMargin: 40
                        Layout.alignment: Qt.AlignHCenter
                        width: parent.width
                        clip: true

                        Column {
                            Layout.rightMargin: 100
                            Text {
                                text: "Import configuration file"
                                font.pixelSize: font_sizes.main
                                color: current_scheme.text
                            }
                            Text {
                                width: 300
                                wrapMode: Text.WordWrap
                                text: "Import a JSON configuration file."
                                font.pixelSize: font_sizes.info
                                color: current_scheme.grayed_text
                            }
                        }
                        ButtonTemplate {
                            button_width: 86
                            button_height: 28
                            button_text: "Import"
                            button_color: current_scheme.colored_button

                            onClicked: import_file.open()
                        }

                    }

                    Rectangle {
                        Layout.topMargin: 10
                        Layout.alignment: Qt.AlignHCenter
                        color: current_scheme.grayed_text

                        implicitWidth: 475
                        implicitHeight: 1
                    }

                    RowLayout {
                        Layout.topMargin: 5
                        Layout.alignment: Qt.AlignHCenter
                        width: parent.width
                        clip: true

                        Column {
                            Layout.rightMargin: 100
                            Text {
                                text: "Import adjacency matrix"
                                font.pixelSize: font_sizes.main
                                color: current_scheme.text
                            }
                            Text {
                                width: 300
                                wrapMode: Text.WordWrap
                                text: "Import a .txt adjacency matrix file."
                                font.pixelSize: font_sizes.info
                                color: current_scheme.grayed_text
                            }
                        }

                        ButtonTemplate {
                            button_width: 86
                            button_height: 28
                            button_text: "Import"
                            button_color: current_scheme.colored_button

                            onClicked: {
                                import_matrix.open()
                            }
                        }

                    }

                    Rectangle {
                        Layout.topMargin: 10
                        Layout.alignment: Qt.AlignHCenter
                        color: current_scheme.grayed_text

                        implicitWidth: 475
                        implicitHeight: 1
                    }

                    RowLayout {
                        Layout.topMargin: 5
                        Layout.alignment: Qt.AlignHCenter
                        width: parent.width
                        clip: true

                        Column {
                            Layout.rightMargin: 100
                            Text {
                                text: "Create from topology"
                                font.pixelSize: font_sizes.main
                                color: current_scheme.text
                            }
                            Text {
                                width: 300
                                wrapMode: Text.WordWrap
                                text: "Create a graph from a topology and its parameters."
                                font.pixelSize: font_sizes.info
                                color: current_scheme.grayed_text
                            }
                        }

                        ButtonTemplate {
                            button_width: 86
                            button_height: 28
                            button_text: "Create"
                            button_color: current_scheme.colored_button

                            onClicked: {
                                loadPage("topology_parameters.qml");
                                manager.SetInputAreaPageProperties();
                            }
                        }

                    }

                    Rectangle {
                        Layout.topMargin: 10
                        Layout.alignment: Qt.AlignHCenter
                        color: current_scheme.grayed_text

                        implicitWidth: 475
                        implicitHeight: 1
                    }
                }
            }

            ThemeButton { id: theme_button }
        }
    }

}
