#pragma once
#include <SDL3/SDL.h>
#include <functional>
#include "../services/view_registry.h"
#include "view_registry_defaults.h"

struct SecondarySidebarResult
{
    bool request_close = false; // true when user clicks the close button
};

struct SecondarySidebarViewModel {
    bool is_visible = false;
    bool has_active_tab = false;
    const ViewDefinition* active_view = nullptr;
    const std::vector<ViewDefinition>* views = nullptr;
    int active_index = 0;
    bool show_tabs = false;
    std::function<void(int)> on_select_tab;
};

// Draws the secondary sidebar using service state
SecondarySidebarResult DrawSecondarySidebarUI(float title_h,
                                              float status_bar_h,
                                              float panel_h,
                                              float width,
                                              const SecondarySidebarViewModel& view_model,
                                              EditorTab* active_tab);
