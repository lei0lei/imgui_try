#pragma once
#include <SDL3/SDL.h>
#include "../services/secondary_sidebar_service.h"
#include "../services/view_registry.h"
#include "view_registry_defaults.h"

struct SecondarySidebarResult
{
    bool request_close = false; // true when user clicks the close button
};

// Draws the secondary sidebar using service state
SecondarySidebarResult DrawSecondarySidebarUI(float title_h,
                                              float status_bar_h,
                                              float panel_h,
                                              float width,
                                              SecondarySidebarService& service,
                                              ViewRegistry& view_registry,
                                              SceneType mode,
                                              EditorTab* active_tab);
