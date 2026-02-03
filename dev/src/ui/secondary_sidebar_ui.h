#pragma once
#include <SDL3/SDL.h>
#include "../services/secondary_sidebar_service.h"

struct SecondarySidebarResult
{
    bool request_close = false; // true when user clicks the close button
};

// Draws the secondary sidebar using service state
SecondarySidebarResult DrawSecondarySidebarUI(float title_h,
                                              float status_bar_h,
                                              float panel_h,
                                              float width,
                                              SecondarySidebarService& service);
