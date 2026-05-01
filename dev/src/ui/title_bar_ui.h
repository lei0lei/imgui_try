#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>

#include "../core/workbench_types.h"
#include "../command/command_ids.h"
#include "../services/editor_tab.h"
#include "../core/title_bar_extension.h"

struct TitleBarProps {
    TitleBarMenu active_menu = TitleBarMenu::None;
    bool primary_sidebar_visible = true;
    EditorTab* active_tab = nullptr;
    const SceneTitleBarExtensionRenderer* scene_titlebar_extension = nullptr;
    float scene_titlebar_extension_width = 0.0f;
};

struct TitleBarResult {
    TitleBarMenu menu = TitleBarMenu::None;
    std::vector<CommandId> commands;
    std::vector<std::string> scene_actions;
    bool block_tab_clicks_once = false;
};

TitleBarResult RenderTitleBarUI(SDL_Window* window, const TitleBarProps& props, float title_h);
