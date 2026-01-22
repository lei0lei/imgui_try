#pragma once

struct WindowBorderConfig {
    float title_h = 30.0f;
    float status_bar_h = 24.0f;
};

// Draws VS Code style window borders (left and right sides)
// Top and bottom borders are drawn by title_bar and status_bar
void DrawWindowBorders(const WindowBorderConfig& config = {});
