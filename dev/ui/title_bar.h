#pragma once

#include <SDL3/SDL.h>

struct TitleBarResult
{
    bool minimize = false;
    bool maximize = false;
    bool close = false;
};

// Draws VS Code style title bar with icon, menu bar, and window controls
TitleBarResult DrawTitleBar(SDL_Window* window, float title_h = 30.0f);
