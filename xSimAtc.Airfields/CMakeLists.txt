add_library(
    xSimAtc.Airfields
    STATIC
    pch.cpp
)

target_sources(
    xSimAtc.Airfields
    PRIVATE
        Collections/collection.h
        Markers/aircraft_marker.h
        Markers/tower_marker.h
        Nodes/airfield_node.h
        Positions/airfield_position.h
        Routing/airfield_route.h
        Routing/airfield_segment.h
        Runways/runway.h
        airfield.h
        airfield_layout.h
)

source_group(
    "Collections"
    FILES
        Collections/collection.h
)

source_group(
    "Markers"
    FILES
        Markers/aircraft_marker.h
        Markers/tower_marker.h
)

source_group(
    "Nodes"
    FILES
        Nodes/airfield_node.h
)

source_group(
    "Positions"
    FILES
        Positions/airfield_position.h
)

source_group(
    "Routing"
    FILES
        Routing/airfield_route.h
        Routing/airfield_segment.h
)

source_group(
    "Runways"
    FILES
        Runways/runway.h
)

target_precompile_headers(
    xSimAtc.Airfields
    PRIVATE
        pch.h
)

target_include_directories(
    xSimAtc.Airfields
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if(MSVC)
    target_compile_options(
        xSimAtc.Airfields
        PRIVATE
            /std:c++latest
            /permissive-
    )
endif()
