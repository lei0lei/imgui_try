// ui/panel_ui.h
// 面板UI声明，负责渲染和交互
#pragma once

#include <imgui.h>
#include "../services/panel_service.h"
#include "../services/view_registry.h"

struct PanelResult {
    // 可根据需要扩展
};

PanelResult DrawPanelUI(float left_offset,
                        float right_offset,
                        float status_bar_h,
                        float panel_h,
                        PanelService& panel_service,
                        ViewRegistry& view_registry,
                        SceneType mode,
                        EditorTab* active_tab);
