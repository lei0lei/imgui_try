#pragma once

#include "../workbench/workbench_config.h"

namespace Scenes {

struct SceneContext {
    ImVec2 content_min;
    ImVec2 content_max;
    const WorkbenchTheme& theme;
    const WorkbenchMetrics& metrics;
};

} // namespace Scenes
