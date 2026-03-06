
#pragma once
#include <SDL3/SDL.h>
#include <functional>
#include "imgui.h"

typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkDevice_T* VkDevice;
typedef struct VkQueue_T* VkQueue;
typedef struct VkAllocationCallbacks VkAllocationCallbacks;

#include "../core/workbench_types.h"

struct ActivityBarResult {
	ActivityBarItem selected_item = ActivityBarItem::None;
	bool item_clicked = false;
};

struct ActivityBarViewModel {
    int selected_index = 0; // 1-based index matching ActivityBarItem enum order
    std::function<void(int)> on_select;
};

void SetActivityBarIconTexture(ActivityBarItem item, ImTextureID texture_id);
void SetActivityBarSettingsIconTexture(ImTextureID texture_id);
void ConfigureActivityBarIcons();
void ClearActivityBarIcons();

bool InitializeActivityBarIconSystem(
	VkPhysicalDevice physical_device,
	VkDevice device,
	uint32_t queue_family,
	VkQueue queue,
	VkAllocationCallbacks* allocator
);
void ShutdownActivityBarIconSystem();

ActivityBarResult DrawActivityBarUI(float title_h, float status_bar_h, float width, const ActivityBarViewModel& view_model);
