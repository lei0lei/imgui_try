/**
 * @file workbench_renderer.cpp
 * @brief 工作台UI布局的主渲染器
 * @author Your Name
 * @date 2026-02-05
 */

#include "workbench_renderer.h"
#include "../ui/view_registry_defaults.h"
#include "../ui/view_registry_defaults_config.h"
#include "../workbench/workbench_config.h"
#include "imgui.h"

#include <string>

namespace {
const char* PrimarySidebarViewIdForItem(ActivityBarItem item)
{
    switch (item) {
        case ActivityBarItem::Explorer: return "explorer";
        case ActivityBarItem::Search: return "search";
        case ActivityBarItem::NodeEditor: return "node";
        case ActivityBarItem::Debug: return "debug";
        case ActivityBarItem::Editor: return "editor";
        case ActivityBarItem::Extensions: return "extensions";
        default: return nullptr;
    }
}
}

WorkbenchRenderer::WorkbenchRenderer(ServiceCollection& services,
                                     TitleBarPart& title_bar_part,
                                     StatusBarPart& status_bar_part,
                                     PanelPart& panel_part,
                                     ActivityBarPart& activity_bar_part,
                                     PrimarySidebarPart& primary_sidebar_part,
                                     SecondarySidebarPart& secondary_sidebar_part,
                                     EditorAreaPart& editor_area_part,
                                     WorkbenchConfig& config,
                                     LayoutManager& layout_manager,
                                     WorkbenchCommandController& command_controller)
    : services_(services),
      title_bar_part_(title_bar_part),
      status_bar_part_(status_bar_part),
      panel_part_(panel_part),
      activity_bar_part_(activity_bar_part),
      primary_sidebar_part_(primary_sidebar_part),
      secondary_sidebar_part_(secondary_sidebar_part),
      editor_area_part_(editor_area_part),
      config_(config),
      layout_manager_(layout_manager),
      command_controller_(command_controller)
{
    last_sidebar_visible_ = services_.GetLayoutService().IsPrimarySidebarVisible();
    last_activity_item_ = (ActivityBarItem)services_.GetActivityBarService().GetSelectedItem();
    last_active_tab_index_ = services_.GetEditorAreaService().GetActiveTabIndex();
    has_active_tab_index_ = true;
}

bool WorkbenchRenderer::ResolvePanelVisible(EditorTab* active_tab) const
{
    if (active_tab) {
        return services_.GetEditorAreaService().GetPanelVisibleForActiveTab(false);
    }
    if (!command_controller_.AllowWithoutEditor(LayoutRegion::Panel)) {
        return false;
    }
    return services_.GetLayoutService().IsPanelVisible();
}

bool WorkbenchRenderer::ResolveSecondaryVisible(EditorTab* active_tab) const
{
    if (active_tab) {
        return services_.GetEditorAreaService().GetSecondaryVisibleForActiveTab(false);
    }
    if (!command_controller_.AllowWithoutEditor(LayoutRegion::SecondarySidebar)) {
        return false;
    }
    return services_.GetLayoutService().IsSecondarySidebarVisible();
}

void WorkbenchRenderer::SyncLayoutFromActiveTab(EditorTab* active_tab)
{
    if (!active_tab) {
        return;
    }

    const bool panel_visible = services_.GetEditorAreaService().GetPanelVisibleForActiveTab(false);
    const bool secondary_visible = services_.GetEditorAreaService().GetSecondaryVisibleForActiveTab(false);
    services_.GetLayoutService().SetPanelVisible(panel_visible);
    services_.GetLayoutService().SetSecondarySidebarVisible(secondary_visible);
    command_controller_.SetAllowWithoutEditor(LayoutRegion::Panel, false);
    command_controller_.SetAllowWithoutEditor(LayoutRegion::SecondarySidebar, false);
}

LayoutInfo WorkbenchRenderer::ComputeLayout(const WorkbenchMetrics& metrics)
{
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    SyncLayoutFromActiveTab(active_tab);

    LayoutState state = services_.GetLayoutService().GetState();
    state.panel_visible = ResolvePanelVisible(active_tab);
    state.secondary_sidebar_visible = ResolveSecondaryVisible(active_tab);

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

void WorkbenchRenderer::RenderEditorArea(const WorkbenchMetrics& metrics, const LayoutInfo& layout)
{
    SceneType requested_mode;
    while (UI::ConsumeCreateSceneTabRequest(requested_mode)) {
        EditorTab new_tab;
        int same_type_count = 0;
        for (const auto& tab : services_.GetEditorAreaService().GetTabs()) {
            if (tab.scene_type == requested_mode) {
                ++same_type_count;
            }
        }

        if (requested_mode == SceneType::Scene2D) {
            new_tab.name = "2D-Scene-" + std::to_string(same_type_count + 1);
        } else if (requested_mode == SceneType::NodeEditor) {
            new_tab.name = "Node-Graph-" + std::to_string(same_type_count + 1);
            new_tab.node_canvas_pan = ImVec2(0.0f, 0.0f);
            new_tab.node_canvas_zoom = 1.0f;
        } else {
            new_tab.name = "3D-Scene-" + std::to_string(same_type_count + 1);
        }

        new_tab.path = "";
        new_tab.modified = false;
        new_tab.active = true;
        new_tab.scene_type = requested_mode;
        new_tab.secondary_active_view_id = UI::GetDefaultSecondaryView(requested_mode).id;
        new_tab.panel_active_view_id = UI::GetDefaultPanelView(requested_mode).id;
        services_.GetEditorAreaService().AddTab(new_tab);
    }

    const int active_tab_index_before = services_.GetEditorAreaService().GetActiveTabIndex();
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    SyncLayoutFromActiveTab(active_tab);
    bool panel_visible = ResolvePanelVisible(active_tab);
    bool block_tab_clicks = services_.GetTitleBarService().GetActiveMenu() != TitleBarMenu::None;
    block_tab_clicks = block_tab_clicks || services_.GetTitleBarService().ConsumeBlockTabClicksOnce();
    RenderEditorArea(
        layout.left_offset,
        layout.right_offset,
        metrics.title_h,
        metrics.status_bar_h,
        metrics.panel_h,
        panel_visible,
        block_tab_clicks
    );

    const int active_tab_index_after = services_.GetEditorAreaService().GetActiveTabIndex();
    if (active_tab_index_after != active_tab_index_before) {
        const LayoutInfo updated_layout = ComputeLayout(metrics);
        EditorTab* updated_active_tab = services_.GetEditorAreaService().GetActiveTab();
        bool updated_panel_visible = ResolvePanelVisible(updated_active_tab);

        RenderEditorArea(
            updated_layout.left_offset,
            updated_layout.right_offset,
            metrics.title_h,
            metrics.status_bar_h,
            metrics.panel_h,
            updated_panel_visible,
            true
        );
    }
}

void WorkbenchRenderer::RenderPanel(const WorkbenchMetrics& metrics, const LayoutInfo& layout)
{
    RenderPanel(layout.left_offset, layout.right_offset, metrics.status_bar_h, metrics.panel_h);
}

void WorkbenchRenderer::RenderSecondarySidebar(const WorkbenchMetrics& metrics, const LayoutInfo& layout)
{
    RenderSecondarySidebar(metrics.title_h, metrics.status_bar_h, layout.secondary_panel_h, metrics.secondary_sidebar_w);
}

void WorkbenchRenderer::RenderPanelAndSecondary(const WorkbenchMetrics& metrics, const LayoutInfo& layout_before)
{
    LayoutInfo layout_after = ComputeLayout(metrics);
    const int current_active_tab_index = services_.GetEditorAreaService().GetActiveTabIndex();
    const bool active_tab_switched = has_active_tab_index_ && (current_active_tab_index != last_active_tab_index_);
    {
        ImGuiIO& io = ImGui::GetIO();
        const WorkbenchThemeColors& colors = GetWorkbenchTheme().colors;
        ImDrawList* bg = ImGui::GetBackgroundDrawList();

        float top = metrics.title_h;
        float bottom = io.DisplaySize.y - metrics.status_bar_h;

        if (!active_tab_switched && layout_after.right_offset < layout_before.right_offset)
        {
            float x0 = io.DisplaySize.x - layout_before.right_offset;
            float x1 = io.DisplaySize.x - layout_after.right_offset;
            bg->AddRectFilled(ImVec2(x0, top), ImVec2(x1, bottom), ImGui::GetColorU32(colors.editor_welcome_bg));
        }

        if (!active_tab_switched && layout_after.secondary_panel_h < layout_before.secondary_panel_h)
        {
            float old_bottom = io.DisplaySize.y - metrics.status_bar_h - layout_before.secondary_panel_h;
            float new_bottom = io.DisplaySize.y - metrics.status_bar_h - layout_after.secondary_panel_h;
            float x0 = layout_after.left_offset;
            float x1 = io.DisplaySize.x - layout_after.right_offset;
            bg->AddRectFilled(ImVec2(x0, old_bottom), ImVec2(x1, new_bottom), ImGui::GetColorU32(colors.editor_welcome_bg));
        }
    }

    RenderPanel(metrics, layout_after);
    RenderSecondarySidebar(metrics, layout_after);

    last_active_tab_index_ = current_active_tab_index;
    has_active_tab_index_ = true;
}

void WorkbenchRenderer::RenderActivityBar(float title_h, float status_bar_h, float activity_bar_w)
{
    ActivityBarResult result = activity_bar_part_.Render(title_h, status_bar_h, activity_bar_w);
    if (result.item_clicked) {
        SceneType mode = services_.GetEditorAreaService().GetActiveSceneType(SceneType::Scene3D);
        const ViewDefinition def = UI::GetDefaultPrimaryView(mode, result.selected_item);
        if (def.renderer) {
            services_.GetViewRegistry().SetActiveViewById(mode, ViewContainer::PrimarySidebar, def.id);
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

void WorkbenchRenderer::RenderPrimarySidebar(float activity_bar_w, float title_h, float status_bar_h, float primary_sidebar_w)
{
    SceneType mode = services_.GetEditorAreaService().GetActiveSceneType(SceneType::Scene3D);
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    ActivityBarItem active_item = (ActivityBarItem)activity_bar_part_.GetService().GetSelectedItem();
    const bool selection_changed = !has_primary_view_selection_
        || last_primary_view_item_ != active_item
        || last_primary_view_scene_ != mode;
    if (selection_changed) {
        if (const char* view_id = PrimarySidebarViewIdForItem(active_item)) {
            services_.GetViewRegistry().SetActiveViewById(mode, ViewContainer::PrimarySidebar, view_id);
        }
        last_primary_view_item_ = active_item;
        last_primary_view_scene_ = mode;
        has_primary_view_selection_ = true;
    }
    primary_sidebar_part_.Render(activity_bar_w, title_h, status_bar_h, primary_sidebar_w, services_.GetViewRegistry(), mode, active_tab, active_item);
}

void WorkbenchRenderer::RenderSecondarySidebar(float title_h, float status_bar_h, float panel_h, float secondary_sidebar_w)
{
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    SyncLayoutFromActiveTab(active_tab);
    bool visible = ResolveSecondaryVisible(active_tab);
    if (!visible) {
        secondary_sidebar_part_.GetService().SetVisible(false);
        return;
    }
    if (!active_tab && !command_controller_.AllowWithoutEditor(LayoutRegion::SecondarySidebar)) {
        services_.GetLayoutService().SetSecondarySidebarVisible(false);
        secondary_sidebar_part_.GetService().SetVisible(false);
        return;
    }

    secondary_sidebar_part_.GetService().SetVisible(true);

    SceneType mode = active_tab ? active_tab->scene_type : SceneType::Scene3D;
    SecondarySidebarResult result = secondary_sidebar_part_.Render(title_h, status_bar_h, panel_h, secondary_sidebar_w, services_.GetViewRegistry(), mode, active_tab);
    if (result.request_close)
    {
        services_.GetLayoutService().SetSecondarySidebarVisible(false);
        secondary_sidebar_part_.GetService().SetVisible(false);
        if (active_tab) {
            services_.GetEditorAreaService().SetSecondaryVisibleForActiveTab(false);
        } else {
            command_controller_.SetAllowWithoutEditor(LayoutRegion::SecondarySidebar, false);
        }
    }
}

void WorkbenchRenderer::RenderPanel(float left_offset, float right_offset, float status_bar_h, float panel_h)
{
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    SyncLayoutFromActiveTab(active_tab);
    bool visible = ResolvePanelVisible(active_tab);
    if (!visible)
        return;
    if (!active_tab && !command_controller_.AllowWithoutEditor(LayoutRegion::Panel)) {
        ImGuiIO& io = ImGui::GetIO();
        const WorkbenchTheme& theme = GetWorkbenchTheme();
        ImDrawList* bg = ImGui::GetBackgroundDrawList();
        float panel_x = left_offset;
        float panel_y = io.DisplaySize.y - status_bar_h - panel_h;
        float panel_width = io.DisplaySize.x - left_offset - right_offset;
        bg->AddRectFilled(ImVec2(panel_x, panel_y),
                          ImVec2(panel_x + panel_width, io.DisplaySize.y - status_bar_h),
                          ImGui::GetColorU32(theme.colors.editor_welcome_bg));
        services_.GetLayoutService().SetPanelVisible(false);
        return;
    }

    SceneType mode = active_tab ? active_tab->scene_type : SceneType::Scene3D;
    panel_part_.Render(left_offset, right_offset, status_bar_h, panel_h, services_.GetViewRegistry(), mode, active_tab);
}

void WorkbenchRenderer::RenderTitleBar(SDL_Window* window, float title_h)
{
    bool primary_visible = services_.GetLayoutService().IsPrimarySidebarVisible();
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    SyncLayoutFromActiveTab(active_tab);
    bool panel_visible = ResolvePanelVisible(active_tab);
    bool secondary_visible = ResolveSecondaryVisible(active_tab);
    title_bar_part_.Render(window, title_h, primary_visible, panel_visible, secondary_visible);
}

void WorkbenchRenderer::RenderStatusBar(SDL_Window* window, float status_bar_h, float title_h)
{
    status_bar_part_.Render(window, status_bar_h, title_h);
}

void WorkbenchRenderer::RenderEditorArea(float left_offset, float right_offset, float title_h, float status_bar_h, float panel_h, bool panel_visible, bool block_tab_clicks)
{
    editor_area_part_.Render(left_offset, right_offset, title_h, status_bar_h, panel_h, panel_visible, block_tab_clicks);
}
