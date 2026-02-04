#include "workbench.h"
#include "../command/command_handlers.h"
#include <vector>
#include <cstdio>

void Workbench::RenderStatusBar(float status_bar_h, float title_h)
{
    status_bar_part_.Render(window_, status_bar_h, title_h);
}

void Workbench::RenderActivityBar(const WorkbenchMetrics& metrics)
{
    RenderActivityBar(metrics.title_h, metrics.status_bar_h, metrics.activity_bar_w);
}

void Workbench::RenderTitleBar(const WorkbenchMetrics& metrics)
{
    RenderTitleBar(metrics.title_h);
}

void Workbench::RenderStatusBar(const WorkbenchMetrics& metrics)
{
    RenderStatusBar(metrics.status_bar_h, metrics.title_h);
}

void Workbench::RenderPrimarySidebar(const WorkbenchMetrics& metrics)
{
    if (!services_.GetLayoutService().IsPrimarySidebarVisible())
        return;

    RenderPrimarySidebar(metrics.activity_bar_w, metrics.title_h, metrics.status_bar_h, metrics.primary_sidebar_w);
}

void Workbench::RenderEditorArea(const WorkbenchMetrics& metrics, const LayoutInfo& layout)
{
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    bool panel_visible = active_tab ? active_tab->panel_visible : services_.GetLayoutService().IsPanelVisible();
    if (!active_tab && !allow_panel_without_editor_) {
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
}

void Workbench::RenderPanel(const WorkbenchMetrics& metrics, const LayoutInfo& layout)
{
    RenderPanel(layout.left_offset, layout.right_offset, metrics.status_bar_h, metrics.panel_h);
}

void Workbench::RenderSecondarySidebar(const WorkbenchMetrics& metrics, const LayoutInfo& layout)
{
    RenderSecondarySidebar(metrics.title_h, metrics.status_bar_h, layout.secondary_panel_h, metrics.secondary_sidebar_w);
}

void Workbench::RenderPanelAndSecondary(const WorkbenchMetrics& metrics, const LayoutInfo& layout_before)
{
    LayoutInfo layout_after = ComputeLayout(metrics);
    {
        ImGuiIO& io = ImGui::GetIO();
        const WorkbenchThemeColors& colors = GetWorkbenchTheme().colors;
        ImDrawList* bg = ImGui::GetBackgroundDrawList();

        float top = metrics.title_h;
        float bottom = io.DisplaySize.y - metrics.status_bar_h;

        if (layout_after.right_offset < layout_before.right_offset)
        {
            float x0 = io.DisplaySize.x - layout_before.right_offset;
            float x1 = io.DisplaySize.x - layout_after.right_offset;
            bg->AddRectFilled(ImVec2(x0, top), ImVec2(x1, bottom), ImGui::GetColorU32(colors.editor_welcome_bg));
        }

        if (layout_after.secondary_panel_h < layout_before.secondary_panel_h)
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
}

LayoutInfo Workbench::ComputeLayout(const WorkbenchMetrics& metrics) const
{
     LayoutState state = services_.GetLayoutService().GetState();
     const EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
     if (active_tab) {
     	 state.panel_visible = active_tab->panel_visible;
     	 state.secondary_sidebar_visible = active_tab->secondary_sidebar_visible;
     } else {
     	 if (!allow_secondary_without_editor_) {
     	 	 state.secondary_sidebar_visible = false;
     	 }
     	 if (!allow_panel_without_editor_) {
     	 	 state.panel_visible = false;
     	 }
     }
     return layout_manager_.Calculate(metrics, state);
}

const WorkbenchMetrics& Workbench::GetUiMetrics() const
{
    return config_.Metrics();
}

void Workbench::UpdateFrameStats(float fps)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "FPS: %.1f", fps);
    services_.GetNotificationService().SetMessage(buf);
}

void Workbench::RenderActivityBar(float title_h, float status_bar_h, float activity_bar_w)
{
    ActivityBarResult result = activity_bar_part_.Render(title_h, status_bar_h, activity_bar_w);
    if (result.item_clicked) {
        SceneType mode = services_.GetEditorAreaService().GetActiveSceneType(SceneType::Scene3D);
        switch (result.selected_item) {
            case ActivityBarItem::Explorer:
                services_.GetViewRegistry().SetActiveViewById(mode, ViewContainer::PrimarySidebar, "explorer");
                break;
            case ActivityBarItem::Search:
                services_.GetViewRegistry().SetActiveViewById(mode, ViewContainer::PrimarySidebar, "search");
                break;
            case ActivityBarItem::NodeEditor:
                services_.GetViewRegistry().SetActiveViewById(mode, ViewContainer::PrimarySidebar, "node");
                break;
            case ActivityBarItem::Debug:
                services_.GetViewRegistry().SetActiveViewById(mode, ViewContainer::PrimarySidebar, "debug");
                break;
            case ActivityBarItem::Extensions:
                services_.GetViewRegistry().SetActiveViewById(mode, ViewContainer::PrimarySidebar, "extensions");
                break;
            default:
                break;
        }
        if (services_.GetLayoutService().IsPrimarySidebarVisible() && result.selected_item == last_activity_item_) {
            SetPrimarySidebarVisible(false);
            last_sidebar_visible_ = false;
        } else {
            SetPrimarySidebarVisible(true);
            last_sidebar_visible_ = true;
        }
        last_activity_item_ = result.selected_item;
    }
}

void Workbench::RenderPrimarySidebar(float activity_bar_w, float title_h, float status_bar_h, float primary_sidebar_w)
{
    SceneType mode = services_.GetEditorAreaService().GetActiveSceneType(SceneType::Scene3D);
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    ActivityBarItem active_item = (ActivityBarItem)activity_bar_part_.GetService().GetSelectedItem();
    switch (active_item) {
        case ActivityBarItem::Explorer:
            services_.GetViewRegistry().SetActiveViewById(mode, ViewContainer::PrimarySidebar, "explorer");
            break;
        case ActivityBarItem::Search:
            services_.GetViewRegistry().SetActiveViewById(mode, ViewContainer::PrimarySidebar, "search");
            break;
        case ActivityBarItem::NodeEditor:
            services_.GetViewRegistry().SetActiveViewById(mode, ViewContainer::PrimarySidebar, "node");
            break;
        case ActivityBarItem::Debug:
            services_.GetViewRegistry().SetActiveViewById(mode, ViewContainer::PrimarySidebar, "debug");
            break;
        case ActivityBarItem::Extensions:
            services_.GetViewRegistry().SetActiveViewById(mode, ViewContainer::PrimarySidebar, "extensions");
            break;
        default:
            break;
    }
    primary_sidebar_part_.Render(activity_bar_w, title_h, status_bar_h, primary_sidebar_w, services_.GetViewRegistry(), mode, active_tab, active_item);
}

void Workbench::RenderSecondarySidebar(float title_h, float status_bar_h, float panel_h, float secondary_sidebar_w)
{
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    bool visible = active_tab ? active_tab->secondary_sidebar_visible : services_.GetLayoutService().IsSecondarySidebarVisible();
    if (!visible) {
        secondary_sidebar_part_.GetService().SetVisible(false);
        return;
    }
    if (!active_tab) {
        if (!allow_secondary_without_editor_) {
            services_.GetLayoutService().SetSecondarySidebarVisible(false);
            secondary_sidebar_part_.GetService().SetVisible(false);
            return;
        }
    } else {
        allow_secondary_without_editor_ = false;
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
            allow_secondary_without_editor_ = false;
        }
    }
}

void Workbench::RenderPanel(float left_offset, float right_offset, float status_bar_h, float panel_h)
{
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    bool visible = active_tab ? active_tab->panel_visible : services_.GetLayoutService().IsPanelVisible();
    if (!visible)
        return;
    if (!active_tab) {
        if (!allow_panel_without_editor_) {
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
        allow_panel_without_editor_ = false;
    }

    SceneType mode = active_tab ? active_tab->scene_type : SceneType::Scene3D;
    panel_part_.Render(left_offset, right_offset, status_bar_h, panel_h, services_.GetViewRegistry(), mode, active_tab);
}

Workbench::Workbench(SDL_Window* window)
        : window_(window),
          services_(),
          title_bar_part_(services_.GetTitleBarService()),
          status_bar_part_(services_.GetNotificationService()),
          panel_part_(services_.GetPanelService()),
          activity_bar_part_(services_.GetActivityBarService()),
          primary_sidebar_part_(services_.GetPrimarySidebarService()),
          secondary_sidebar_part_(services_.GetSecondarySidebarService()),
                    editor_area_part_(services_.GetEditorAreaService())
{
    // 可初始化其它状态
    services_.GetPanelService().Reset();
    services_.GetLayoutService().SetPrimarySidebarVisible(true);
    last_sidebar_visible_ = services_.GetLayoutService().IsPrimarySidebarVisible();
    last_activity_item_ = (ActivityBarItem)services_.GetActivityBarService().GetSelectedItem();
    UI::RegisterDefaultViews(services_.GetViewRegistry());
    RegisterCommands();
}

void Workbench::SetPrimarySidebarVisible(bool visible)
{
    services_.GetLayoutService().SetPrimarySidebarVisible(visible);
}

void Workbench::RenderTitleBar(float title_h)
{
    bool primary_visible = services_.GetLayoutService().IsPrimarySidebarVisible();
    EditorTab* active_tab = services_.GetEditorAreaService().GetActiveTab();
    bool panel_visible = active_tab ? active_tab->panel_visible : services_.GetLayoutService().IsPanelVisible();
    bool secondary_visible = active_tab ? active_tab->secondary_sidebar_visible : services_.GetLayoutService().IsSecondarySidebarVisible();
    if (!active_tab && !allow_panel_without_editor_) {
        panel_visible = false;
    }
    if (!active_tab && !allow_secondary_without_editor_) {
        secondary_visible = false;
    }
    title_bar_part_.Render(window_, title_h, primary_visible, panel_visible, secondary_visible);
}

void Workbench::TogglePrimarySidebar()
{
    SetPrimarySidebarVisible(!services_.GetLayoutService().IsPrimarySidebarVisible());
}

void Workbench::CloseSecondarySidebar()
{
    services_.GetLayoutService().SetSecondarySidebarVisible(false);
    secondary_sidebar_part_.GetService().SetVisible(false);
}

void Workbench::RenderEditorArea(float left_offset, float right_offset, float title_h, float status_bar_h, float panel_h, bool panel_visible, bool block_tab_clicks) {
    editor_area_part_.Render(left_offset, right_offset, title_h, status_bar_h, panel_h, panel_visible, block_tab_clicks);
}

void Workbench::HandleWindowAndMenuActions(bool& done, SDL_Window* window) {
    if (pending_exit_) {
        done = true;
        pending_exit_ = false;
    }

    CommandId cmd = title_bar_part_.GetService().ConsumeLastCommand();
    if (cmd != CommandId::None)
        command_service_.Execute(cmd);
}

void Workbench::RegisterCommands()
{
    CommandHandlersContext ctx{
        services_.GetNotificationService(),
        services_.GetPanelService(),
        services_.GetActivityBarService(),
        services_.GetEditorAreaService(),
        services_.GetLayoutService(),
        [this](bool visible) { SetPrimarySidebarVisible(visible); },
        [this]() { pending_exit_ = true; },
        [this]() { SDL_MinimizeWindow(window_); },
        [this]() {
            static bool is_max = false;
            is_max ? SDL_RestoreWindow(window_) : SDL_MaximizeWindow(window_);
            is_max = !is_max;
        },
        [this]() { pending_exit_ = true; },
        [this]() { services_.GetLayoutService().TogglePrimarySidebar(); },
        [this]() {
            if (auto* tab = services_.GetEditorAreaService().GetActiveTab()) {
                tab->panel_visible = !tab->panel_visible;
                services_.GetLayoutService().SetPanelVisible(tab->panel_visible);
                allow_panel_without_editor_ = false;
            } else {
                bool will_show = !services_.GetLayoutService().IsPanelVisible();
                services_.GetLayoutService().TogglePanel();
                allow_panel_without_editor_ = will_show;
            }
        },
        [this]() {
            if (auto* tab = services_.GetEditorAreaService().GetActiveTab()) {
                tab->secondary_sidebar_visible = !tab->secondary_sidebar_visible;
                services_.GetLayoutService().SetSecondarySidebarVisible(tab->secondary_sidebar_visible);
                allow_secondary_without_editor_ = false;
            } else {
                bool will_show = !services_.GetLayoutService().IsSecondarySidebarVisible();
                services_.GetLayoutService().ToggleSecondarySidebar();
                allow_secondary_without_editor_ = will_show;
            }
            secondary_sidebar_part_.GetService().SetVisible(services_.GetLayoutService().IsSecondarySidebarVisible());
        }
    };

    RegisterWorkbenchCommands(command_service_, ctx);
}
