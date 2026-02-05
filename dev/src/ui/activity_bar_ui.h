
#pragma once
#include <SDL3/SDL.h>
#include <functional>
#include "../core/workbench_types.h"

struct ActivityBarResult {
	ActivityBarItem selected_item = ActivityBarItem::None;
	bool item_clicked = false;
};

struct ActivityBarViewModel {
    int selected_index = 0; // 1-based index matching ActivityBarItem enum order
    std::function<void(int)> on_select;
};

ActivityBarResult DrawActivityBarUI(float title_h, float status_bar_h, float width, const ActivityBarViewModel& view_model);
