/**
 * @file workbench_renderer.cpp
 * @brief 工作台UI布局的主渲染器
 * @author Your Name
 * @date 2026-02-05
 */

#include "workbench_renderer.h"
#include "scene_extension_resolver.h"
#include "../ui/view_registry_defaults.h"
#include "../ui/view_registry_defaults_config.h"
#include "../scenes/scene_plugin_registry.h"
#include "../workbench/workbench_config.h"
#include "imgui.h"

#include <string>

namespace {
const char* PrimarySidebarViewIdForItem(ActivityBarItem item)
{
    switch (item) {
        case ActivityBarItem::Explorer: return "explorer";
        case ActivityBarItem::Search: return "search";
        case ActivityBarItem::Debug: return "debug";
        case ActivityBarItem::Editor: return "editor";
        case ActivityBarItem::Extensions: return "extensions";
        default: return nullptr;
    }
}

std::string ResolveScenePluginId(EditorTab* active_tab)
{
    if (active_tab && !active_tab->scene_plugin_id.empty()) {
        return active_tab->scene_plugin_id;
    }
    const std::string fallback = UI::GetDefaultScenePluginId();
    return fallback.empty() ? std::string("__default_scene__") : fallback;
}
}

WorkbenchRenderer::WorkbenchRenderer(ServiceCollection& services,
                                     TitleBarPart& title_bar_part,
                                     StatusBarPart& status_bar_part,
                                     ActivityBarPart& activity_bar_part,
                                     PrimarySidebarPart& primary_sidebar_part,
                                     EditorAreaPart& editor_area_part,
                                     WorkbenchConfig& config,
                                     LayoutManager& layout_manager)
    : services_(services),
      title_bar_part_(title_bar_part),
      status_bar_part_(status_bar_part),
      activity_bar_part_(activity_bar_part),
      primary_sidebar_part_(primary_sidebar_part),
      editor_area_part_(editor_area_part),
      config_(config),
      layout_manager_(layout_manager)
{
    last_sidebar_visible_ = services_.GetLayoutService().IsPrimarySidebarVisible();
    last_activity_item_ = (ActivityBarItem)services_.GetActivityBarService().GetSelectedItem();
}

LayoutInfo WorkbenchRenderer::ComputeLayout(const WorkbenchMetrics& metrics)
{
    LayoutState state = services_.GetLayoutService().GetState();

    return layout_manager_.Calculate(metrics, state);
}

void WorkbenchRenderer::RenderActivityBar(const WorkbenchMetrics& metrics)
{
    RenderActivityBar(metrics.title_h, metrics.status_bar_h, metrics.activity_bar_w);
}

void WorkbenchRenderer::RenderTitleBar(const WorkbenchMetrics& metrics, SDL_Window* window)
{
    RenderTitleBar(window, metrics.title_h);
}

void WorkbenchRenderer::RenderStatusBar(const WorkbenchMetrics& metrics, SDL_Window* window)
{
    RenderStatusBar(window, metrics.status_bar_h, metrics.title_h);
}

void WorkbenchRenderer::RenderPrimarySidebar(const WorkbenchMetrics& metrics)
{
    if (!services_.GetLayoutService().IsPrimarySidebarVisible())
        return;

    RenderPrimarySidebar(metrics.activity_bar_w, metrics.title_h, metrics.status_bar_h, metrics.primary_sidebar_w);
}

void WorkbenchRenderer::HandleCreateSceneTabRequests()
{
    ::std::string requested_plugin_id;
    ::Scenes::ScenePluginRegistry::Instance().EnsureLoaded();
    while (::UI::ConsumeCreateSceneTabRequest(requested_plugin_id)) {
        const auto* descriptor = ::Scenes::ScenePluginRegistry::Instance().FindPluginById(requested_plugin_id);
        if (!descriptor) {
            continue;
        }

        EditorTab new_tab;
        int same_type_count = 0;
        for (const auto& tab : services_.GetEditorAreaService().GetTabs()) {
            if (tab.scene_plugin_id == requested_plugin_id) {
                ++same_type_count;
            }
        }

        new_tab.name = descriptor->name + "-" + std::to_string(same_type_count + 1);
        new_tab.path = "";
        new_tab.modified = false;
        new_tab.active = true;
        new_tab.scene_plugin_id = requested_plugin_id;
        services_.GetEditorAreaService().AddTab(new_tab);
    }
}

void WorkbenchRenderer::RenderEditorArea(const WorkbenchMetrics& metrics, const LayoutInfo& layout)
{
    // Dispatch queued cross-scene events once per frame.
    services_.GetEventBus().Dispatch();

    HandleCreateSceneTabRequests();

    bool block_tab_clicks = services_.GetTitleBarService().GetActiveMenu() != TitleBarMenu::None;
    block_tab_clicks = block_tab_clicks || services_.GetTitleBarService().ConsumeBlockTabClicksOnce();
    RenderEditorArea(
        layout.left_offset,
        layout.right_offset,
        metrics.title_h,
        metrics.status_bar_h,
        block_tab_clicks
    );
}

void WorkbenchRenderer::RenderActivityBar(float title_h, float status_bar_h, float activity_bar_w)
{
    ActivityBarResult result = activity_bar_part_.Render(title_h, status_bar_h, activity_bar_w);
    if (result.item_clicked) {
        const std::string& primary_plugin_id = UI::GetPrimarySidebarGlobalPluginId();
        const ViewDefinition def = UI::GetDefaultPrimaryViewForPlugin(primary_plugin_id, result.selected_item);
        if (def.renderer) {
            services_.GetViewRegistry().SetActiveViewById(primary_plugin_id, def.id);
        }
        if (services_.GetLayoutService().IsPrimarySidebarVisible() && result.selected_item == last_activity_item_) {
            services_.GetLayoutService().SetPrimarySidebarVisible(false);
            last_sidebar_visible_ = false;
        } else {
            services_.GetLayoutService().SetPrimarySidebarVisible(true);
            last_sidebar_visible_ = true;
        }
        last_activity_item_ = result.selected_item;
    }
}

void WorkbenchRenderer::SyncPrimarySidebarViewSelection(const std::string& primary_plugin_id, ActivityBarItem active_item)
{
    const bool selection_changed = !has_primary_view_selection_
        || last_primary_view_item_ != active_item
        || last_primary_view_plugin_id_ != primary_plugin_id;
    if (!selection_changed) {
        return;
    }

    if (const char* view_id = PrimarySidebarViewIdForItem(active_item)) {
        services_.GetViewRegistry().SetActiveViewById(primary_plugin_id, view_id);
    }
    last_primary_view_item_ = active_item;
    last_primary_view_plugin_id_ = primary_plugin_id;
    has_primary_view_selection_ = true;
}

void WorkbenchRenderer::RenderPrimarySidebar(float activity_bar_w, float title_h, float status_bar_h, float primary_sidebar_w)
{
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    const std::string& primary_plugin_id = UI::GetPrimarySidebarGlobalPluginId();
    ActivityBarItem active_item = (ActivityBarItem)activity_bar_part_.GetService().GetSelectedItem();
    SyncPrimarySidebarViewSelection(primary_plugin_id, active_item);
    primary_sidebar_part_.Render(activity_bar_w, title_h, status_bar_h, primary_sidebar_w, services_.GetViewRegistry(), primary_plugin_id, active_tab, active_item);
}

void WorkbenchRenderer::RenderTitleBar(SDL_Window* window, float title_h)
{
    bool primary_visible = services_.GetLayoutService().IsPrimarySidebarVisible();
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    const auto ext = WorkbenchSceneExtensionResolver::ResolveTitleBar(active_tab, title_h);

    title_bar_part_.Render(window,
        title_h,
        primary_visible,
        active_tab,
        ext.extension,
        ext.action_handler,
        ext.extension_width);
}

void WorkbenchRenderer::RenderStatusBar(SDL_Window* window, float status_bar_h, float title_h)
{
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    const auto ext = WorkbenchSceneExtensionResolver::ResolveStatusBar(active_tab);

    status_bar_part_.Render(window, status_bar_h, title_h, active_tab, ext.extension, ext.action_handler);
}

void WorkbenchRenderer::RenderEditorArea(float left_offset, float right_offset, float title_h, float status_bar_h, bool block_tab_clicks)
{
    editor_area_part_.Render(left_offset, right_offset, title_h, status_bar_h, block_tab_clicks);
}
