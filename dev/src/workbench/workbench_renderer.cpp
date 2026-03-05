/**
 * @file workbench_renderer.cpp
 * @brief 工作台UI布局的主渲染器
 * @author Your Name
 * @date 2026-02-05
 */

#include "workbench_renderer.h"
#include "../ui/view_registry_defaults.h"
#include "../workbench/workbench_config.h"
#include "imgui.h"

namespace {
const char* PrimarySidebarViewIdForItem(ActivityBarItem item)
{
    switch (item) {
        case ActivityBarItem::Explorer: return "explorer";
        case ActivityBarItem::Search: return "search";
        case ActivityBarItem::NodeEditor: return "node";
        case ActivityBarItem::Debug: return "debug";
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

LayoutInfo WorkbenchRenderer::ComputeLayout(const WorkbenchMetrics& metrics) const
{
    LayoutState state = services_.GetLayoutService().GetState();
    const EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    if (active_tab) {
        state.panel_visible = active_tab->panel_visible;
        state.secondary_sidebar_visible = active_tab->secondary_sidebar_visible;
    } else {
        if (!command_controller_.AllowSecondaryWithoutEditor()) {
            state.secondary_sidebar_visible = false;
        }
        if (!command_controller_.AllowPanelWithoutEditor()) {
            state.panel_visible = false;
        }
    }
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
    const int active_tab_index_before = services_.GetEditorAreaService().GetActiveTabIndex();
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    bool panel_visible = active_tab ? active_tab->panel_visible : services_.GetLayoutService().IsPanelVisible();
    if (!active_tab && !command_controller_.AllowPanelWithoutEditor()) {
        panel_visible = false;
    }
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
        bool updated_panel_visible = updated_active_tab ? updated_active_tab->panel_visible : services_.GetLayoutService().IsPanelVisible();
        if (!updated_active_tab && !command_controller_.AllowPanelWithoutEditor()) {
            updated_panel_visible = false;
        }

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
    bool visible = active_tab ? active_tab->secondary_sidebar_visible : services_.GetLayoutService().IsSecondarySidebarVisible();
    if (!visible) {
        secondary_sidebar_part_.GetService().SetVisible(false);
        return;
    }
    if (!active_tab) {
        if (!command_controller_.AllowSecondaryWithoutEditor()) {
            services_.GetLayoutService().SetSecondarySidebarVisible(false);
            secondary_sidebar_part_.GetService().SetVisible(false);
            return;
        }
    } else {
        command_controller_.SetAllowSecondaryWithoutEditor(false);
    }

    secondary_sidebar_part_.GetService().SetVisible(true);

    SceneType mode = active_tab ? active_tab->scene_type : SceneType::Scene3D;
    SecondarySidebarResult result = secondary_sidebar_part_.Render(title_h, status_bar_h, panel_h, secondary_sidebar_w, services_.GetViewRegistry(), mode, active_tab);
    if (result.request_close)
    {
        services_.GetLayoutService().SetSecondarySidebarVisible(false);
        secondary_sidebar_part_.GetService().SetVisible(false);
        if (active_tab) {
            active_tab->secondary_sidebar_visible = false;
        } else {
            command_controller_.SetAllowSecondaryWithoutEditor(false);
        }
    }
}

void WorkbenchRenderer::RenderPanel(float left_offset, float right_offset, float status_bar_h, float panel_h)
{
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    bool visible = active_tab ? active_tab->panel_visible : services_.GetLayoutService().IsPanelVisible();
    if (!visible)
        return;
    if (!active_tab) {
        if (!command_controller_.AllowPanelWithoutEditor()) {
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
    } else {
        command_controller_.SetAllowPanelWithoutEditor(false);
    }

    SceneType mode = active_tab ? active_tab->scene_type : SceneType::Scene3D;
    panel_part_.Render(left_offset, right_offset, status_bar_h, panel_h, services_.GetViewRegistry(), mode, active_tab);
}

void WorkbenchRenderer::RenderTitleBar(SDL_Window* window, float title_h)
{
    bool primary_visible = services_.GetLayoutService().IsPrimarySidebarVisible();
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    bool panel_visible = active_tab ? active_tab->panel_visible : services_.GetLayoutService().IsPanelVisible();
    bool secondary_visible = active_tab ? active_tab->secondary_sidebar_visible : services_.GetLayoutService().IsSecondarySidebarVisible();
    if (!active_tab && !command_controller_.AllowPanelWithoutEditor()) {
        panel_visible = false;
    }
    if (!active_tab && !command_controller_.AllowSecondaryWithoutEditor()) {
        secondary_visible = false;
    }
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
