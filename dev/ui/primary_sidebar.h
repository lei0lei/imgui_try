#pragma once

#include "activity_bar.h"

struct PrimarySidebarResult
{
    bool is_visible = true;
};

// Draws VS Code style primary sidebar (file explorer, search results, etc.)
PrimarySidebarResult DrawPrimarySidebar(ActivityBarItem active_item, 
                                        float activity_bar_w, 
                                        float title_h, 
                                        float status_bar_h, 
                                        float width = 300.0f);
