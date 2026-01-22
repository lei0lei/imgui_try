#pragma once

struct SecondarySidebarResult
{
    bool request_close = false; // true when user clicks the close button
};

// Draws VS Code style secondary sidebar (outline, timeline, etc.)
SecondarySidebarResult DrawSecondarySidebar(float title_h, 
                                            float status_bar_h, 
                                            float panel_h,
                                            float width = 250.0f);
