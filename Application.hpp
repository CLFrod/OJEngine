#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <ranges>
#include <algorithm>
#include <string>
#include <map>
#include <vulkan/vulkan_raii.hpp>


namespace OJEngine{

class Application {
public:
	void run();

private:
	GLFWwindow* window = nullptr; // window obj
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
	vk::raii::PhysicalDevice selectedPhysicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue graphicsQueue = nullptr;

	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();
	void createInstance();
	void createSurface();
	
	// Physical Device Related Functions
	void pickPhysicalDevice();
	bool isDeviceSuitable(const vk::raii::PhysicalDevice & physicalDevice);
	int scoreDevice(vk::raii::PhysicalDevice &pd);

	// Logical Device Creation:
	void createLogicalDevice();
};
}