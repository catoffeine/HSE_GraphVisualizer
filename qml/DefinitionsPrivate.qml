import QtQuick
import QtQuick.Controls

QtObject {
    readonly property var color_scheme: QtObject {
        readonly property var light: QtObject {
            readonly property string background: "#ffffff"
            readonly property string header_text: "#000"
            readonly property string grayed_text: "#848484"
            readonly property string text: "#4f4f4f"
            // readonly property string highlighted_text: "#ffffff"
            readonly property string wrong_text: "#b04747"
            readonly property string import_button: "#2970d1"

            readonly property string accent: "#f8f8f8"
            readonly property string textarea_color: "#f8f8f8"

            readonly property string button: "#f8f8f8"
            readonly property string colored_button: "#2970d1"
            readonly property string button_hover: "#f0f0f0"
            readonly property string button_text: "#464646"

            readonly property string theme_button: "#f8f8f8"
            readonly property string icon_path: "static_files/sun.svg"

            readonly property string move_mode_icon_path: "static_files/move_mode_bright.svg"
            readonly property string outline_mode_icon_path: "static_files/outline_mode_bright.svg"
            readonly property string shortest_path_icon_path: "static_files/shortest_path_bright.svg"
            readonly property string reset_positions_icon_path: "static_files/reset_bright.svg"

            readonly property string grid_color: "#c3c3c3"
        }

        readonly property var dark: QtObject {
            readonly property string background: "#1e1e1e"
            readonly property string header_text: "#fff"
            readonly property string grayed_text: "#848484"
            readonly property string import_button: "#4477c7"
            readonly property string text: "#fff"
            readonly property string wrong_text: "#b04747"

            readonly property string textarea_color: "#232323"
            readonly property string accent: "#232323"

            readonly property string button: "#212121"
            readonly property string button_hover: "#272727"
            readonly property string colored_button: "#4477c7"
            readonly property string button_text: "#ffffff"

            readonly property string theme_button: "#1a1a1a"
            readonly property string icon_path: "static_files/moon.svg"

            readonly property string move_mode_icon_path: "static_files/move_mode_dark.svg"
            readonly property string outline_mode_icon_path: "static_files/outline_mode_dark.svg"
            readonly property string shortest_path_icon_path: "static_files/shortest_path_dark.svg"
            readonly property string reset_positions_icon_path: "static_files/reset_dark.svg"

            readonly property string grid_color: "#484848"
        }
    }

    readonly property var font_sizes: QtObject {
        readonly property int header: 24
        readonly property int main: 16
        readonly property int info: 12
    }

}