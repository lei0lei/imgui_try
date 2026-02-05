#include "view_registry_defaults.h"
#include "view_registry_defaults_config.h"

namespace UI {

void RegisterDefaultViews(IViewRegistry& registry)
{
    for (const auto& entry : GetDefaultViewConfigs()) {
        registry.RegisterView(entry.mode, entry.container, { entry.id, entry.title, entry.renderer });
    }
}

} // namespace UI