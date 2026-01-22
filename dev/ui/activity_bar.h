#pragma once

#include <SDL3/SDL.h>

enum class ActivityBarItem {
    None = 0,
    Explorer,      // 文件资源管理器
    Search,        // 搜索
    SourceControl, // 源代码管理
    Debug,         // 调试
    Extensions     // 扩展
};

struct ActivityBarResult
{
    ActivityBarItem selected_item = ActivityBarItem::None;
    bool item_clicked = false;
    bool sidebar_visible = true;  // Whether primary sidebar should be shown
};

// Draws VS Code style activity bar (leftmost narrow strip with icons)
// Returns the width of the activity bar and which item was clicked
ActivityBarResult DrawActivityBar(float title_h = 30.0f, float status_bar_h = 24.0f, float width = 48.0f);

// Toggle the primary sidebar visibility
void TogglePrimarySidebar();
