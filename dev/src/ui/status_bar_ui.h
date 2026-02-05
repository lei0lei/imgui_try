#pragma once
#include <SDL3/SDL.h>
#include <string>

struct StatusBarViewModel {
	std::string message;
	float progress = -1.0f;
};

// 只负责渲染和交互收集，不直接依赖 service
void RenderStatusBarUI(SDL_Window* window, const StatusBarViewModel& view_model, float status_bar_h, float title_h);
