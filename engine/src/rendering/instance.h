#pragma once

#include <vulkan/vulkan.h>

namespace arvek
{
    class Instance final
    {
    public:
#ifdef NDEBUG
        explicit Instance(bool enableValidation = false);
#else
        explicit Instance(bool enableValidation = true);
#endif

        ~Instance();

        Instance(const Instance&) = delete;
        Instance& operator=(const Instance&) = delete;
        Instance(Instance&&) = delete;
        Instance& operator=(Instance&&) = delete;

        [[nodiscard]]
        VkInstance handle() const noexcept;

    private:
        VkInstance instance_{ VK_NULL_HANDLE };
        VkDebugUtilsMessengerEXT debugMessenger_{ VK_NULL_HANDLE };
    };
}