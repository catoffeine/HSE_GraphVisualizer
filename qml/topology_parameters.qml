import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Basic

Page {
    function getTopologyComponent(topology) {
        switch (topology) {
            case "Circulant": return circulant_form
            case "Multiplicative Circulant": return adv_circulant_form
            case "Paley" : return adv_circulant_form
            case "HyperCirculant" : return hypercirculant_form
            case "Mesh": return mesh_form
            case "Spidergon": return spidergon_form
            case "Torus": return mesh_form
            case "Ring": return ring_form
            case "FatTree": return ft_form
            case "XTree": return xt_form
            case "TMesh": return mesh_form
            case "DMesh": return mesh_form
            case "DMeshDoubleLinked": return mesh_form
            case "ZMesh": return mesh_form
            case "DCM": return mesh_form
            case "MCC": return mesh_form
            case "XNetwork": return mesh_form
            case "CBPMesh": return mesh_form
            case "StarMesh": return mesh_form
            case "L2Star": return mesh_form
            default: return circulant_form
        }

    }



    Loader {
        anchors.fill: parent
        id: page_loader

        Rectangle {
            anchors.fill: parent
            color: current_scheme.background

            Column {
                anchors.centerIn: parent
                spacing: 20

                Text {
                    text: "Choose a topology"
                    font.pixelSize: 18
                    color: current_scheme.text
                }

                ComboBox {
                    anchors.horizontalCenter: parent.horizontalCenter
                    id: topology_select
                    width: 200
                    height: 50

                    background: Rectangle {
                        color: current_scheme.accent
                        radius: 5
                    }

                    contentItem: Text {
                        text: topology_select.displayText
                        color: current_scheme.text
                        font.pixelSize: 15
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }

                    indicator: Text {
                        text: "▼"
                        font.pixelSize: 15
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        rightPadding: 20
                        color: current_scheme.grayed_text
                    }

                    delegate: ItemDelegate {
                        width: topology_select.width
                        height: 40

                        background: Rectangle {
                            color: highlighted ? current_scheme.button : current_scheme.button_hover
                            border.color: "transparent"
                            radius: 5
                        }

                        contentItem: Text {
                            text: modelData
                            color: current_scheme.text
                            font.pixelSize: 14
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            leftPadding: 10
                        }

                        highlighted: topology_select.highlightedIndex === index
                    }

                    popup: Popup {
                        y: topology_select.height - 10
                        width: topology_select.width
                        implicitHeight: contentItem.implicitHeight

                        background: null

                        contentItem: ListView {
                            clip: true
                            implicitHeight: contentHeight
                            implicitWidth: topology_select.width
                            model: topology_select.popup.visible ? topology_select.delegateModel : null
                            currentIndex: topology_select.highlightedIndex
                        }
                    }

                    function topologyUpdate() {
                        let topology = topology_select.currentText
                        current_topology = topology
                        topology_parameters_loader.sourceComponent = getTopologyComponent(topology);


                    }

                    Component.onCompleted: {
                        topology_select.model = manager.GetTopologiesList()

                        topologyUpdate()

                    }

                    onActivated: (index) => {
                        topologyUpdate()
                    }
                }

                Loader {
                    id: topology_parameters_loader
                }

                ButtonTemplate {
                    anchors.horizontalCenter: parent.horizontalCenter
                    button_width: 86
                    button_height: 28
                    button_text: "Create"
                    button_color: current_scheme.colored_button

                    onClicked: {
                        if (topology_parameters_loader.item.checkParameters()) {
                            let parameters = topology_parameters_loader.item.getParameters()
                            parameters["topology"] = current_topology
                            topology_parameters_json = JSON.stringify(parameters)
                            loadPage("main_page.qml");
                            manager.SetInputDataFromTopology("topology_parameters", topology_parameters_json);
                            manager.SetMainPageProperties();
                        }
                    }
                }
            }

            Component {
                id: circulant_form

                Column {
                    spacing: 10

                    TextField {
                        id: nodes_field
                        width: 200
                        placeholderText: "number of vertices"

                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text
                    }

                    TextField {
                        id: generators_field
                        width: 200
                        placeholderText: "generators, e.g. 1,2"

                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text
                    }

                    Text {
                        id: wrong_data_info
                        opacity: 0
                        width: 200
                        horizontalAlignment: Text.AlignHCenter
                        color: current_scheme.wrong_text
                        text: "Invalid input"
                        font.pixelSize: 14
                    }

                    function checkParameters() {
                        let generators_field_check = true
                        let gens = generators_field.text.split(",").
                            map(x => x.trim())

                        gens.forEach(gen => {
                            if (isNaN(gen)) generators_field_check = false;
                        })

                        if (
                            nodes_field.text.trim() === "" ||
                            generators_field.text.trim() === "" ||
                            isNaN(nodes_field.text) ||
                            !generators_field_check
                        ) {
                            wrong_data_info.opacity = 1
                            return false
                        }

                        return true
                    }


                    function getParameters() {
                        return {
                            nodes_number: nodes_field.text,
                            generators: generators_field.text.split(",").map(x => x.trim())
                        };
                    }
                }
            }

            Component {
                id: hypercirculant_form

                Column {
                    spacing: 10

                    TextField {
                        id: nodes_field
                        width: 200
                        placeholderText: "number of vertices"

                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text
                    }

                    TextField {
                        id: generators_field
                        width: 200
                        placeholderText: "generators, e.g. 1,2;1,3"

                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text
                    }

                    Text {
                        id: wrong_data_info
                        opacity: 0
                        width: 200
                        horizontalAlignment: Text.AlignHCenter
                        color: current_scheme.wrong_text
                        text: "Invalid input"
                        font.pixelSize: 14
                    }

                    function checkParameters() {
                        if (nodes_field.text.trim() === "" || isNaN(nodes_field.text)) {
                            wrong_data_info.opacity = 1;
                            return false;
                        }

                        let generatorsValid = true;
                        const generatorsText = generators_field.text.trim();

                        if (generatorsText === "") {
                            wrong_data_info.opacity = 1;
                            return false;
                        }

                        const outerParts = generatorsText.split(';');

                        for (let i = 0; i < outerParts.length; i++) {
                            const innerParts = outerParts[i].split(',');

                            if (innerParts.length === 0) {
                                generatorsValid = false;
                                break;
                            }

                            for (let j = 0; j < innerParts.length; j++) {
                                const numStr = innerParts[j].trim();

                                if (numStr === "" || isNaN(numStr)) {
                                    generatorsValid = false;
                                    break;
                                }
                            }

                            if (!generatorsValid) break;
                        }

                        if (!generatorsValid) {
                            wrong_data_info.opacity = 1;
                            return false;
                        }

                        return true;
                    }


                    function getParameters() {
                        let generatorsArrays = generators_field.text.split(";").map(function(outerPart) {
                            return outerPart.split(",").map(function(x) {
                                return x.trim();
                            }).filter(function(x) {
                                return x.length > 0;
                            });
                        }).filter(function(innerArray) {
                            return innerArray.length > 0;
                        });

                        return {
                            nodes_number: nodes_field.text,
                            generators: generatorsArrays
                        };
                    }
                }
            }

            Component {
                id: adv_circulant_form

                Column {
                    spacing: 10

                    TextField {
                        id: nodes_field
                        width: 200
                        placeholderText: "base"

                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text
                    }

                    TextField {
                        id: degree_field
                        width: 200
                        placeholderText: "degree"

                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text
                    }

                    Text {
                        id: wrong_data_info
                        opacity: 0
                        width: 200
                        horizontalAlignment: Text.AlignHCenter
                        color: current_scheme.wrong_text
                        text: "Invalid input"
                        font.pixelSize: 14
                    }

                    function checkParameters() {
                        if (degree_field.text.trim().length === 0 || nodes_field.text.trim().length === 0 ||
                            isNaN(degree_field.text) || isNaN(nodes_field.text))
                        {
                            wrong_data_info.opacity = 1
                            return false
                        }

                        return true
                    }


                    function getParameters() {
                        return {
                            nodes: nodes_field.text,
                            degree: degree_field.text
                        };
                    }
                }
            }

            Component {
                id: mesh_form

                Column {
                    spacing: 10

                    TextField {
                        id: rows_field
                        width: 200
                        placeholderText: "number of rows"
                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text
                    }

                    TextField {
                        id: cols_field
                        width: 200
                        placeholderText: "number of columns"
                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text
                    }

                    Text {
                        id: wrong_data_info
                        opacity: 0
                        width: 200
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: current_scheme.wrong_text
                        text: "Invalid input"
                        font.pixelSize: 12
                    }

                    function checkParameters() {
                        if (rows_field.text.trim().length === 0 || cols_field.text.trim().length === 0 ||
                            isNaN(rows_field.text) || isNaN(cols_field.text)) {
                            wrong_data_info.opacity = 1
                            return false
                        }

                        return true
                    }

                    function getParameters() {
                        return {
                            rows: rows_field.text,
                            columns: cols_field.text
                        };
                    }
                }
            }

            Component {
                id: ft_form

                Column {
                    spacing: 10

                    TextField {
                        id: concentration
                        width: 200
                        placeholderText: "vertex concentration"
                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text
                    }

                    TextField {
                        id: levels
                        width: 200
                        placeholderText: "number of levels"
                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text
                    }

                    Text {
                        id: wrong_data_info
                        opacity: 0
                        width: 200
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: current_scheme.wrong_text
                        text: "Invalid input"
                        font.pixelSize: 12
                    }

                    function checkParameters() {
                        if (concentration.text.trim().length === 0 || levels.text.trim().length === 0 ||
                            isNaN(concentration.text) || isNaN(levels.text)) {
                            wrong_data_info.opacity = 1
                            return false
                        }

                        return true
                    }

                    function getParameters() {
                        return {
                            concentration: concentration.text,
                            levels: levels.text
                        };
                    }
                }
            }

            Component {
                id: xt_form

                Column {
                    spacing: 10

                    TextField {
                        id: levels
                        width: 200
                        placeholderText: "number of levels"
                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        placeholderTextColor: current_scheme.grayed_text
                        color: current_scheme.text
                    }

                    Text {
                        id: wrong_data_info
                        opacity: 0
                        width: 200
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: current_scheme.wrong_text
                        text: "Invalid input"
                        font.pixelSize: 12
                    }

                    function checkParameters() {
                        if (levels.text.trim().length === 0 || isNaN(levels.text)) {
                            wrong_data_info.opacity = 1
                            return false
                        }

                        return true
                    }

                    function getParameters() {
                        return {
                            levels: levels.text
                        };
                    }
                }
            }

            Component {
                id: spidergon_form

                Column {
                    spacing: 10

                    TextField {
                        id: nodes_number
                        width: 200
                        placeholderText: "number of vertices"
                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        color: current_scheme.text
                        placeholderTextColor: current_scheme.grayed_text
                    }

                    Text {
                        id: wrong_data_info
                        opacity: 0
                        width: 200
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: current_scheme.wrong_text
                        text: "Invalid input"
                        font.pixelSize: 12
                    }

                    function checkParameters() {
                        if (nodes_number.text.trim().length === 0 || isNaN(nodes_number.text) ||
                            parseInt(nodes_number.text) % 2 !== 0) {

                            wrong_data_info.opacity = 1
                            return false
                        }

                        return true
                    }

                    function getParameters() {
                        return {
                            nodes_number: nodes_number.text.trim()
                        };
                    }
                }
            }

            Component {
                id: ring_form

                Column {
                    spacing: 10

                    TextField {
                        id: nodes_number
                        width: 200
                        placeholderText: "number of vertices"
                        background: Rectangle {
                            color: current_scheme.textarea_color
                        }
                        color: current_scheme.text
                        placeholderTextColor: current_scheme.grayed_text
                    }

                    Text {
                        id: wrong_data_info
                        opacity: 0
                        width: 200
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: current_scheme.wrong_text
                        text: "Invalid input"
                        font.pixelSize: 12
                    }

                    function checkParameters() {
                        if (nodes_number.text.trim().length === 0 ||
                            isNaN(nodes_number.text)) {
                            wrong_data_info.opacity = 1
                            return false
                        }

                        return true
                    }
                    function getParameters() {
                        return {
                            nodes_number: nodes_number.text.trim()
                        };
                    }
                }
            }

            ThemeButton { id: theme_button }
        }
    }
}
