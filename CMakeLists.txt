cmake_minimum_required(VERSION 3.28)
project(GraphVisualizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Quick
        Concurrent
        Svg
        REQUIRED)

add_subdirectory(Topologies)
add_subdirectory(Layout)
add_subdirectory(Algorithms)

qt_add_executable(GraphVisualizer main.cpp
        PageManager.cpp
        CanvasWidget.cpp
)

qt6_add_qml_module(GraphVisualizer
        VERSION 1.0
        URI hello
        QML_FILES
        qml/main.qml
        qml/Definitions.qml
        qml/DefinitionsPrivate.qml
        qml/ThemeButton.qml
        qml/main_page.qml
        qml/ButtonTemplate.qml
        qml/Settings.qml
        qml/MainCanvas.qml
        qml/MainMenuItem.qml
        qml/topology_parameters.qml
        qml/ColorSelector.qml
)

target_link_libraries(GraphVisualizer PRIVATE
        Topologies
        Layout
        Algorithms
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Quick
        Qt::Concurrent
        Qt::Svg
)
