# LLAMA-001H64D: Shared canonical llama.cpp runtime.
# Keep llama.cpp outside the main build graph to isolate whisper.cpp's GGML.
set(XSIM_LLAMA_PREBUILT_ROOT
    "C:/Dev/llama-runtime/ead9f3205/win-x64"
    CACHE PATH "Pinned llama.cpp prebuilt runtime")

foreach(_config IN ITEMS Debug Release)
    foreach(_relative IN ITEMS
        include/llama.h
        lib/llama.lib
        lib/ggml.lib
        lib/ggml-base.lib
        lib/ggml-cpu.lib
        bin/llama.dll
        bin/ggml.dll
        bin/ggml-base.dll
        bin/ggml-cpu.dll
        bin/ggml-vulkan.dll)
        if(NOT EXISTS "${XSIM_LLAMA_PREBUILT_ROOT}/${_config}/${_relative}")
            message(FATAL_ERROR
                "Missing pinned llama.cpp artifact: "
                "${XSIM_LLAMA_PREBUILT_ROOT}/${_config}/${_relative}")
        endif()
    endforeach()
endforeach()

set(XSIM_LLAMA_INCLUDE_DIR
    "${XSIM_LLAMA_PREBUILT_ROOT}/$<CONFIG>/include")
set(XSIM_LLAMA_RUNTIME_DIR
    "${XSIM_LLAMA_PREBUILT_ROOT}/$<CONFIG>/bin")
add_library(
    xsimatc_llama_external
    SHARED
    IMPORTED
    GLOBAL
)

set_target_properties(
    xsimatc_llama_external
    PROPERTIES
        IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
        IMPORTED_IMPLIB_DEBUG "${XSIM_LLAMA_PREBUILT_ROOT}/Debug/lib/llama.lib"
        IMPORTED_LOCATION_DEBUG "${XSIM_LLAMA_PREBUILT_ROOT}/Debug/bin/llama.dll"
        IMPORTED_IMPLIB_RELEASE "${XSIM_LLAMA_PREBUILT_ROOT}/Release/lib/llama.lib"
        IMPORTED_LOCATION_RELEASE "${XSIM_LLAMA_PREBUILT_ROOT}/Release/bin/llama.dll"
        INTERFACE_INCLUDE_DIRECTORIES "${XSIM_LLAMA_PREBUILT_ROOT}/Debug/include"
)

add_library(
    xSimAtc.Inference
    STATIC
    llama_model.cpp
    llama_cpp_inference.cpp
    pch.cpp
)

target_include_directories(
    xSimAtc.Inference
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_precompile_headers(
    xSimAtc.Inference
    PRIVATE
        pch.h
)

target_link_libraries(
    xSimAtc.Inference
    PRIVATE
        xsimatc_llama_external
)

if(MSVC)
    target_compile_options(
        xSimAtc.Inference
        PRIVATE
            /std:c++latest
            /permissive-
    )
endif()

# Manual local-model smoke target.
# Intentionally not registered with CTest because it depends on a machine-local GGUF.
add_executable(
    xSimAtc.Inference.Smoke
    Smoke/main.cpp
)

target_link_libraries(
    xSimAtc.Inference.Smoke
    PRIVATE
        xSimAtc.Inference
)

if(MSVC)
    target_compile_options(
        xSimAtc.Inference.Smoke
        PRIVATE
            /std:c++latest
            /permissive-
    )
endif()

# Deploy the pinned runtime matching the active build configuration.
# All five DLLs are required to avoid loading unrelated GGML binaries.
foreach(_dll IN ITEMS
    llama.dll
    ggml.dll
    ggml-base.dll
    ggml-cpu.dll
    ggml-vulkan.dll)

    add_custom_command(
        TARGET xSimAtc.Inference.Smoke
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${XSIM_LLAMA_RUNTIME_DIR}/${_dll}"
            "$<TARGET_FILE_DIR:xSimAtc.Inference.Smoke>/${_dll}"
        VERBATIM
    )
endforeach()