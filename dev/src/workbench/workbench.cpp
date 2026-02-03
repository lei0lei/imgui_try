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
    RenderEditorArea(
        layout.left_offset,
        layout.right_offset,
        metrics.title_h,
        metrics.status_bar_h,
        metrics.panel_h,
        services_.GetLayoutService().IsPanelVisible()
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

LayoutInfo Workbench::ComputeLayout(const WorkbenchMetrics& metrics) const
{
    return layout_manager_.Calculate(metrics, services_.GetLayoutService().GetState());
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
    ActivityBarItem active_item = (ActivityBarItem)activity_bar_part_.GetService().GetSelectedItem();
    primary_sidebar_part_.Render(active_item, activity_bar_w, title_h, status_bar_h, primary_sidebar_w);
}

void Workbench::RenderSecondarySidebar(float title_h, float status_bar_h, float panel_h, float secondary_sidebar_w)
{
    if (!services_.GetLayoutService().IsSecondarySidebarVisible()) {
        secondary_sidebar_part_.GetService().SetVisible(false);
        return;
    }

    secondary_sidebar_part_.GetService().SetVisible(true);

    SecondarySidebarResult result = secondary_sidebar_part_.Render(title_h, status_bar_h, panel_h, secondary_sidebar_w);
    if (result.request_close)
    {
        services_.GetLayoutService().SetSecondarySidebarVisible(false);
        secondary_sidebar_part_.GetService().SetVisible(false);
    }
}

void Workbench::RenderPanel(float left_offset, float right_offset, float status_bar_h, float panel_h)
{
    if (!services_.GetLayoutService().IsPanelVisible())
        return;

    panel_part_.Render(left_offset, right_offset, status_bar_h, panel_h);
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
    RegisterCommands();
}

void Workbench::SetPrimarySidebarVisible(bool visible)
{
    services_.GetLayoutService().SetPrimarySidebarVisible(visible);
}

void Workbench::RenderTitleBar(float title_h)
{
    title_bar_part_.Render(window_, title_h, services_.GetLayoutService().IsPrimarySidebarVisible(), services_.GetLayoutService().IsPanelVisible(), services_.GetLayoutService().IsSecondarySidebarVisible());
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

void Workbench::RenderEditorArea(float left_offset, float right_offset, float title_h, float status_bar_h, float panel_h, bool panel_visible) {
    editor_area_part_.Render(left_offset, right_offset, title_h, status_bar_h, panel_h, panel_visible);
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
        [this]() { services_.GetLayoutService().TogglePanel(); },
        [this]() {
            services_.GetLayoutService().ToggleSecondarySidebar();
            secondary_sidebar_part_.GetService().SetVisible(services_.GetLayoutService().IsSecondarySidebarVisible());
        }
    };

    RegisterWorkbenchCommands(command_service_, ctx);
}
