
#pragma once
#include <SDL3/SDL.h>
#include <optional>
#include "../services/view_registry.h"
#include "view_registry_defaults.h"

struct PrimarySidebarResult {};

struct PrimarySidebarProps {
    float activity_bar_w = 0.0f;
    float title_h = 0.0f;
    float status_bar_h = 0.0f;
    float width = 0.0f;
    const ViewDefinition* active_view = nullptr;
    std::optional<ViewDefinition> fallback_view;
};

PrimarySidebarResult DrawPrimarySidebarUI(const PrimarySidebarProps& props, EditorTab* active_tab);
