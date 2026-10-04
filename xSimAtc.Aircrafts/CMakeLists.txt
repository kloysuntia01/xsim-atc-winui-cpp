add_library(
    xSimAtc.Aircrafts
    STATIC
    aircraft.cpp
    aircraft_base.cpp
    transponder.cpp
)

target_precompile_headers(
    xSimAtc.Aircrafts
    PRIVATE
        pch.h
)

target_include_directories(
    xSimAtc.Aircrafts
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    xSimAtc.Aircrafts
    PUBLIC
        rxcpp
        ole32
)

if(MSVC)
    target_compile_options(
        xSimAtc.Aircrafts
        PRIVATE
            /std:c++latest
            /permissive-
    )

    target_compile_definitions(
        xSimAtc.Aircrafts
        PUBLIC
            _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING
    )
endif()