add_executable(
    xSimAtc.Inference.Cli
    main.cpp
)

target_compile_features(
    xSimAtc.Inference.Cli
    PRIVATE
        cxx_std_23
)

target_include_directories(
    xSimAtc.Inference.Cli
    PRIVATE
        ${CMAKE_SOURCE_DIR}/xSimAtc.Inference
        ${CMAKE_SOURCE_DIR}/xSimAtc.Speech
)

target_link_libraries(
    xSimAtc.Inference.Cli
    PRIVATE
        xSimAtc.Inference
        xSimAtc.Speech
)

add_dependencies(
    xSimAtc.Inference.Cli
    xSimAtc.Inference
    xSimAtc.Speech
)

# LLAMA-001H64F: Deploy the pinned llama.cpp runtime.
# This remains separate from whisper.cpp's bundled GGML.
foreach(_dll IN ITEMS
    llama.dll
    ggml.dll
    ggml-base.dll
    ggml-cpu.dll
    ggml-vulkan.dll)

    add_custom_command(
        TARGET xSimAtc.Inference.Cli
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${XSIM_LLAMA_RUNTIME_DIR}/${_dll}"
            "$<TARGET_FILE_DIR:xSimAtc.Inference.Cli>/${_dll}"
        VERBATIM
    )
endforeach()