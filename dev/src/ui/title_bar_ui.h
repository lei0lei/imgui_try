#pragma once
#include <SDL3/SDL.h>
#include <functional>
#include "../core/workbench_types.h"
#include "../command/command_ids.h"

struct TitleBarViewModel {
    TitleBarMenu active_menu = TitleBarMenu::None;
    std::function<void(TitleBarMenu)> set_active_menu;
    std::function<void(CommandId)> trigger_command;
    std::function<void()> request_block_tab_clicks_once;
    bool primary_sidebar_visible = true;
    bool panel_visible = true;
    bool secondary_sidebar_visible = true;
};

// 只负责渲染和交互收集，不直接依赖 service
void RenderTitleBarUI(SDL_Window* window, const TitleBarViewModel& view_model, float title_h);
