add_executable(
    xSimAtc.Inference.Tests
    llama_model_tests.cpp
    llama_inference_request_tests.cpp
)

target_link_libraries(
    xSimAtc.Inference.Tests
    PRIVATE
        xSimAtc.Inference
        GTest::gtest_main
)

target_include_directories(
    xSimAtc.Inference.Tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/xSimAtc.Inference
)

if(MSVC)
    target_compile_options(
        xSimAtc.Inference.Tests
        PRIVATE
            /std:c++latest
            /permissive-
    )
endif()

# LLAMA-001H64M: Deploy the canonical llama.cpp runtime before
# GoogleTest POST_BUILD discovery executes the test executable.
foreach(_dll IN ITEMS
    llama.dll
    ggml.dll
    ggml-base.dll
    ggml-cpu.dll
    ggml-vulkan.dll)

    add_custom_command(
        TARGET xSimAtc.Inference.Tests
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${XSIM_LLAMA_RUNTIME_DIR}/${_dll}"
            "$<TARGET_FILE_DIR:xSimAtc.Inference.Tests>/${_dll}"
        VERBATIM
    )
endforeach()
include(GoogleTest)

gtest_discover_tests(
    xSimAtc.Inference.Tests
)
