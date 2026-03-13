/**
 * @file layout_manager.cpp
 * @brief 布局管理器，用于计算工作台组件位置
 * @author Your Name
 * @date 2026-02-05
 */

 #include "layout_manager.h"

LayoutInfo LayoutManager::Calculate(const WorkbenchMetrics& metrics, const LayoutState& state) const
 {
	 LayoutInfo info;
	info.left_offset = metrics.activity_bar_w + (state.primary_sidebar_visible ? metrics.primary_sidebar_w : 0.0f);
	info.right_offset = 0.0f;
	info.secondary_panel_h = 0.0f;
	 return info;
 }
