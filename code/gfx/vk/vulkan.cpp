#include "vkfuncs.h"

struct vk_state
{
    VkPhysicalDevice PhysicalDevice;
    VkDevice Device;
    VkQueue Queue;
    VkSurfaceKHR Surface;
    VkSwapchainKHR SwapChain;
    u32 SwapChainImageCount;
    VkImage *SwapChainImages;
    VkImageView *SwapChainImageViews;
    VkExtent2D SwapChainExtent;
    VkFormat SwapChainImageFormat;

#if COMPILE_DEV
    VkDebugReportCallbackEXT DebugReportCallback;
#endif
};

global vk_state VK = { };

static b32
Vulkan_CheckLayers(u32 LayerCount, const char **LayerNames)
{
    u32 AvailLayerCount;
    vkEnumerateInstanceLayerProperties(&AvailLayerCount, 0);
    VkLayerProperties *Layers = 
        (VkLayerProperties *)Api_Malloc(sizeof(VkLayerProperties) * AvailLayerCount);
    vkEnumerateInstanceLayerProperties(&AvailLayerCount, Layers);
    
    b32 FoundAllLayers = true;
    for (u32 i = 0; i < LayerCount; ++i)
    {
        b32 FoundLayer = false;
        for (u32 j = 0; j < AvailLayerCount; ++j)
        {
            if (!String_Compare(Layers[j].layerName, LayerNames[i]))
            {
                FoundLayer = true;
                break;
            }
        }
        if (!FoundLayer)
        {
            FoundAllLayers = false;
            break;
        }
    }
    
    Api_Free(Layers);
    return FoundAllLayers;
}

static VkBool32
Vulkan_DebugCallback(VkDebugReportFlagsEXT Flags, VkDebugReportObjectTypeEXT ObjType,
                     uint64_t Obj, size_t Location, int32_t Code, 
                     const char* LayerPrefix, const char* Msg, void* UserData)
{
    Api_Error(Msg);
    return VK_FALSE;
}

static i32
Vulkan_QueueFamily(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface)
{
    u32 QFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QFamilyCount, 0);

    VkQueueFamilyProperties *QFamilies = (VkQueueFamilyProperties *)
        Api_Malloc(sizeof(VkQueueFamilyProperties) * QFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QFamilyCount, QFamilies);

    i32 QueueFamily = -1;
    for (u32 i = 0; i < QFamilyCount; ++i)
    {
        VkBool32 PresentSupport;
        vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface, &PresentSupport);
        
        if (PresentSupport &&
            QFamilies[i].queueCount > 0 && 
            (QFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
        {
            QueueFamily = i;
            break;
        }
    }

    Api_Free(QFamilies);
    return QueueFamily;
}

static b32
Vulkan_CheckDeviceExtensions(VkPhysicalDevice PhysicalDevice, 
                             u32 ReqExtensionCount, const char **ReqExtensionNames)
{
    u32 ExtensionCount;
    vkEnumerateDeviceExtensionProperties(PhysicalDevice, 0, &ExtensionCount, 0);

    VkExtensionProperties *Extensions = (VkExtensionProperties *)
        Api_Malloc(sizeof(VkExtensionProperties) * ExtensionCount);
    vkEnumerateDeviceExtensionProperties(PhysicalDevice, 0, &ExtensionCount, Extensions);
    
    b32 FoundAllExtensions = true;
    for (u32 i = 0; i < ReqExtensionCount; ++i)
    {
        b32 FoundExtension = false;
        for (u32 j = 0; j < ExtensionCount; ++j)
        {
            if (!String_Compare(ReqExtensionNames[i], Extensions[j].extensionName))
            {
                FoundExtension = true;
                break;
            }
        }
        if (!FoundExtension)
        {
            FoundAllExtensions = false;
            break;
        }
    }
    
    Api_Free(Extensions);
    return FoundAllExtensions;
}

static b32
Vulkan_CheckSwapChainSupport(VkPhysicalDevice Device, VkSurfaceKHR Surface)
{
    VkSurfaceCapabilitiesKHR Capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, Surface, &Capabilities);

    u32 FormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, 0);
    if (!FormatCount)
        return false;

    u32 PresentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, 0);
    if (!PresentModeCount)
        return false;

    return true;
}

static VkPhysicalDevice
Vulkan_FindPhysicalDevice(VkInstance Instance, VkSurfaceKHR Surface, 
                          u32 ExtCount, const char **ExtNames)
{
    u32 DeviceCount = 0;
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, 0);
    if (DeviceCount == 0)
        return VK_NULL_HANDLE;

    VkPhysicalDevice *Devices = 
        (VkPhysicalDevice *)Api_Malloc(sizeof(VkPhysicalDevice) * DeviceCount);
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, Devices);

    VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
    for (u32 i = 0; i < DeviceCount; ++i)
    {
        VkPhysicalDeviceProperties Properties;
        VkPhysicalDeviceFeatures Features;

        vkGetPhysicalDeviceProperties(Devices[i], &Properties);
        vkGetPhysicalDeviceFeatures(Devices[i], &Features);
        
        if (Vulkan_QueueFamily(Devices[i], Surface) == -1) continue;
        if (!Vulkan_CheckDeviceExtensions(Devices[i], ExtCount, ExtNames)) continue;
        if (!Vulkan_CheckSwapChainSupport(Devices[i], Surface)) continue;
        
        PhysicalDevice = Devices[i];
        break;
    }

    Api_Free(Devices);
    return PhysicalDevice;
}

static b32
Vulkan_CreateSwapChain()
{
    VkSurfaceCapabilitiesKHR Capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VK.PhysicalDevice, VK.Surface, &Capabilities);

    if (Capabilities.currentExtent.width != 0xFFFFFFFF)
        VK.SwapChainExtent = Capabilities.currentExtent;
    else
    {
        VK.SwapChainExtent.width = CLAMP(1024, Capabilities.minImageExtent.width, 
                                               Capabilities.maxImageExtent.width);
        VK.SwapChainExtent.height = CLAMP(1024, Capabilities.minImageExtent.height, 
                                                Capabilities.maxImageExtent.height);
    }

    u32 ImageCount = Capabilities.minImageCount + 1;
    if (Capabilities.maxImageCount > 0 && ImageCount > Capabilities.maxImageCount)
        ImageCount = Capabilities.maxImageCount;

    u32 FormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(VK.PhysicalDevice, VK.Surface, &FormatCount, 0);
    VkSurfaceFormatKHR *Formats = 
        (VkSurfaceFormatKHR *)Api_Malloc(sizeof(VkSurfaceFormatKHR) * FormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(VK.PhysicalDevice, 
                                         VK.Surface, &FormatCount, Formats);
    VkSurfaceFormatKHR SurfaceFormat = Formats[0];
    if (FormatCount == 1 && SurfaceFormat.format == VK_FORMAT_UNDEFINED)
    {
        SurfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    }
    else
    {
        for (u32 i = 0; i < FormatCount; ++i)
        {
            if (Formats[i].format == VK_FORMAT_B8G8R8A8_UNORM && 
                Formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
            {
                SurfaceFormat = Formats[i];
                break;
            }
        }
    }
    Api_Free(Formats);

    u32 PreModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(VK.PhysicalDevice, 
                                              VK.Surface, &PreModeCount, 0);
    VkPresentModeKHR *PreModes = 
        (VkPresentModeKHR *)Api_Malloc(sizeof(VkPresentModeKHR) * PreModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(VK.PhysicalDevice, 
                                              VK.Surface, &PreModeCount, PreModes);
    VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i = 0; i < PreModeCount; ++i)
    {
        if (PreModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            PresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        else if (PreModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            PresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }
    Api_Free(PreModes);

    VkSwapchainCreateInfoKHR CreateInfo = { };
    CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    CreateInfo.surface = VK.Surface;
    CreateInfo.minImageCount = ImageCount;
    CreateInfo.imageFormat = SurfaceFormat.format;
    CreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
    CreateInfo.imageExtent = VK.SwapChainExtent;
    CreateInfo.imageArrayLayers = 1;
    CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    CreateInfo.queueFamilyIndexCount = 0;
    CreateInfo.pQueueFamilyIndices = 0;
    CreateInfo.preTransform = Capabilities.currentTransform;
    CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    CreateInfo.presentMode = PresentMode;
    CreateInfo.clipped = VK_TRUE;
    CreateInfo.oldSwapchain = VK_NULL_HANDLE;

    VK.SwapChainImageFormat = SurfaceFormat.format;
    if (vkCreateSwapchainKHR(VK.Device, &CreateInfo, 0, &VK.SwapChain) != VK_SUCCESS)
        return false;

    vkGetSwapchainImagesKHR(VK.Device, VK.SwapChain, &VK.SwapChainImageCount, 0);
    VK.SwapChainImages =  (VkImage *)Api_Malloc(sizeof(VkImage) * VK.SwapChainImageCount);
    vkGetSwapchainImagesKHR(VK.Device, VK.SwapChain, &VK.SwapChainImageCount, 
                                                      VK.SwapChainImages);

    VK.SwapChainImageViews = 
        (VkImageView *)Api_Malloc(sizeof(VkImageView) * VK.SwapChainImageCount);
    for (u32 i = 0; i < VK.SwapChainImageCount; ++i)
    {
        VkImageViewCreateInfo ImageViewCreateInfo = { };
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewCreateInfo.image = VK.SwapChainImages[i];
        ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ImageViewCreateInfo.format = VK.SwapChainImageFormat;
        ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ImageViewCreateInfo.subresourceRange.levelCount = 1;
        ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ImageViewCreateInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(VK.Device, &ImageViewCreateInfo, 0, 
                             &VK.SwapChainImageViews[i]) != VK_SUCCESS)
            return false;
    }

    return true;
}

static b32
Vulkan_Instance(VkInstance *Instance, u32 ExtCount, const char **ExtNames)
{
    if (!VK_LoadFunctions())
        return false;

    VkApplicationInfo AppInfo = { };
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pApplicationName = GAME_NAME;
    AppInfo.applicationVersion = GAME_VERSION;
    AppInfo.pEngineName = ENGINE_NAME;
    AppInfo.engineVersion = ENGINE_VERSION;
    AppInfo.apiVersion = VK_API_VERSION_1_0;

#if COMPILE_DEV
    const char *LayerNames[] = { "VK_LAYER_LUNARG_standard_validation" };
    const u32 LayerCount = sizeof(LayerNames) / sizeof(const char *);
    if (!Vulkan_CheckLayers(LayerCount, LayerNames))
        return 1;
#else
    u32 LayerCount = 0;
    const char** LayerNames = 0;
#endif

    VkInstanceCreateInfo InstanceCreateInfo = { };
    InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceCreateInfo.pApplicationInfo = &AppInfo;
    InstanceCreateInfo.enabledLayerCount = LayerCount;
    InstanceCreateInfo.ppEnabledLayerNames = LayerNames;
    InstanceCreateInfo.enabledExtensionCount = ExtCount;
    InstanceCreateInfo.ppEnabledExtensionNames = ExtNames;

    if (vkCreateInstance(&InstanceCreateInfo, 0, Instance) != VK_SUCCESS)
        return false;
    
    if (!VK_LoadFunctions(*Instance))
        return false;

#if COMPILE_DEV
    VkDebugReportCallbackCreateInfoEXT DebugCreateInfo = { };
    DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    DebugCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    DebugCreateInfo.pfnCallback = Vulkan_DebugCallback;
    
    if (vkCreateDebugReportCallbackEXT(*Instance, &DebugCreateInfo, 
                                       0, &VK.DebugReportCallback))
        return false;
#endif

    return true;
}

static b32
Vulkan_Init(VkInstance Instance, VkSurfaceKHR Surface)
{
    VK.Surface = Surface;

    const char* ExtNames[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const u32 ExtCount = sizeof(ExtNames) / sizeof(const char *);

    // PHYSICAL DEVICE
    VK.PhysicalDevice = Vulkan_FindPhysicalDevice(Instance, Surface, ExtCount, ExtNames);
    if (VK.PhysicalDevice == VK_NULL_HANDLE)
        return false;

    // QUEUES
    const u32 QueueFamilyCount = 1;
    i32 QueueFamilyIndices[QueueFamilyCount] = 
    {
        Vulkan_QueueFamily(VK.PhysicalDevice, Surface)
    };
    
    VkDeviceQueueCreateInfo QueueCreateInfos[QueueFamilyCount] = { };
    for (u32 i = 0; i < QueueFamilyCount; ++i)
    {
        const u32 QueueCount = 1;
        f32 QueuePriorities[QueueCount] = { 1.0f };
        QueueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueueCreateInfos[i].queueFamilyIndex = QueueFamilyIndices[i];
        QueueCreateInfos[i].queueCount = QueueCount;
        QueueCreateInfos[i].pQueuePriorities = QueuePriorities;
    }

    // DEVICE
    VkPhysicalDeviceFeatures DeviceFeatures = { };
    VkDeviceCreateInfo DeviceCreateInfo = { };
    DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos;
    DeviceCreateInfo.queueCreateInfoCount = QueueFamilyCount;
    DeviceCreateInfo.pEnabledFeatures = &DeviceFeatures;
    DeviceCreateInfo.enabledExtensionCount = ExtCount;
    DeviceCreateInfo.ppEnabledExtensionNames = ExtNames;

    if (vkCreateDevice(VK.PhysicalDevice, &DeviceCreateInfo, 0, &VK.Device) != VK_SUCCESS)
        return false;

    vkGetDeviceQueue(VK.Device, QueueFamilyIndices[0], 0, &VK.Queue);

    // SWAP CHAIN
    if (!Vulkan_CreateSwapChain())
        return false;

    return true;
}

static void
Vulkan_Destroy(VkInstance Instance)
{
    for (u32 i = 0; i < VK.SwapChainImageCount; ++i)
        vkDestroyImageView(VK.Device, VK.SwapChainImageViews[i], 0);
    vkDestroySwapchainKHR(VK.Device, VK.SwapChain, 0);
    vkDestroyDevice(VK.Device, 0);
#if COMPILE_DEV
    vkDestroyDebugReportCallbackEXT(Instance, VK.DebugReportCallback, 0);
#endif
    vkDestroySurfaceKHR(Instance, VK.Surface, 0);
    vkDestroyInstance(Instance, 0);
}
