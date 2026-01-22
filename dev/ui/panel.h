#pragma once

struct PanelResult
{
    bool is_visible = false;
    float height = 200.0f;
};

// Draws VS Code style bottom panel (terminal, output, problems, etc.)
PanelResult DrawPanel(float left_offset, float right_offset, float status_bar_h, float height = 200.0f);
