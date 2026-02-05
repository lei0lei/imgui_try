#include "workbench.h"
#include "../command/command_handlers.h"
#include <vector>
#include <cstdio>

void Workbench::RenderStatusBar(float status_bar_h, float title_h)
{
    renderer_.RenderStatusBar(window_, status_bar_h, title_h);
}

void Workbench::RenderActivityBar(const WorkbenchMetrics& metrics)
{
    renderer_.RenderActivityBar(metrics);
}

void Workbench::RenderTitleBar(const WorkbenchMetrics& metrics)
{
    renderer_.RenderTitleBar(metrics, window_);
}

void Workbench::RenderStatusBar(const WorkbenchMetrics& metrics)
{
    renderer_.RenderStatusBar(metrics, window_);
}

void Workbench::RenderPrimarySidebar(const WorkbenchMetrics& metrics)
{
    renderer_.RenderPrimarySidebar(metrics);
}

void Workbench::RenderEditorArea(const WorkbenchMetrics& metrics, const LayoutInfo& layout)
{
    renderer_.RenderEditorArea(metrics, layout);
}

void Workbench::RenderPanel(const WorkbenchMetrics& metrics, const LayoutInfo& layout)
{
    renderer_.RenderPanel(metrics, layout);
}

void Workbench::RenderSecondarySidebar(const WorkbenchMetrics& metrics, const LayoutInfo& layout)
{
    renderer_.RenderSecondarySidebar(metrics, layout);
}

void Workbench::RenderPanelAndSecondary(const WorkbenchMetrics& metrics, const LayoutInfo& layout_before)
{
    renderer_.RenderPanelAndSecondary(metrics, layout_before);
}

LayoutInfo Workbench::ComputeLayout(const WorkbenchMetrics& metrics) const
{
     return renderer_.ComputeLayout(metrics);
}

const WorkbenchMetrics& Workbench::GetUiMetrics() const
{
    return renderer_.GetUiMetrics();
}

void Workbench::UpdateFrameStats(float fps)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "FPS: %.1f", fps);
    services_.GetNotificationService().SetMessage(buf);
}

void Workbench::RenderActivityBar(float title_h, float status_bar_h, float activity_bar_w)
{
    renderer_.RenderActivityBar(title_h, status_bar_h, activity_bar_w);
}

void Workbench::RenderPrimarySidebar(float activity_bar_w, float title_h, float status_bar_h, float primary_sidebar_w)
{
    renderer_.RenderPrimarySidebar(activity_bar_w, title_h, status_bar_h, primary_sidebar_w);
}

void Workbench::RenderSecondarySidebar(float title_h, float status_bar_h, float panel_h, float secondary_sidebar_w)
{
    renderer_.RenderSecondarySidebar(title_h, status_bar_h, panel_h, secondary_sidebar_w);
}

void Workbench::RenderPanel(float left_offset, float right_offset, float status_bar_h, float panel_h)
{
    renderer_.RenderPanel(left_offset, right_offset, status_bar_h, panel_h);
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
          editor_area_part_(services_.GetEditorAreaService()),
          command_controller_(
                command_service_,
                services_.GetTitleBarService(),
                services_.GetNotificationService(),
                services_.GetPanelService(),
                services_.GetActivityBarService(),
                services_.GetEditorAreaService(),
                services_.GetLayoutService(),
                services_.GetSecondarySidebarService()),
          renderer_(
                services_,
                title_bar_part_,
                status_bar_part_,
                panel_part_,
                activity_bar_part_,
                primary_sidebar_part_,
                secondary_sidebar_part_,
                editor_area_part_,
                config_,
                layout_manager_,
                command_controller_)
{
    // 可初始化其它状态
    services_.GetPanelService().Reset();
    services_.GetLayoutService().SetPrimarySidebarVisible(true);
    UI::RegisterDefaultViews(services_.GetViewRegistry());
    command_controller_.RegisterCommands(
        [this](bool visible) { SetPrimarySidebarVisible(visible); },
        []() {},
        [this]() { SDL_MinimizeWindow(window_); },
        [this]() {
            static bool is_max = false;
            is_max ? SDL_RestoreWindow(window_) : SDL_MaximizeWindow(window_);
            is_max = !is_max;
        },
        [this]() {},
        [this]() { services_.GetLayoutService().TogglePrimarySidebar(); },
        []() {},
        []() {}
    );
}

void Workbench::SetPrimarySidebarVisible(bool visible)
{
    services_.GetLayoutService().SetPrimarySidebarVisible(visible);
}

void Workbench::RenderTitleBar(float title_h)
{
    renderer_.RenderTitleBar(window_, title_h);
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
    renderer_.RenderEditorArea(left_offset, right_offset, title_h, status_bar_h, panel_h, panel_visible, block_tab_clicks);
}

void Workbench::HandleWindowAndMenuActions(bool& done, SDL_Window* window) {
    (void)window;
    command_controller_.HandleWindowAndMenuActions(done);
}
