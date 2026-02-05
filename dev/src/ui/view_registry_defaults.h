#pragma once

#include "../services/view_registry.h"
#include "../core/workbench_types.h"

namespace UI {

void RegisterDefaultViews(IViewRegistry& registry);

ViewDefinition GetDefaultPrimaryView(SceneType mode, ActivityBarItem item);
ViewDefinition GetDefaultSecondaryView(SceneType mode);
ViewDefinition GetDefaultPanelView(SceneType mode);

} // namespace UI