#pragma once

#include <vector>
#include "../services/view_registry.h"
#include "../core/workbench_types.h"

namespace UI {

struct DefaultViewConfig {
    SceneType mode;
    ViewContainer container;
    const char* id;
    const char* title;
    ViewRenderer renderer;
};

const std::vector<DefaultViewConfig>& GetDefaultViewConfigs();
ViewDefinition GetDefaultPrimaryView(ActivityBarItem item);

} // namespace UI
