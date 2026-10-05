add_library(
    xSimAtc.Towers
    STATIC
    tower.cpp
    tower_base.cpp
)

target_precompile_headers(
    xSimAtc.Towers
    PRIVATE
        pch.h
)

target_include_directories(
    xSimAtc.Towers
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    xSimAtc.Towers
    PUBLIC
        rxcpp
        ole32
)

if(MSVC)
    target_compile_options(
        xSimAtc.Towers
        PRIVATE
            /std:c++latest
            /permissive-
    )

    target_compile_definitions(
        xSimAtc.Towers
        PUBLIC
            _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING
    )
endif()
