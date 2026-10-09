add_library(Topologies SHARED
        GraphTopologies.cpp
)

find_package(Qt6 COMPONENTS
        Core
        REQUIRED)

target_link_libraries(Topologies PRIVATE
        Qt6::Core
)

target_include_directories(Topologies PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})