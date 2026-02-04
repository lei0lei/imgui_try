#pragma once

#include "../services/view_registry.h"
#include "activity_bar_ui.h"

namespace UI {

void RegisterDefaultViews(ViewRegistry& registry);

ViewDefinition GetDefaultPrimaryView(ActivityBarItem item);

} // namespace UI