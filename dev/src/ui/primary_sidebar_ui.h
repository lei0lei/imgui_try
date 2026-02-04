
#pragma once
#include <SDL3/SDL.h>
#include "../services/primary_sidebar_service.h"
#include "../services/view_registry.h"
#include "activity_bar_ui.h"
#include "view_registry_defaults.h"

// 直接定义 PrimarySidebarResult
struct PrimarySidebarResult {
    bool is_visible = true;
};

PrimarySidebarResult DrawPrimarySidebarUI(
    float activity_bar_w,
    float title_h,
    float status_bar_h,
    float width,
    PrimarySidebarService& service,
    ViewRegistry& view_registry,
    SceneType mode,
    EditorTab* active_tab,
    ActivityBarItem active_item);
