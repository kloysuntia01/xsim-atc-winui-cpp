cmake_minimum_required(VERSION 3.28)

if(NOT MSVC)
    return()
endif()

set(XSIM_DEV_DIR
    "C:/Dev/Xsim.Dev"
    CACHE PATH "xSimAtc Terminal publish directory")

add_custom_target(
    xSimAtc.Terminal.WinUI
    ALL

    COMMAND
        "${CMAKE_VS_MSBUILD_COMMAND}"
        "${CMAKE_CURRENT_SOURCE_DIR}/xSimAtc.Terminal.WinUI.vcxproj"
        /p:Configuration=$<CONFIG>
        /p:Platform=x64
        "/p:XSimCMakeBuildRoot=${CMAKE_BINARY_DIR}"
        "/p:XSimLlamaRuntimeRoot=${XSIM_LLAMA_PREBUILT_ROOT}"
        "/p:OutDir=${CMAKE_BINARY_DIR}/terminal-winui/$<CONFIG>/bin/"
        "/p:IntDir=${CMAKE_BINARY_DIR}/terminal-winui/$<CONFIG>/obj/"
        "/p:GeneratedFilesDir=${CMAKE_BINARY_DIR}/terminal-winui/$<CONFIG>/generated/"

    COMMAND
        "${CMAKE_COMMAND}" -E make_directory
        "${XSIM_DEV_DIR}"

    COMMAND
        "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_BINARY_DIR}/terminal-winui/$<CONFIG>/bin"
        "${XSIM_DEV_DIR}"

    WORKING_DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}"

    COMMENT
        "Building and publishing xSimAtc.Terminal.WinUI to ${XSIM_DEV_DIR}"

    VERBATIM
    USES_TERMINAL
)

# The WinUI smoke slice links the CMake-built Speech static library.
add_dependencies(
    xSimAtc.Terminal.WinUI
    xSimAtc.Speech
    xSimAtc.Inference
)

# LLAMA-001H64H: Publish the pinned llama.cpp runtime matching $<CONFIG>.
# Keep these DLLs together and separate from whisper.cpp's build artifacts.
foreach(_dll IN ITEMS
    llama.dll
    ggml.dll
    ggml-base.dll
    ggml-cpu.dll
    ggml-vulkan.dll)

    add_custom_command(
        TARGET xSimAtc.Terminal.WinUI
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${XSIM_LLAMA_RUNTIME_DIR}/${_dll}"
            "${XSIM_DEV_DIR}/${_dll}"
        VERBATIM
    )
endforeach()