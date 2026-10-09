add_library(Layout SHARED
        ALVertex.cpp
        ALEdge.cpp
)

find_package(Qt6 COMPONENTS
        Gui
        REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Topologies)
target_link_libraries(Layout PRIVATE
        Topologies
        Qt6::Gui
)

target_include_directories(Layout PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})