#pragma once

#include <functional>
#include <string>

#include "imgui.h"
#include "../command/command_ids.h"
#include "../services/editor_tab.h"

enum class TitleBarExtensionRegion {
    Left,
    Center,
    Right
};

struct TitleBarExtensionContext {
    TitleBarExtensionRegion region = TitleBarExtensionRegion::Center;
    ImVec2 region_min = ImVec2(0.0f, 0.0f);
    ImVec2 region_max = ImVec2(0.0f, 0.0f);
    float title_h = 0.0f;
    EditorTab* active_tab = nullptr;
    std::function<void(CommandId)> trigger_command;
    std::function<void(const std::string& action_id)> trigger_scene_action;
};

using SceneTitleBarExtensionRenderer = std::function<void(const TitleBarExtensionContext& ctx)>;
