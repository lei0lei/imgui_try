#pragma once

#include "../../services/layout_service.h"
#include "../workbench_config.h"

struct LayoutInfo {
	 float left_offset = 0.0f;
	 float right_offset = 0.0f;
	 float secondary_panel_h = 0.0f;
 };

class LayoutManager {
public:
	 LayoutInfo Calculate(const WorkbenchMetrics& metrics, const LayoutState& state) const;
};
