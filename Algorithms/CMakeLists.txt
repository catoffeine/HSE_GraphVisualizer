add_library(Algorithms SHARED
        Algorithms.cpp
        AutoDetection.cpp
)

find_package(Qt6 COMPONENTS
        Core
        REQUIRED)

target_link_libraries(Algorithms PRIVATE
        Topologies
        Qt6::Core
)

target_include_directories(Topologies PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})