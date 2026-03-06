#pragma once

#include "imgui.h"

#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif
#endif

#include <vulkan/vulkan.h>

struct IconTextureManagerInitInfo {
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t queue_family = (uint32_t)-1;
    VkQueue queue = VK_NULL_HANDLE;
    VkAllocationCallbacks* allocator = nullptr;
    const char* base_path = nullptr;
};

bool IconTextureManagerInitialize(const IconTextureManagerInitInfo& init_info);
void IconTextureManagerShutdown();

ImTextureID IconTextureManagerLoadPng(const char* key, const char* relative_icon_path);
ImTextureID IconTextureManagerGet(const char* key);
void IconTextureManagerUnload(const char* key);
