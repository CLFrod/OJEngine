#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <ranges>
#include <algorithm>
#include <string>
#include <map>
#include <vulkan/vulkan_raii.hpp>


class OJApplication {
public:
    void run();

private:
    GLFWwindow* window = nullptr; // window obj
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::PhysicalDevice selectedPhysicalDevice = nullptr;
    std::vector<vk::raii::PhysicalDevice> physicalDeviceOptions;

    void initWindow();

    void initVulkan();


};
