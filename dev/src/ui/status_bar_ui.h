#pragma once
#include <SDL3/SDL.h>
#include <string>

#include "../workbench/status_bar_extension.h"

struct StatusBarViewModel {
	std::string message;
	float progress = -1.0f;
	EditorTab* active_tab = nullptr;
	const SceneStatusBarExtensionRenderer* scene_status_bar_extension = nullptr;
	const std::function<void(EditorTab&, const std::string&)>* scene_status_bar_action_handler = nullptr;
};

// 只负责渲染和交互收集，不直接依赖 service
void RenderStatusBarUI(SDL_Window* window, const StatusBarViewModel& view_model, float status_bar_h, float title_h);
