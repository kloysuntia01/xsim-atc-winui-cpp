add_executable(
    xSimAtc.Airfields.Tests
    pch.cpp
    airfield_tests.cpp
    airfield_route_tests.cpp
    airfield_waypoint_movement_tests.cpp
)

target_precompile_headers(
    xSimAtc.Airfields.Tests
    PRIVATE
        pch.h
)

target_link_libraries(
    xSimAtc.Airfields.Tests
    PRIVATE
        xSimAtc.Airfields
        GTest::gtest_main
)

if(MSVC)
    target_compile_options(
        xSimAtc.Airfields.Tests
        PRIVATE
            /std:c++latest
            /permissive-
    )
endif()

include(GoogleTest)

gtest_discover_tests(
    xSimAtc.Airfields.Tests
)


# Airfield taxiway graph slice
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_graph_tests.cpp
)

# Option-B taxiway layout mapping slice
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_taxiway_layout_tests.cpp
)

# Option-B intermediate taxiway overlay nodes
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_taxiway_overlay_tests.cpp
)

# Full Option-B taxiway network
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_full_taxiway_network_tests.cpp
)

# Interactive Option-B taxiway nodes
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_taxiway_selection_tests.cpp
)

# Taxiway graph -> movement waypoint route
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_taxiway_waypoint_route_tests.cpp
)

# Selected taxiway route -> AirfieldMovement
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_movement_route_binding_tests.cpp
)

# Live taxi runtime coordinator
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_taxi_runtime_tests.cpp
)

# Option-B physical waypoint calibration
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_optionb_waypoint_calibration_tests.cpp
)

# Taxi runtime completion hook
target_sources(xSimAtc.Airfields.Tests PRIVATE
    airfield_taxi_runtime_completion_hook_tests.cpp
)
