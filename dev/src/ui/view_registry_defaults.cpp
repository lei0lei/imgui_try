/**
 * @file view_registry_defaults.cpp
 * @brief 默认视图注册表设置和配置
 * @author Your Name
 * @date 2026-02-05
 */

#include "view_registry_defaults.h"
#include "view_registry_defaults_config.h"

namespace UI {

void RegisterDefaultViews(IViewRegistry& registry)
{
    for (const auto& entry : GetDefaultViewConfigs()) {
        registry.RegisterView(entry.plugin_id, { entry.id, entry.title, entry.renderer });
    }
}

} // namespace UI