#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>

#include "../workbench/status_bar_extension.h"

struct StatusBarProps {
    std::string message;
    float progress = -1.0f;
    EditorTab* active_tab = nullptr;
    const SceneStatusBarExtensionRenderer* scene_status_bar_extension = nullptr;
};

struct StatusBarResult {
    std::vector<std::string> scene_actions;
};

StatusBarResult RenderStatusBarUI(SDL_Window* window, const StatusBarProps& props, float status_bar_h, float title_h);
