#pragma once

#include "../core/workbench_theme.h"
#include "../services/event_bus.h"

namespace Scenes {

struct SceneContext {
    ImVec2 content_min;
    ImVec2 content_max;
    const WorkbenchTheme& theme;
    const WorkbenchMetrics& metrics;
    IEventBus& event_bus;
};

} // namespace Scenes
