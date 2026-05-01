#include "scene_extension_resolver.h"

#include "../scenes/scene_plugin_registry.h"
#include "../services/editor_tab.h"

namespace WorkbenchSceneExtensionResolver {

TitleBarSceneExtensions ResolveTitleBar(EditorTab* active_tab, float title_h)
{
    TitleBarSceneExtensions out{};
    if (!active_tab || active_tab->scene_plugin_id.empty()) {
        return out;
    }

    Scenes::ScenePluginRegistry::Instance().EnsureLoaded();
    out.extension = Scenes::ScenePluginRegistry::Instance().GetTitleBarExtension(active_tab->scene_plugin_id);
    out.action_handler = Scenes::ScenePluginRegistry::Instance().GetTitleBarActionHandler(active_tab->scene_plugin_id);
    if (const auto* resolver = Scenes::ScenePluginRegistry::Instance().GetTitleBarExtensionWidthResolver(active_tab->scene_plugin_id)) {
        out.extension_width = (*resolver)(title_h, active_tab);
        if (out.extension_width < 0.0f) {
            out.extension_width = 0.0f;
        }
    }
    return out;
}

StatusBarSceneExtensions ResolveStatusBar(EditorTab* active_tab)
{
    StatusBarSceneExtensions out{};
    if (!active_tab || active_tab->scene_plugin_id.empty()) {
        return out;
    }

    Scenes::ScenePluginRegistry::Instance().EnsureLoaded();
    out.extension = Scenes::ScenePluginRegistry::Instance().GetStatusBarExtension(active_tab->scene_plugin_id);
    out.action_handler = Scenes::ScenePluginRegistry::Instance().GetTitleBarActionHandler(active_tab->scene_plugin_id);
    return out;
}

} // namespace WorkbenchSceneExtensionResolver

