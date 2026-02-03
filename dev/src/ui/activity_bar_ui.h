
#pragma once
#include <SDL3/SDL.h>
#include "../services/activity_bar_service.h"

// 直接定义 ActivityBarItem 和 ActivityBarResult
enum class ActivityBarItem {
	None = 0,
	Explorer,      // 文件资源管理器
	Search,        // 搜索
	NodeEditor, // 源代码管理
	Debug,         // 调试
	Extensions     // 扩展
};

struct ActivityBarResult {
	ActivityBarItem selected_item = ActivityBarItem::None;
	bool item_clicked = false;
};

ActivityBarResult DrawActivityBarUI(float title_h, float status_bar_h, float width, ActivityBarService& service);
