#pragma once

#include "../services/view_registry.h"
#include "../core/workbench_types.h"

namespace UI {

void RegisterDefaultViews(IViewRegistry& registry);

ViewDefinition GetDefaultPrimaryView(ActivityBarItem item);

} // namespace UI