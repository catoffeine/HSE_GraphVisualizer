import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Fusion

Page {

    property bool is_outline: false
    property var graphResults
    property bool isDarkTheme: true
    property color shortest_path_selected_color: "lightblue"
    property var group_names: []

    function change_outline_mode() {
        if (!canvas) {
            return
        }

        if (is_outline) {
            outline_mode.enabled = true;
            move_mode.enabled = false;
        } else {
            outline_mode.enabled = false;
            move_mode.enabled = true;
        }
        is_outline = !is_outline;
        manager.ToggleOutlineMode();
    }

    function reset_positions() {
        manager.ResetPositions();
    }


    function handleEdgesSelected(number) {
        if (!canvas) {
            return
        }

        if (number === 1) {
            pathDialog.visible = false
            vertex_container.visible = false
            manager.DeselectVertices();
            single_edge_container.visible = true
            group_names = manager.GetGroupNames()
            group_names.unshift("Apply to current group", "Create a new group")
            single_edge_combobox.model = group_names

            let edge_vertices = manager.GetSelectedEdgeVertices()
            let edge_group_name = manager.GetSelectedEdgeGroupName()
            if (edge_group_name === "") edge_group_name = "no group"

            single_edge_info_vertices.text = `From vertex ${edge_vertices[0]} to vertex ${edge_vertices[1]}`
            single_edge_info_groupname.text = `Edge group: ${edge_group_name}`

        } else if (number === 0) {
            single_edge_container.visible = false;
        } else {
            pathDialog.visible = false
            single_edge_container.visible = false;
        }
    }

    function handleVerticesSelected(selection_status) {
        if (!canvas) {
            return
        }

        if (selection_status) {
            pathDialog.visible = false
            single_edge_container.visible = false
            manager.DeselectEdges()

            vertex_container.visible = true;
        } else {
            vertex_container.visible = false;
        }
    }

    function handleVerticesMoved(move_status) {
        if (move_status) {
            reset_positions_button.enabled = true;
        } else {
            reset_positions_button.enabled = false;
        }
    }

    Loader {
        anchors.fill: parent
        id: page_loader

        Rectangle {
            anchors.fill: parent
            color: current_scheme.background
            id: main_page_rect

            MainCanvas {
                id: canvas
                Rectangle {
                    id: topologyInfoRectangle
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.topMargin: 10
                    implicitWidth: 20
                    implicitHeight: 30
                    radius: 25
                    color: current_scheme.accent
                    clip: true

                    property string topology_color: current_scheme.grayed_text

                    Row {
                        anchors.centerIn: parent
                        visible: topologyInfoRectangle.width > 30

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Detected topology: ";
                            font.pointSize: 12
                            color: current_scheme.grayed_text
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: current_topology
                            font.pointSize: 12
                            color: topologyInfoRectangle.topology_color
                        }
                    }

                    SequentialAnimation on width {
                        id: topologyInfoAnimation
                        loops: 1
                        running: true

                        PauseAnimation {
                            duration: 100
                        }

                        PropertyAnimation {
                            target: topologyInfoRectangle
                            property: "width"
                            from: 20
                            to: 400
                            duration: 700
                            easing.type: Easing.OutExpo
                        }

                        ColorAnimation {
                            target: topologyInfoRectangle
                            property: "topology_color"
                            from: current_scheme.grayed_text
                            to: "#84b841"
                            duration: 350
                            easing.type: Easing.InOutCubic
                        }
                        // "#bb4c4c"

                        ScriptAction {
                            script: {
                                topologyInfoRectangle.topology_color = "#84b841"
                            }
                        }
                    }

                    Component.onCompleted: {
                        topologyInfoAnimation.start();
                        manager.SetCanvasWidgetItem(canvas);
                        manager.edgesSelected.connect(handleEdgesSelected);
                        manager.verticesSelected.connect(handleVerticesSelected);
                        manager.verticesMoved.connect(handleVerticesMoved);
                    }
                }

                Settings {
                }
            }


            ButtonTemplate {
                anchors.top: parent.top
                anchors.right: parent.right
                button_width: 100
                anchors.margins: 5

                id: menu_button
                button_color: current_scheme.button
                button_hover_color: current_scheme.button_hover
                button_text: "Menu"
                text_color: current_scheme.button_text
                onClicked: menu.open()

                Menu {
                    background: null
                    id: menu
                    y: menu_button.height + 5
                    width: 200

                    MainMenuItem {
                        inner_text: "Open"

                        onTriggered: {
                            loadPage("main.qml");
                            manager.SetMeetPageProperties()
                        }
                    }

                    MainMenuItem {
                        inner_text: "Save JPEG"

                        onTriggered: {
                            manager.SaveCanvas()
                        }
                    }

                    MainMenuItem {
                        inner_text: "Save SVG"

                        onTriggered: {
                            manager.SaveCanvas(true)
                        }
                    }

                    MainMenuItem {
                        inner_text: "Save JSON"

                        onTriggered: {
                            manager.toJson()
                        }
                    }

                    MainMenuItem {
                        inner_text: "Save adjacency matrix"

                        onTriggered: {
                            manager.toMatrix()
                        }
                    }

                    MainMenuItem {
                        inner_text: "Save BookSim file"

                        onTriggered: {
                            manager.toBookSim()
                        }
                    }

                    MainMenuItem {
                    }
                }
            }

            Rectangle {
                id: vertex_container
                visible: false
                width: 270
                height: 130
                color: current_scheme.background
                radius: 10
                border.color: "#353535"
                border.width: 1
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 40

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 10

                    ColorSelector {
                        id: vertex_color_selector
                        Layout.alignment: Qt.AlignHCenter
                        selector_text: "Choose vertex color"
                        selector_width: 240
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: 5
                        spacing: 20

                        ButtonTemplate {
                            button_text: "Apply"
                            button_width: 100
                            button_height: 35
                            text_color: current_scheme.text
                            button_color: current_scheme.button
                            button_hover_color: current_scheme.button_hover
                            onClicked: {
                                manager.ChangeVertexColor(vertex_color_selector.selected_color);
                                manager.DeselectVertices();
                                vertex_container.visible = false;
                            }
                        }

                        ButtonTemplate {
                            button_text: "Cancel"
                            button_height: 35
                            button_width: 100
                            button_color: current_scheme.button
                            text_color: current_scheme.text
                            button_hover_color: current_scheme.button_hover
                            onClicked: {
                                vertex_container.visible = false
                                manager.DeselectVertices();
                            }
                        }
                    }
                }
                z: 100
            }

            Rectangle {
                id: single_edge_container
                visible: false
                width: 320
                height: 465
                color: current_scheme.background
                radius: 10
                border.color: "#353535"
                border.width: 1
                anchors.top: menu_button.bottom
                anchors.right: parent.right
                anchors.rightMargin: 40
                z: 100

                SequentialAnimation on height {
                    id: single_edge_container_animation_params_out
                    loops: 1
                    running: false
                    PropertyAnimation {
                        target: single_edge_container
                        property: "height"
                        from: 465
                        to: 575
                        duration: 600
                        easing.type: Easing.OutExpo
                    }
                }

                SequentialAnimation on height {
                    id: single_edge_container_animation_params_into
                    loops: 1
                    running: false
                    PropertyAnimation {
                        target: single_edge_container
                        property: "height"
                        from: 575
                        to: 465
                        duration: 600
                        easing.type: Easing.OutExpo
                    }
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 10

                    Text {
                        Layout.alignment: Qt.AlignLeft
                        text: "<b>Edge information</b>"
                        font.pixelSize: 16
                        color: current_scheme.text
                    }

                    Text {
                        Layout.alignment: Qt.AlignLeft
                        id: single_edge_info_vertices
                        font.pixelSize: 14
                        color: current_scheme.text
                        text: "From vertex 1 to vertex 2"
                    }

                    Text {
                        Layout.alignment: Qt.AlignLeft
                        id: single_edge_info_groupname
                        font.pixelSize: 14
                        color: current_scheme.text
                        text: "Edge group: group 1"
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignVCenter
                        Text {
                            Layout.alignment: Qt.AlignLeft
                            text: "Apply to entire group"
                            font.pixelSize: 14
                            color: current_scheme.text
                        }
                        CheckBox {
                            id: single_edge_useasgroup
                            Layout.alignment: Qt.AlignVCenter
                            Layout.topMargin: 14
                            indicator: Rectangle {
                                width: 18
                                height: 18
                                radius: 25
                                color: single_edge_useasgroup.checked ? "#8dca6a" : current_scheme.text
                                Layout.alignment: Qt.AlignVCenter
                            }
                            spacing: 6
                        }
                    }

                    Text {
                        Layout.alignment: Qt.AlignLeft
                        font.pixelSize: 14
                        color: current_scheme.text
                        Layout.preferredWidth: parent.width
                        wrapMode: Text.WordWrap
                        text: "<b>Add to a group or create a new one:</b>"
                    }

                    ComboBox {
                        Layout.alignment: Qt.AlignHCenter
                        id: single_edge_combobox
                        implicitWidth: 230
                        implicitHeight: 40
                        Layout.topMargin: 5

                        background: Rectangle {
                            color: current_scheme.accent
                            radius: 5
                        }

                        contentItem: Text {
                            text: single_edge_combobox.displayText
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
                            width: single_edge_combobox.width
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
                            highlighted: single_edge_combobox.highlightedIndex === index
                        }

                        popup: Popup {
                            y: single_edge_combobox.height - 10
                            width: single_edge_combobox.width
                            implicitHeight: contentItem.implicitHeight
                            background: null
                            contentItem: ListView {
                                clip: true
                                implicitHeight: contentHeight
                                implicitWidth: single_edge_combobox.width
                                model: single_edge_combobox.popup.visible ? single_edge_combobox.delegateModel : null
                                currentIndex: single_edge_combobox.highlightedIndex
                            }
                        }

                        Component.onCompleted: {
                            let group_names = manager.GetGroupNames()
                            group_names.unshift("Apply to current group", "Create a new group")
                            single_edge_combobox.model = group_names
                        }

                        onActivated: (index) => {
                            if (index == 1) {
                                new_groupname_loader.sourceComponent = groupname_component
                                new_groupname_loader.item.makeOkay();
                            }
                            else {
                                new_groupname_loader.sourceComponent = empty_component
                            }
                        }
                    }

                    Loader {
                        id: new_groupname_loader
                        onLoaded: {
                            if (single_edge_combobox.currentIndex != 1) {
                                sourceComponent = undefined
                            }
                        }
                    }

                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        id: modeSwitch
                        visible: single_edge_combobox.currentIndex <= 1
                        width: 220
                        implicitHeight: 40
                        color: current_scheme.button
                        radius: 5

                        RowLayout {
                            Layout.rightMargin: 5
                            anchors.fill: parent
                            spacing: 0

                            Rectangle {
                                id: colorModeButton
                                Layout.preferredWidth: 100
                                Layout.alignment: Qt.AlignLeft
                                Layout.fillHeight: true
                                radius: 4
                                color: colorRadio.checked ? current_scheme.button_hover : "transparent"
                                // border.color: current_scheme.grayed_text

                                Text {
                                    anchors.centerIn: parent
                                    Layout.rightMargin: 10
                                    text: "Color"
                                    font.pixelSize: 14
                                    color: current_scheme.text
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: colorRadio.checked = true
                                }
                            }

                            Rectangle {
                                id: paramsModeButton
                                Layout.fillHeight: true
                                Layout.preferredWidth: 100
                                Layout.leftMargin: 10
                                Layout.alignment: Qt.AlignRight
                                radius: 4
                                color: paramsRadio.checked ? current_scheme.button_hover : "transparent"
                                // border.color: current_scheme.grayed_text

                                Text {
                                    anchors.centerIn: parent
                                    text: "Parameters"
                                    font.pixelSize: 14
                                    color: current_scheme.text
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: paramsRadio.checked = true
                                }
                            }
                        }

                        ButtonGroup { id: colorParamsGroup }
                        RadioButton {
                            id: colorRadio
                            checked: true
                            ButtonGroup.group: colorParamsGroup
                            visible: false
                        }
                        RadioButton {
                            id: paramsRadio
                            ButtonGroup.group: colorParamsGroup
                            visible: false
                        }
                    }

                    ColorSelector {
                        id: single_edge_color_selector
                        Layout.alignment: Qt.AlignHCenter
                        selector_text: "Choose edge color"
                        selector_width: 230
                        visible: colorRadio.checked && single_edge_combobox.currentIndex <= 1
                    }

                    ColumnLayout {
                        Layout.alignment: Qt.AlignHCenter
                        id: paramsColumn
                        visible: paramsRadio.checked && single_edge_combobox.currentIndex <= 1
                        spacing: 5
                        // Layout.topMargin: 10


                        ComboBox {
                            id: lineTypeCombo
                            Layout.alignment: Qt.AlignHCenter
                            property int previous_index: 0

                            onCurrentIndexChanged: {
                                if (lineTypeCombo.currentIndex == 2) {
                                    // single_edge_container_animation_params_out.start()
                                    single_edge_container.height = 575
                                } else if (lineTypeCombo.currentIndex == 1) {
                                    single_edge_container.height = 500
                                } else {
                                    // if (previous_index == 2) {
                                    //     // single_edge_container_animation_params_into.start()
                                    //     single_edge_container.height = 465
                                    // }
                                    single_edge_container.height = 465
                                }

                                // previous_index = lineTypeCombo.currentIndex

                            }

                            Layout.preferredWidth: 180
                            Layout.preferredHeight: 40
                            model: ["Line", "Arc", "BCurve"]
                            currentIndex: 0

                            background: Rectangle {
                                color: current_scheme.accent
                                radius: 5
                            }

                            contentItem: Text {
                                text: lineTypeCombo.displayText
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
                                width: lineTypeCombo.width
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
                                highlighted: lineTypeCombo.highlightedIndex === index
                            }

                            popup: Popup {
                                y: lineTypeCombo.height - 10
                                width: lineTypeCombo.width
                                implicitHeight: contentItem.implicitHeight
                                background: null
                                contentItem: ListView {
                                    clip: true
                                    implicitHeight: contentHeight
                                    implicitWidth: lineTypeCombo.width
                                    model: lineTypeCombo.popup.visible ? lineTypeCombo.delegateModel : null
                                    currentIndex: lineTypeCombo.highlightedIndex
                                }
                            }
                        }

                        ColumnLayout {
                            id: arcParams
                            visible: lineTypeCombo.currentIndex === 1
                            spacing: 5
                            Layout.alignment: Qt.AlignHCenter

                            Text {
                                text: "Ratio:"
                                font.pixelSize: 14
                                color: current_scheme.text
                            }

                            TextField {
                                id: arcRatioField
                                Layout.fillWidth: true
                                placeholderText: "0.0-1.0"
                                placeholderTextColor: current_scheme.grayed_text
                                // Keep graph parameters locale-independent: only a dot is valid as the decimal separator.
                                validator: RegularExpressionValidator {
                                    regularExpression: /^(?:0(?:\.\d{0,2})?|1(?:\.0{0,2})?)$/
                                }
                                color: current_scheme.text
                                background: Rectangle {
                                    color: current_scheme.accent
                                    radius: 5
                                }
                            }
                        }

                        ColumnLayout {
                            id: bcurveParams
                            visible: lineTypeCombo.currentIndex === 2
                            spacing: 5
                            Layout.alignment: Qt.AlignHCenter

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.alignment: Qt.AlignHCenter
                                ColumnLayout {
                                    Layout.preferredWidth: 100
                                    Layout.alignment: Qt.AlignLeft
                                    Text {
                                        text: "Along1:"
                                        font.pixelSize: 14
                                        color: current_scheme.text
                                    }
                                    TextField {
                                        id: along1Field
                                        Layout.fillWidth: true
                                        placeholderText: "Along1"
                                        placeholderTextColor: current_scheme.grayed_text
                                        color: current_scheme.text
                                        background: Rectangle {
                                            color: current_scheme.accent
                                            radius: 5
                                        }
                                    }
                                }
                                ColumnLayout {
                                    Layout.preferredWidth: 100
                                    Layout.alignment: Qt.AlignRight
                                    Text {
                                        text: "Across1:"
                                        font.pixelSize: 14
                                        color: current_scheme.text
                                    }
                                    TextField {
                                        id: across1Field
                                        Layout.fillWidth: true
                                        placeholderText: "Across1"
                                        placeholderTextColor: current_scheme.grayed_text
                                        color: current_scheme.text
                                        background: Rectangle {
                                            color: current_scheme.accent
                                            radius: 5
                                        }
                                    }
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.alignment: Qt.AlignHCenter
                                ColumnLayout {
                                    Layout.preferredWidth: 100
                                    Layout.alignment: Qt.AlignLeft
                                    Text {
                                        text: "Along2:"
                                        font.pixelSize: 14
                                        color: current_scheme.text
                                    }
                                    TextField {
                                        id: along2Field
                                        Layout.fillWidth: true
                                        placeholderText: "Along2"
                                        placeholderTextColor: current_scheme.grayed_text
                                        color: current_scheme.text
                                        background: Rectangle {
                                            color: current_scheme.accent
                                            radius: 5
                                        }
                                    }
                                }
                                ColumnLayout {
                                    Layout.preferredWidth: 100
                                    Layout.alignment: Qt.AlignRight
                                    Text {
                                        text: "Across2:"
                                        font.pixelSize: 14
                                        color: current_scheme.text
                                    }
                                    TextField {
                                        id: across2Field
                                        Layout.fillWidth: true
                                        placeholderText: "Across2"
                                        placeholderTextColor: current_scheme.grayed_text
                                        color: current_scheme.text
                                        background: Rectangle {
                                            color: current_scheme.accent
                                            radius: 5
                                        }
                                    }
                                }
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: 5
                        spacing: 20

                        ButtonTemplate {
                            button_text: "Apply"
                            button_width: 100
                            button_height: 35
                            button_color: current_scheme.button
                            text_color: current_scheme.text
                            button_hover_color: current_scheme.button_hover
                            onClicked: {
                                let edge_vertices = manager.GetSelectedEdgeVertices()
                                let edge_group_name = manager.GetSelectedEdgeGroupName()

                                if (single_edge_combobox.currentIndex <= 1) {
                                    if (colorRadio.checked) {
                                        if (single_edge_combobox.currentIndex == 1) {
                                            let entered_name = new_groupname_loader.item.getParameters();
                                            if (group_names.includes(entered_name)) {
                                                new_groupname_loader.item.makeWrong();
                                            } else {
                                                new_groupname_loader.item.makeOkay();

                                                manager.AddGroup(edge_vertices[0],
                                                    edge_vertices[1],
                                                    entered_name,
                                                    single_edge_color_selector.selected_color,
                                                    single_edge_useasgroup.checked
                                                );
                                            }
                                        } else {
                                            manager.ChangeColor(edge_vertices[0],
                                                edge_vertices[1],
                                                single_edge_color_selector.selected_color,
                                                single_edge_useasgroup.checked
                                            );
                                        }
                                    } else {
                                        let params = {}
                                        if (lineTypeCombo.currentIndex === 0) {
                                            params.type = 0;
                                        } else if (lineTypeCombo.currentIndex === 1) {
                                            params.type = 1;
                                            params.ratio = parseFloat(arcRatioField.text)
                                        } else if (lineTypeCombo.currentIndex === 2) {
                                            params.type = 2;
                                            params.along1 = parseFloat(along1Field.text)
                                            params.across1 = parseFloat(across1Field.text)
                                            params.along2 = parseFloat(along2Field.text)
                                            params.across2 = parseFloat(across2Field.text)
                                        }

                                        if (single_edge_combobox.currentIndex == 1) {
                                            let entered_name = new_groupname_loader.item.getParameters();
                                            if (group_names.includes(entered_name)) {
                                                new_groupname_loader.item.makeWrong();
                                            } else {
                                                new_groupname_loader.item.makeOkay();
                                                manager.AddGroupWithParameters(edge_vertices[0],
                                                    edge_vertices[1],
                                                    entered_name,
                                                    params,
                                                    single_edge_useasgroup.checked
                                                );
                                            }
                                        } else {
                                            manager.ChangeParameters(edge_vertices[0],
                                                edge_vertices[1],
                                                params,
                                                single_edge_useasgroup.checked
                                            );
                                        }
                                    }
                                } else {
                                    manager.AddToGroup(edge_vertices[0],
                                        edge_vertices[1],
                                        single_edge_combobox.currentIndex - 2,
                                        single_edge_useasgroup.checked
                                    );
                                }

                                manager.DeselectEdges();
                                single_edge_container.visible = false;
                            }
                        }

                        ButtonTemplate {
                            button_text: "Cancel"
                            button_height: 35
                            button_width: 100
                            button_color: current_scheme.button
                            text_color: current_scheme.text
                            button_hover_color: current_scheme.button_hover
                            onClicked: {
                                single_edge_container.visible = false
                                manager.DeselectEdges();
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: pathDialog
                visible: false
                width: 260
                height: 290
                color: current_scheme.background
                radius: 10
                border.color: "#353535"
                border.width: 1
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 40

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20

                    id: shortest_path_component

                    Label {
                        text: "Enter vertices to find the shortest path:"
                        Layout.preferredWidth: 230
                        wrapMode: Text.Wrap
                        font.pixelSize: 18
                        Layout.alignment: Qt.AlignHCenter
                        color: current_scheme.text
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignLeft
                        spacing: 10

                        Label {
                            text: "Start vertex:"
                            Layout.alignment: Qt.AlignLeft
                            font.pixelSize: 14
                            color: current_scheme.text
                        }

                        TextField {
                            id: vertex_field_1
                            width: 100
                            height: 35
                            placeholderText: "index"
                            Layout.alignment: Qt.AlignLeft

                            background: Rectangle {
                                color: current_scheme.textarea_color
                            }
                            placeholderTextColor: current_scheme.grayed_text
                            color: current_scheme.text
                        }
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignLeft
                        spacing: 10

                        Label {
                            text: "End vertex:"
                            Layout.alignment: Qt.AlignLeft
                            font.pixelSize: 14
                            color: current_scheme.text
                        }

                        TextField {
                            id: vertex_field_2
                            width: 150
                            height: 35
                            placeholderText: "index"
                            Layout.alignment: Qt.AlignLeft

                            background: Rectangle {
                                color: current_scheme.textarea_color
                            }
                            placeholderTextColor: current_scheme.grayed_text
                            color: current_scheme.text
                        }
                    }


                    ColorSelector {
                        id: shortest_path_color_selector
                        Layout.alignment: Qt.AlignHCenter
                        selector_width: 210
                        selector_text: "Choose edge color"
                    }

                    Text {
                        id: vertex_wrong_info
                        text: "One or more vertices do not exist in the graph"
                        Layout.alignment: Qt.AlignLeft
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                        height: 17
                        color: current_scheme.wrong_text
                        opacity: 0
                    }

                    function checkParameters() {
                        if (vertex_field_1.text.trim().length === 0 || vertex_field_2.text.trim().length === 0 ||
                            isNaN(vertex_field_1.text) || isNaN(vertex_field_2.text)) {
                            vertex_wrong_info.text = "A vertex index must be a number and cannot be empty"
                            vertex_wrong_info.opacity = 1
                            return false
                        }

                        return true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignLeft
                        spacing: 20

                        ButtonTemplate {
                            button_text: "Apply"
                            button_width: 100
                            button_color: current_scheme.button
                            button_hover_color: current_scheme.button_hover
                            text_color: current_scheme.text
                            onClicked: {
                                if (shortest_path_component.checkParameters()) {
                                    if (!manager.CheckVerticesExist(vertex_field_1.text, vertex_field_2.text)) {
                                        vertex_wrong_info.text = "One or more vertices do not exist in the graph"
                                        vertex_wrong_info.opacity = 1
                                    } else {
                                        pathDialog.visible = false;
                                        vertex_wrong_info.opacity = 0
                                        manager.DrawShortestPath(parseInt(vertex_field_1.text, 10), parseInt(vertex_field_2.text, 10),
                                            shortest_path_color_selector.selected_color);
                                    }
                                }
                            }
                        }

                        ButtonTemplate {
                            button_text: "Close"
                            button_width: 100
                            button_color: current_scheme.button
                            text_color: current_scheme.text
                            button_hover_color: current_scheme.button_hover
                            onClicked: {
                                pathDialog.visible = false
                            }
                        }
                    }
                }
            }

            RowLayout {
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 20

                // width: 200
                // height: 35

                Button {
                    Layout.alignment: Qt.AlignRight
                    Layout.rightMargin: 20

                    id: reset_positions_button
                    enabled: false
                    property string button_color: reset_positions_button.enabled ? current_scheme.button_hover : current_scheme.button
                    property string button_hover_color: button_color
                    onClicked: reset_positions()

                    flat: true
                    highlighted: false
                    implicitWidth: 45
                    implicitHeight: 45

                    background: Rectangle {
                        color: reset_positions_button.hovered ? reset_positions_button.button_hover_color : reset_positions_button.button_color
                        border.color: "transparent"
                        radius: 4
                        border.pixelAligned: true

                        Image {
                            anchors.centerIn: parent
                            width: 20
                            height: 20
                            opacity: reset_positions_button.enabled ? 1.0 : 0.5
                            source: current_scheme.reset_positions_icon_path
                        }

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
                }

                Button {
                    Layout.alignment: Qt.AlignRight

                    id: outline_mode
                    enabled: true
                    property string button_color: outline_mode.enabled ? current_scheme.button_hover : current_scheme.button
                    property string button_hover_color: button_color
                    onClicked: change_outline_mode()

                    flat: true
                    highlighted: false
                    implicitWidth: 45
                    implicitHeight: 45

                    background: Rectangle {
                        color: outline_mode.hovered ? outline_mode.button_hover_color : outline_mode.button_color
                        border.color: "transparent"
                        radius: 4
                        border.pixelAligned: true

                        Image {
                            anchors.centerIn: parent
                            width: 25
                            height: 25
                            opacity: outline_mode.enabled ? 1.0 : 0.5
                            source: current_scheme.outline_mode_icon_path
                        }

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
                }

                Button {
                    Layout.alignment: Qt.AlignRight
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20

                    id: move_mode
                    enabled: false
                    property string button_color: move_mode.enabled ? current_scheme.button_hover : current_scheme.button
                    property string button_hover_color: button_color
                    onClicked: change_outline_mode()

                    flat: true
                    highlighted: false
                    implicitWidth: 45
                    implicitHeight: 45

                    background: Rectangle {
                        color: move_mode.hovered ? move_mode.button_hover_color : move_mode.button_color
                        border.color: "transparent"
                        radius: 4
                        border.pixelAligned: true

                        Image {
                            anchors.centerIn: parent
                            width: 25
                            height: 25
                            opacity: move_mode.enabled ? 1.0 : 0.5
                            source: current_scheme.move_mode_icon_path
                        }

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
                }

                Button {
                    Layout.alignment: Qt.AlignRight

                    id: shortest_path
                    property string button_color: current_scheme.button
                    property string button_hover_color: current_scheme.button_hover
                    onClicked: {
                        pathDialog.visible = true
                        manager.DeselectEdges()
                        single_edge_container.visible = false;
                    }

                    flat: true
                    highlighted: false
                    implicitWidth: 45
                    implicitHeight: 45

                    background: Rectangle {
                        color: shortest_path.hovered ? shortest_path.button_hover_color : shortest_path.button_color
                        border.color: "transparent"
                        radius: 4
                        border.pixelAligned: true

                        Image {
                            anchors.centerIn: parent
                            width: 25
                            height: 25
                            source: current_scheme.shortest_path_icon_path
                        }

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
                }


                // ButtonTemplate {
                //     id: shortest_path
                //     button_width: 130
                //     button_height: 35
                //     Layout.alignment: Qt.AlignRight
                //     button_color: current_scheme.button
                //     button_hover_color: current_scheme.button_hover
                //     button_text: "Calculate Path"
                //     text_color: current_scheme.button_text
                //     onClicked: {
                //         pathDialog.visible = true
                //         manager.DeselectEdges()
                //         single_edge_container.visible = false;
                //     }
                // }

            }

            Column {
                id: parameters
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: 30
                    bottomMargin: 90
                }
                anchors.rightMargin: 40
                spacing: 8

                Text {
                    text: "<b>Topology parameters:</b>"
                    font.pixelSize: 16
                    bottomPadding: 10
                    color: current_scheme.text
                }
                Text {
                    text: `Diameter: ${graphResults?.diameter ?? "-"}`
                    font.pixelSize: 14
                    color: current_scheme.text
                }
                Text {
                    text: `Average distance: ${graphResults?.average_distance?.toFixed(2) ?? "-"}`
                    font.pixelSize: 14
                    color: current_scheme.text
                }
                Text {
                    text: `Maximum degree: ${graphResults?.max_node_degree ?? "-"}`
                    font.pixelSize: 14
                    color: current_scheme.text
                }
                Text {
                    text: `Global network cost: ${graphResults?.global_network_cost ?? "-"}`
                    font.pixelSize: 14
                    color: current_scheme.text
                }
                Text {
                    text: `Global packing density: ${graphResults?.global_packing_density?.toFixed(3) ?? "-"}`
                    font.pixelSize: 14
                    color: current_scheme.text
                }
                Text {
                    text: `Modified network cost: ${graphResults?.modified_network_cost ?? "-"}`
                    font.pixelSize: 14
                    color: current_scheme.text
                }
                Text {
                    text: `Modified packing density: ${graphResults?.modified_packing_density?.toFixed(3) ?? "-"}`
                    font.pixelSize: 14
                    color: current_scheme.text
                }
            }
        }

        ThemeButton {
            id: theme_button
        }


        ButtonTemplate {
            id: computeButton
            button_text: "Calculate parameters"
            button_width: 180
            button_height: 40
            button_color: current_scheme.button
            button_hover_color: current_scheme.button_hover
            text_color: current_scheme.text
            anchors {
                left: parent.left
                bottom: parent.bottom
                margins: 20
            }

            onClicked: {
                busyIndicator.running = true
                manager.computeParameters()
            }
        }

        BusyIndicator {
            id: busyIndicator
            anchors.centerIn: parent
            running: false
            width: 60
            height: 60
            palette.dark: current_scheme.text
        }


        Connections {
            target: manager

            function onComputationFinished(result) {
                graphResults = result
                busyIndicator.running = false
            }
        }
        Connections {
            target: manager

            function onThemeChanged() {
                isDarkTheme = manager.isDark()
            }
        }

        Component {
            id: empty_component
            Rectangle {
                height: 0
                width: 0
            }
        }

        Component {
            id: groupname_component

            ColumnLayout {
                spacing: 12
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 10

                TextField {
                    id: groupname
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: 230
                    Layout.preferredHeight: 30
                    Layout.leftMargin: 24
                    placeholderText: "group name"
                    background: Rectangle {
                        color: current_scheme.textarea_color
                    }
                    color: current_scheme.text
                    placeholderTextColor: current_scheme.grayed_text
                }

                Text {
                    id: groupname_incorrect
                    Layout.alignment: Qt.AlignHCenter
                    opacity: 0
                    color: current_scheme.wrong_text
                    text: "Invalid input"
                    font.pixelSize: 12
                }

                function checkParameters() {


                    return true
                }

                function getParameters() {
                    return groupname.text
                }

                function makeWrong() {
                    groupname_incorrect.opacity = 1;
                }

                function makeOkay() {
                    groupname_incorrect.opacity = 0;
                }
            }
        }

    }
}
