cmake_minimum_required(VERSION 3.28)

if(NOT MSVC)
    return()
endif()

set(XSIM_DEV_DIR "C:/Dev/Xsim.Dev")

add_custom_target(
    xSimAtc.Terminal.WinUI
    ALL

    COMMAND
        "${CMAKE_VS_MSBUILD_COMMAND}"
        "${CMAKE_CURRENT_SOURCE_DIR}/xSimAtc.Terminal.WinUI.vcxproj"
        /p:Configuration=$<CONFIG>
        /p:Platform=x64

    COMMAND
        "${CMAKE_COMMAND}" -E make_directory
        "${XSIM_DEV_DIR}"

    COMMAND
        "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/x64/$<CONFIG>/xSimAtc.Terminal.WinUI"
        "${XSIM_DEV_DIR}"

    WORKING_DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}"

    COMMENT
        "Building and publishing xSimAtc.Terminal.WinUI to ${XSIM_DEV_DIR}"

    VERBATIM
    USES_TERMINAL
)
