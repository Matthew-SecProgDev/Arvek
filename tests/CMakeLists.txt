# ArvekLoggerTest
add_executable(ArvekLoggerTest
    logger_test.cpp
)

target_link_libraries(ArvekLoggerTest
    PRIVATE ArvekEngine
)

add_test(
    NAME ArvekLoggerTest
    COMMAND ArvekLoggerTest
)

# ArvekAssertTest
add_executable(ArvekAssertTest
    assert_test.cpp
)

target_link_libraries(ArvekAssertTest
    PRIVATE ArvekEngine
)

add_test(
    NAME ArvekAssertPass
    COMMAND ArvekAssertTest
)

add_test(
    NAME ArvekAssertFailure
    CONFIGURATIONS Debug
    COMMAND ${CMAKE_COMMAND}
        "-DTEST_EXECUTABLE=$<TARGET_FILE:ArvekAssertTest>"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/check_assert_failure.cmake"
)

# ArvekServiceRegistryTest
add_executable(ArvekServiceRegistryTest
    service_registry_test.cpp
)

target_link_libraries(ArvekServiceRegistryTest
    PRIVATE ArvekEngine
)

target_include_directories(ArvekServiceRegistryTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/core
)

add_test(
    NAME ArvekServiceRegistryTest
    COMMAND ArvekServiceRegistryTest
)

# ArvekServiceHolderTest
add_executable(ArvekServiceHolderTest
    service_holder_test.cpp
)

target_compile_features(ArvekServiceHolderTest
    PRIVATE cxx_std_17
)

target_link_libraries(ArvekServiceHolderTest
    PRIVATE ArvekEngine
)

add_test(
    NAME ArvekServiceHolderTest
    COMMAND ArvekServiceHolderTest
)

# ArvekEngineLifecycleTest
add_executable(ArvekEngineLifecycleTest
    engine_lifecycle_test.cpp
)

target_link_libraries(ArvekEngineLifecycleTest
    PRIVATE ArvekEngine
)

add_test(
    NAME ArvekEngineLifecycleTest
    COMMAND ArvekEngineLifecycleTest
)

# ArvekGlfwRuntimeTest
add_executable(ArvekGlfwRuntimeTest
    glfw_runtime_test.cpp
)

target_link_libraries(ArvekGlfwRuntimeTest
    PRIVATE ArvekEngine
)

target_include_directories(ArvekGlfwRuntimeTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/core
)

add_test(
    NAME ArvekGlfwRuntimeTest
    COMMAND ArvekGlfwRuntimeTest
)

# ArvekGlfwWindowManualTest
add_executable(ArvekGlfwWindowManualTest
    glfw_window_manual_test.cpp
)

target_link_libraries(ArvekGlfwWindowManualTest
    PRIVATE
        ArvekEngine
        Vulkan::Vulkan
)

target_include_directories(ArvekGlfwWindowManualTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/core
)

# ArvekVulkanInstanceTest
add_executable(ArvekVulkanInstanceTest
    vulkan_instance_test.cpp
)

target_link_libraries(ArvekVulkanInstanceTest
    PRIVATE
        ArvekEngine
        Vulkan::Vulkan
)

target_include_directories(ArvekVulkanInstanceTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/rendering
)

add_test(
    NAME ArvekVulkanInstanceTest
    COMMAND ArvekVulkanInstanceTest
)

# ArvekVulkanSurfaceTest
add_executable(ArvekVulkanSurfaceTest
    vulkan_surface_test.cpp
)

target_link_libraries(ArvekVulkanSurfaceTest
    PRIVATE
        ArvekEngine
        Vulkan::Vulkan
)

target_include_directories(ArvekVulkanSurfaceTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/rendering
)

add_test(
    NAME ArvekVulkanSurfaceTest
    COMMAND ArvekVulkanSurfaceTest
)

# ArvekVulkanPhysicalDeviceTest
add_executable(ArvekVulkanPhysicalDeviceTest
    vulkan_physical_device_test.cpp
)

target_link_libraries(ArvekVulkanPhysicalDeviceTest
    PRIVATE
        ArvekEngine
        Vulkan::Vulkan
)

target_include_directories(ArvekVulkanPhysicalDeviceTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/rendering
)

add_test(
    NAME ArvekVulkanPhysicalDeviceTest
    COMMAND ArvekVulkanPhysicalDeviceTest
)

# ArvekVulkanDeviceTest
add_executable(ArvekVulkanDeviceTest
    vulkan_device_test.cpp
)

target_link_libraries(ArvekVulkanDeviceTest
    PRIVATE
        ArvekEngine
        Vulkan::Vulkan
)

target_include_directories(ArvekVulkanDeviceTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/rendering
)

add_test(
    NAME ArvekVulkanDeviceTest
    COMMAND ArvekVulkanDeviceTest
)

# ArvekVulkanValidationTest
add_executable(ArvekVulkanValidationTest
    vulkan_validation_test.cpp
)

target_link_libraries(ArvekVulkanValidationTest
    PRIVATE
        ArvekEngine
        Vulkan::Vulkan
)

target_include_directories(ArvekVulkanValidationTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/rendering
)

add_test(
    NAME ArvekVulkanValidationTest
    COMMAND ArvekVulkanValidationTest
)

# ArvekVulkanSwapchainTest
add_executable(ArvekVulkanSwapchainTest
    vulkan_swapchain_test.cpp
)

target_link_libraries(ArvekVulkanSwapchainTest
    PRIVATE
        ArvekEngine
        Vulkan::Vulkan
)

target_include_directories(ArvekVulkanSwapchainTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/rendering
)

add_test(
    NAME ArvekVulkanSwapchainTest
    COMMAND ArvekVulkanSwapchainTest
)

# ArvekVulkanCommandPoolTest
add_executable(ArvekVulkanCommandPoolTest
    vulkan_command_pool_test.cpp
)

target_link_libraries(ArvekVulkanCommandPoolTest
    PRIVATE
        ArvekEngine
        Vulkan::Vulkan
)

target_include_directories(ArvekVulkanCommandPoolTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/rendering
)

add_test(
    NAME ArvekVulkanCommandPoolTest
    COMMAND ArvekVulkanCommandPoolTest
)

# ArvekVulkanQueueSubmissionTest
add_executable(ArvekVulkanQueueSubmissionTest
    vulkan_queue_submission_test.cpp
)

target_link_libraries(ArvekVulkanQueueSubmissionTest
    PRIVATE
        ArvekEngine
        Vulkan::Vulkan
)

target_include_directories(ArvekVulkanQueueSubmissionTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/rendering
)

add_test(
    NAME ArvekVulkanQueueSubmissionTest
    COMMAND ArvekVulkanQueueSubmissionTest
)

# ArvekVulkanClearPresentManualTest
add_executable(ArvekVulkanClearPresentManualTest
    vulkan_clear_present_manual_test.cpp
)

target_link_libraries(ArvekVulkanClearPresentManualTest
    PRIVATE
        ArvekEngine
        Vulkan::Vulkan
)

target_include_directories(ArvekVulkanClearPresentManualTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/rendering
)

# ArvekVulkanFrameLoopManualTest
add_executable(ArvekVulkanFrameLoopManualTest
    vulkan_frame_loop_manual_test.cpp
)

target_link_libraries(ArvekVulkanFrameLoopManualTest
    PRIVATE
        ArvekEngine
        Vulkan::Vulkan
)

target_include_directories(ArvekVulkanFrameLoopManualTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/rendering
)

# Triangle shaders
find_program(GLSLC_EXECUTABLE
    NAMES glslc
    HINTS
        "$ENV{VULKAN_SDK}/Bin"
        "$ENV{VULKAN_SDK}/bin"
    REQUIRED
)

set(TRIANGLE_SHADER_SOURCE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
)

set(TRIANGLE_SHADER_BINARY_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/shaders"
)

set(TRIANGLE_VERTEX_SPV
    "${TRIANGLE_SHADER_BINARY_DIR}/triangle.vert.spv"
)

set(TRIANGLE_FRAGMENT_SPV
    "${TRIANGLE_SHADER_BINARY_DIR}/triangle.frag.spv"
)

foreach(stage IN ITEMS vert frag)
    add_custom_command(
        OUTPUT
            "${TRIANGLE_SHADER_BINARY_DIR}/triangle.${stage}.spv"

        COMMAND
            "${CMAKE_COMMAND}" -E make_directory
            "${TRIANGLE_SHADER_BINARY_DIR}"

        COMMAND
            "${GLSLC_EXECUTABLE}"
            --target-env=vulkan1.0
            "${TRIANGLE_SHADER_SOURCE_DIR}/triangle.${stage}"
            -o
            "${TRIANGLE_SHADER_BINARY_DIR}/triangle.${stage}.spv"

        DEPENDS
            "${TRIANGLE_SHADER_SOURCE_DIR}/triangle.${stage}"

        COMMENT
            "Compiling triangle.${stage} to SPIR-V"

        VERBATIM
    )
endforeach()

add_custom_target(ArvekTriangleShaders
    DEPENDS
        "${TRIANGLE_VERTEX_SPV}"
        "${TRIANGLE_FRAGMENT_SPV}"
)

# ArvekVulkanShaderModuleTest
add_executable(ArvekVulkanShaderModuleTest
    vulkan_shader_module_test.cpp
)

target_link_libraries(ArvekVulkanShaderModuleTest
    PRIVATE
        ArvekEngine
        Vulkan::Vulkan
)

target_include_directories(ArvekVulkanShaderModuleTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/src/rendering
)

add_dependencies(
    ArvekVulkanShaderModuleTest
    ArvekTriangleShaders
)

add_test(
    NAME ArvekVulkanShaderModuleTest

    COMMAND
        ArvekVulkanShaderModuleTest
        "${TRIANGLE_VERTEX_SPV}"
        "${TRIANGLE_FRAGMENT_SPV}"
)