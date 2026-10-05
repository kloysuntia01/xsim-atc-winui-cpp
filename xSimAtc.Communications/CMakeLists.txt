add_library(
    xSimAtc.Communications
    STATIC
    communication_checkpoint.cpp
    message_queue.cpp
)

target_precompile_headers(
    xSimAtc.Communications
    PRIVATE
        pch.h
)

target_include_directories(
    xSimAtc.Communications
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    xSimAtc.Communications
    PUBLIC
        rxcpp
)

if(MSVC)
    target_compile_options(
        xSimAtc.Communications
        PRIVATE
            /std:c++latest
            /permissive-
    )

    target_compile_definitions(
        xSimAtc.Communications
        PUBLIC
            _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING
    )
endif()
