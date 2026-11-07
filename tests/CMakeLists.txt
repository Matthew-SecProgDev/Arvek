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