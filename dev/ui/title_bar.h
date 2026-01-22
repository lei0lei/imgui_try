#pragma once

#include <SDL3/SDL.h>

enum class MenuAction {
    None = 0,
    FileNew,
    FileNew2D,
    FileNew3D,
    FileOpen,
    FileSave,
    FileExit,
    EditUndo,
    EditRedo,
    ViewExplorer,
    ViewConsole,
    HelpAbout
};

struct TitleBarResult
{
    bool minimize = false;
    bool maximize = false;
    bool close = false;
    bool toggle_primary_sidebar = false;    // 切换左侧主面板
    bool toggle_panel = false;               // 切换底部面板
    bool toggle_secondary_sidebar = false;   // 切换右侧次要面板
    MenuAction menu_action = MenuAction::None;
};

// Draws VS Code style title bar with icon, menu bar, and window controls
TitleBarResult DrawTitleBar(SDL_Window* window, float title_h = 30.0f, 
                            bool primary_sidebar_visible = true, 
                            bool panel_visible = true, 
                            bool secondary_sidebar_visible = true);
