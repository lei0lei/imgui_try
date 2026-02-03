
#pragma once
#include <SDL3/SDL.h>
#include "../services/primary_sidebar_service.h"
#include "activity_bar_ui.h"

// 直接定义 PrimarySidebarResult
struct PrimarySidebarResult {
    bool is_visible = true;
};

PrimarySidebarResult DrawPrimarySidebarUI(
    ActivityBarItem active_item,
    float activity_bar_w,
    float title_h,
    float status_bar_h,
    float width,
    PrimarySidebarService& service);
