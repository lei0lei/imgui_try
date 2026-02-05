
#pragma once
#include <SDL3/SDL.h>
#include <optional>
#include "../services/view_registry.h"
#include "../core/workbench_types.h"
#include "view_registry_defaults.h"

// 直接定义 PrimarySidebarResult
struct PrimarySidebarResult {
    bool is_visible = true;
};

struct PrimarySidebarViewModel {
    const ViewDefinition* active_view = nullptr;
    std::optional<ViewDefinition> fallback_view;
    bool is_visible = true;
    ActivityBarItem active_item = ActivityBarItem::None;
};

PrimarySidebarResult DrawPrimarySidebarUI(
    float activity_bar_w,
    float title_h,
    float status_bar_h,
    float width,
    const PrimarySidebarViewModel& view_model,
    EditorTab* active_tab);
