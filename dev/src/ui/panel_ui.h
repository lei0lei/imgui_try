// ui/panel_ui.h
// 面板UI声明，负责渲染和交互
#pragma once

#include <imgui.h>
#include <functional>
#include "../services/view_registry.h"

struct PanelResult {
    // 可根据需要扩展
};

struct PanelViewModel {
    const std::vector<ViewDefinition>* views = nullptr;
    int active_index = 0;
    std::function<void(int)> on_select_tab;
    const ViewDefinition* active_view = nullptr;
    bool has_active_tab = false;
    bool show_tabs = true;
};

PanelResult DrawPanelUI(float left_offset,
                        float right_offset,
                        float status_bar_h,
                        float panel_h,
                        const PanelViewModel& view_model,
                        EditorTab* active_tab);
