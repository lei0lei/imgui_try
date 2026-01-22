#pragma once

struct StatusBarResult
{
    // Placeholder for future status bar actions
};

// Draws VS Code style status bar at bottom with FPS counter
// Automatically draws all window borders (top/bottom/left/right)
StatusBarResult DrawStatusBar(float status_bar_h = 24.0f, float title_h = 30.0f);
