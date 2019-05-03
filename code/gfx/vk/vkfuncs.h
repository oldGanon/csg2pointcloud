#define VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"

/* 1.0 */
#define VK_FUNC(Name) static PFN_##Name Name
VK_FUNC(vkGetInstanceProcAddr);
VK_FUNC(vkCreateInstance);
VK_FUNC(vkEnumerateInstanceExtensionProperties);
VK_FUNC(vkEnumerateInstanceLayerProperties);
VK_FUNC(vkDestroyInstance);
VK_FUNC(vkEnumeratePhysicalDevices);
VK_FUNC(vkGetPhysicalDeviceProperties);
VK_FUNC(vkGetPhysicalDeviceFeatures);
VK_FUNC(vkGetPhysicalDeviceQueueFamilyProperties);
VK_FUNC(vkCreateDevice);
VK_FUNC(vkDestroyDevice);
VK_FUNC(vkGetDeviceQueue);
VK_FUNC(vkDestroySurfaceKHR);
VK_FUNC(vkGetPhysicalDeviceSurfaceSupportKHR);
VK_FUNC(vkEnumerateDeviceExtensionProperties);
VK_FUNC(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
VK_FUNC(vkGetPhysicalDeviceSurfaceFormatsKHR);
VK_FUNC(vkGetPhysicalDeviceSurfacePresentModesKHR);
VK_FUNC(vkCreateSwapchainKHR);
VK_FUNC(vkDestroySwapchainKHR);
VK_FUNC(vkGetSwapchainImagesKHR);
VK_FUNC(vkCreateImageView);
VK_FUNC(vkDestroyImageView);

#if COMPILE_DEV
VK_FUNC(vkCreateDebugReportCallbackEXT);
VK_FUNC(vkDestroyDebugReportCallbackEXT);
#endif

#define VK_LOAD_AND_CHECK(I, Name) \
	Name = (PFN_##Name)vkGetInstanceProcAddr(I, #Name); if(!Name) return false;
static b32
VK_LoadFunctions()
{
	VK_LOAD_AND_CHECK(0, vkCreateInstance);
	VK_LOAD_AND_CHECK(0, vkEnumerateInstanceExtensionProperties);
	VK_LOAD_AND_CHECK(0, vkEnumerateInstanceLayerProperties);

	return true;
}

static b32
VK_LoadFunctions(VkInstance Instance)
{
	VK_LOAD_AND_CHECK(Instance, vkDestroyInstance);
	VK_LOAD_AND_CHECK(Instance, vkEnumeratePhysicalDevices);
	VK_LOAD_AND_CHECK(Instance, vkGetPhysicalDeviceProperties);
	VK_LOAD_AND_CHECK(Instance, vkGetPhysicalDeviceFeatures);
	VK_LOAD_AND_CHECK(Instance, vkGetPhysicalDeviceQueueFamilyProperties);
	VK_LOAD_AND_CHECK(Instance, vkCreateDevice);
	VK_LOAD_AND_CHECK(Instance, vkDestroyDevice);
	VK_LOAD_AND_CHECK(Instance, vkGetDeviceQueue);
	VK_LOAD_AND_CHECK(Instance, vkDestroySurfaceKHR);
	VK_LOAD_AND_CHECK(Instance, vkGetPhysicalDeviceSurfaceSupportKHR);
	VK_LOAD_AND_CHECK(Instance, vkEnumerateDeviceExtensionProperties);
	VK_LOAD_AND_CHECK(Instance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
	VK_LOAD_AND_CHECK(Instance, vkGetPhysicalDeviceSurfaceFormatsKHR);
	VK_LOAD_AND_CHECK(Instance, vkGetPhysicalDeviceSurfacePresentModesKHR);
	VK_LOAD_AND_CHECK(Instance, vkCreateSwapchainKHR);
	VK_LOAD_AND_CHECK(Instance, vkDestroySwapchainKHR);
	VK_LOAD_AND_CHECK(Instance, vkGetSwapchainImagesKHR);
	VK_LOAD_AND_CHECK(Instance, vkCreateImageView);
	VK_LOAD_AND_CHECK(Instance, vkDestroyImageView);

#if COMPILE_DEV
	VK_LOAD_AND_CHECK(Instance, vkCreateDebugReportCallbackEXT);
	VK_LOAD_AND_CHECK(Instance, vkDestroyDebugReportCallbackEXT);
#endif

	return true;
}
