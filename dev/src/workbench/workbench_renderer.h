#pragma once

#include <SDL3/SDL.h>
#include <string>
#include "../services/service_collection.h"
#include "../services/service_interfaces.h"
#include "workbench_config.h"
#include "layout/layout_manager.h"
#include "parts/title_bar.h"
#include "parts/status_bar.h"
#include "parts/activity_bar.h"
#include "parts/primary_sidebar.h"
#include "parts/editor_area.h"

class WorkbenchRenderer {
public:
    WorkbenchRenderer(ServiceCollection& services,
                      TitleBarPart& title_bar_part,
                      StatusBarPart& status_bar_part,
                      ActivityBarPart& activity_bar_part,
                      PrimarySidebarPart& primary_sidebar_part,
                      EditorAreaPart& editor_area_part,
                      WorkbenchConfig& config,
                      LayoutManager& layout_manager);

    const WorkbenchMetrics& GetUiMetrics() const { return config_.Metrics(); }

    LayoutInfo ComputeLayout(const WorkbenchMetrics& metrics);

    void RenderActivityBar(const WorkbenchMetrics& metrics);
    void RenderTitleBar(const WorkbenchMetrics& metrics, SDL_Window* window);
    void RenderStatusBar(const WorkbenchMetrics& metrics, SDL_Window* window);
    void RenderPrimarySidebar(const WorkbenchMetrics& metrics);
    void RenderEditorArea(const WorkbenchMetrics& metrics, const LayoutInfo& layout);

private:
    void HandleCreateSceneTabRequests();
    void SyncPrimarySidebarViewSelection(const std::string& primary_plugin_id, ActivityBarItem active_item);

    void RenderActivityBar(float title_h, float status_bar_h, float activity_bar_w);
    void RenderPrimarySidebar(float activity_bar_w, float title_h, float status_bar_h, float primary_sidebar_w);
    void RenderTitleBar(SDL_Window* window, float title_h);
    void RenderStatusBar(SDL_Window* window, float status_bar_h, float title_h);
    void RenderEditorArea(float left_offset, float right_offset, float title_h, float status_bar_h, bool block_tab_clicks);

    ServiceCollection& services_;
    TitleBarPart& title_bar_part_;
    StatusBarPart& status_bar_part_;
    ActivityBarPart& activity_bar_part_;
    PrimarySidebarPart& primary_sidebar_part_;
    EditorAreaPart& editor_area_part_;
    WorkbenchConfig& config_;
    LayoutManager& layout_manager_;

    bool last_sidebar_visible_ = true;
    ActivityBarItem last_activity_item_ = ActivityBarItem::None;
    ActivityBarItem last_primary_view_item_ = ActivityBarItem::None;
    std::string last_primary_view_plugin_id_;
    bool has_primary_view_selection_ = false;
};
