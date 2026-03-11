#pragma once

#include <SDL3/SDL.h>
#include "../services/service_collection.h"
#include "../services/service_interfaces.h"
#include "workbench_config.h"
#include "layout/layout_manager.h"
#include "workbench_command_controller.h"
#include "parts/title_bar.h"
#include "parts/status_bar.h"
#include "parts/panel.h"
#include "parts/activity_bar.h"
#include "parts/primary_sidebar.h"
#include "parts/secondary_sidebar.h"
#include "parts/editor_area.h"

class WorkbenchRenderer {
public:
    WorkbenchRenderer(ServiceCollection& services,
                      TitleBarPart& title_bar_part,
                      StatusBarPart& status_bar_part,
                      PanelPart& panel_part,
                      ActivityBarPart& activity_bar_part,
                      PrimarySidebarPart& primary_sidebar_part,
                      SecondarySidebarPart& secondary_sidebar_part,
                      EditorAreaPart& editor_area_part,
                      WorkbenchConfig& config,
                      LayoutManager& layout_manager,
                      WorkbenchCommandController& command_controller);

    const WorkbenchMetrics& GetUiMetrics() const { return config_.Metrics(); }

    LayoutInfo ComputeLayout(const WorkbenchMetrics& metrics);

    void RenderActivityBar(const WorkbenchMetrics& metrics);
    void RenderTitleBar(const WorkbenchMetrics& metrics, SDL_Window* window);
    void RenderStatusBar(const WorkbenchMetrics& metrics, SDL_Window* window);
    void RenderPrimarySidebar(const WorkbenchMetrics& metrics);
    void RenderEditorArea(const WorkbenchMetrics& metrics, const LayoutInfo& layout);
    void RenderPanel(const WorkbenchMetrics& metrics, const LayoutInfo& layout);
    void RenderSecondarySidebar(const WorkbenchMetrics& metrics, const LayoutInfo& layout);
    void RenderPanelAndSecondary(const WorkbenchMetrics& metrics, const LayoutInfo& layout_before);

private:
    void RenderActivityBar(float title_h, float status_bar_h, float activity_bar_w);
    void RenderPrimarySidebar(float activity_bar_w, float title_h, float status_bar_h, float primary_sidebar_w);
    void RenderSecondarySidebar(float title_h, float status_bar_h, float panel_h, float secondary_sidebar_w);
    void RenderPanel(float left_offset, float right_offset, float status_bar_h, float panel_h);
    void RenderTitleBar(SDL_Window* window, float title_h);
    void RenderStatusBar(SDL_Window* window, float status_bar_h, float title_h);
    void RenderEditorArea(float left_offset, float right_offset, float title_h, float status_bar_h, float panel_h, bool panel_visible, bool block_tab_clicks);

    bool ResolvePanelVisible(EditorTab* active_tab) const;
    bool ResolveSecondaryVisible(EditorTab* active_tab) const;
    void SyncLayoutFromActiveTab(EditorTab* active_tab);

    ServiceCollection& services_;
    TitleBarPart& title_bar_part_;
    StatusBarPart& status_bar_part_;
    PanelPart& panel_part_;
    ActivityBarPart& activity_bar_part_;
    PrimarySidebarPart& primary_sidebar_part_;
    SecondarySidebarPart& secondary_sidebar_part_;
    EditorAreaPart& editor_area_part_;
    WorkbenchConfig& config_;
    LayoutManager& layout_manager_;
    WorkbenchCommandController& command_controller_;

    bool last_sidebar_visible_ = true;
    ActivityBarItem last_activity_item_ = ActivityBarItem::None;
    ActivityBarItem last_primary_view_item_ = ActivityBarItem::None;
    SceneType last_primary_view_scene_ = SceneType::Scene3D;
    bool has_primary_view_selection_ = false;
    int last_active_tab_index_ = -1;
    bool has_active_tab_index_ = false;
};
