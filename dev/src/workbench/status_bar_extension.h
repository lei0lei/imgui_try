#pragma once

#include <functional>
#include <string>

#include "imgui.h"
#include "../services/editor_tab.h"

enum class StatusBarExtensionRegion {
    Left,
    Right
};

struct StatusBarExtensionContext {
    StatusBarExtensionRegion region = StatusBarExtensionRegion::Right;
    ImVec2 region_min = ImVec2(0.0f, 0.0f);
    ImVec2 region_max = ImVec2(0.0f, 0.0f);
    float status_bar_h = 0.0f;
    EditorTab* active_tab = nullptr;
    std::function<void(const std::string& action_id)> trigger_scene_action;
};

using SceneStatusBarExtensionRenderer = std::function<void(const StatusBarExtensionContext& ctx)>;
