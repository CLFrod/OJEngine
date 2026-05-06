#define GLM_FORCE_RADIANS 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // vulkan 0 - 1
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include "Application.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <ranges>
#include <algorithm>
#include <string>
#include <map>
#include <vulkan/vulkan_raii.hpp>

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif


namespace OJEngine{

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

const std::vector<char const*> validationLayers = {
    "VK_LAYER_KHRONOS_validation" // single bundled main validation layer, can replace with individual ones though
};

std::vector<const char*> requiredDeviceExtensions = { vk::KHRSwapchainExtensionName };

void Application::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

// function to create a GLFW window
void Application::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // force no opengl
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // handling resized windows takes special care, disable.
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}
// initializes vulkan instance, and etc.
void Application::initVulkan() {
    createInstance();
    pickPhysicalDevice();
    if (enableValidationLayers) {
        std::cout << "Selected Physical Device: " << selectedPhysicalDevice.getProperties().deviceName << "\n";
    }
}

// Simple physical device scoring system
int Application::scoreDevice(vk::raii::PhysicalDevice &pd) {
    vk::PhysicalDeviceProperties deviceProperties = pd.getProperties();
    uint32_t score = 0;
    
    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000;
    }
    score += deviceProperties.limits.maxImageDimension2D;
    return score;
}

// checks if a physical device is suitable
bool Application::isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice) {
    // check if supports vulkan1.3
    bool supportsVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

    // get all of the device supported queue families:
    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const& qfp) {return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics); });
    
    // todo: continue by checking 
    auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
    bool supportsRequiredExtensions = std::ranges::all_of(requiredDeviceExtensions, 
        [&availableDeviceExtensions](auto const& requiredDeviceExtension){
            return std::ranges::any_of(availableDeviceExtensions,
                [requiredDeviceExtension](auto const & availableDeviceExtension){
                    return strcmp(requiredDeviceExtension, availableDeviceExtension.extensionName) == 0;
                }
            );
        }
    );

    // check if the device supports the required features:
    return supportsVulkan1_3 && supportsGraphics && supportsRequiredExtensions;
}

void Application::pickPhysicalDevice() {
    std::vector<vk::raii::PhysicalDevice> physicalDevices = vk::raii::PhysicalDevices(instance);

    // If no device with vulkan support, throw in the towel
    //if (physicalDevices.empty())
    //{
    //    throw std::runtime_error("failed to find GPUs with Vulkan support!");
    //}
       
    
    // score devices
    // using ordered multimap (ascending order)
    //std::multimap<int, vk::raii::PhysicalDevice> pdCandidates;
    for (auto& pd : physicalDevices) {
    //    vk::PhysicalDeviceProperties deviceProperties = pd.getProperties();
    //    vk::PhysicalDeviceFeatures deviceFeatures = pd.getFeatures();
    //    uint32_t score = 0;
    //
    //    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
    //        score += 1000;
    //    }
    //    score += deviceProperties.limits.maxImageDimension2D;
    //    // if no geometry shader support, reject
    //    if (!deviceFeatures.geometryShader) {
    //        continue;
    //    }
    //    // reject device if it doesn't at least support Vk 13
    //    if (deviceProperties.apiVersion < vk::ApiVersion13) {
    //        continue;
    //    }
    //    pdCandidates.insert(std::make_pair(score, pd));
    //}
    //if (!pdCandidates.empty() && pdCandidates.rbegin()->first > 0) {
    //    selectedPhysicalDevice = pdCandidates.rbegin()->second;
    //}
    //else {
    //    throw std::runtime_error("Failed to find a suitable GPU!");
    }
}
// runs the main vulkan loop
void Application::mainLoop() {
    while (!glfwWindowShouldClose(window)) { // checks if window should close, if not, then stay open
        glfwPollEvents(); // process input events
    }
}
// cleans up vulkan resources
void Application::cleanup() {
    glfwDestroyWindow(window); // destroys current window

    glfwTerminate(); // terminates glfw
}

// function to create a vulkan instance
void Application::createInstance() {
    // get the required validation layers
    std::vector<const char*> requiredLayers;

    if (enableValidationLayers) {
        requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }

    // Check if the required extensions are supported by the Vulkan implementation.
    auto layerProperties = context.enumerateInstanceLayerProperties();
    // only grabs the first unsupported layer. find_if returns an iterator to the first one.
    auto unsupportedPropertyIt = std::ranges::find_if(requiredLayers, [&layerProperties](auto const& requiredLayer) {
        return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty) {return strcmp(requiredLayer, layerProperty.layerName) == 0; });
        }
    );
    if (unsupportedPropertyIt != requiredLayers.end()) {
        throw std::runtime_error("Required Extension not supported: " + std::string(*unsupportedPropertyIt));
    }


    // create general application info for the instance
    constexpr vk::ApplicationInfo appInfo = {
        .pApplicationName = "OJApplication",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "OJEngine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = vk::ApiVersion14,
    };

    // Get correct number of extensions required to use glfw:
    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // check if the extensions are supported by our vulkan instance:
    auto extensionProperties = context.enumerateInstanceExtensionProperties();
    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        if (std::ranges::none_of(extensionProperties, [glfwExtension = glfwExtensions[i]](auto const& extensionProperty)
            {return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }))
        {
            throw std::runtime_error("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
        }

    }
    // global extensions and validation layers used
    // finish instance creation
    // TODO: ensure layers are added, with layercount and layer names.
    // TODO: read up on layers, and extensions
    vk::InstanceCreateInfo createInfo{
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = glfwExtensionCount,
        .ppEnabledExtensionNames = glfwExtensions,
    };

    // create instance
    instance = vk::raii::Instance(context, createInfo);
}

}
