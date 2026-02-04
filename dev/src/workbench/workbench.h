#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <memory>

#include "../command/command_service.h"
#include "parts/title_bar.h"
#include "parts/status_bar.h"
#include "parts/activity_bar.h"
#include "parts/primary_sidebar.h"
#include "parts/secondary_sidebar.h"
#include "parts/panel.h"
#include "parts/editor_area.h"
#include "../services/service_collection.h"
#include "workbench_config.h"
#include "layout/layout_manager.h"
#include "../ui/view_registry_defaults.h"

class Workbench {
public:
    Workbench(SDL_Window* window);
    const WorkbenchMetrics& GetUiMetrics() const;
    void UpdateFrameStats(float fps);
    void RenderActivityBar(const WorkbenchMetrics& metrics);
    void RenderTitleBar(const WorkbenchMetrics& metrics);
    void RenderStatusBar(const WorkbenchMetrics& metrics);
    void RenderPrimarySidebar(const WorkbenchMetrics& metrics);
    void RenderEditorArea(const WorkbenchMetrics& metrics, const LayoutInfo& layout);
    void RenderPanel(const WorkbenchMetrics& metrics, const LayoutInfo& layout);
    void RenderSecondarySidebar(const WorkbenchMetrics& metrics, const LayoutInfo& layout);
    void RenderPanelAndSecondary(const WorkbenchMetrics& metrics, const LayoutInfo& layout_before);
    LayoutInfo ComputeLayout(const WorkbenchMetrics& metrics) const;
    void RenderActivityBar(float title_h, float status_bar_h, float activity_bar_w);
    void RenderPrimarySidebar(float activity_bar_w, float title_h, float status_bar_h, float primary_sidebar_w);
    void RenderSecondarySidebar(float title_h, float status_bar_h, float panel_h, float secondary_sidebar_w);
    void RenderPanel(float left_offset, float right_offset, float status_bar_h, float panel_h);
    void RenderTitleBar(float title_h);
    void RenderStatusBar(float status_bar_h, float title_h);
    // 标题栏事件已改为命令处理

    // 状态暴露（如有需要）
    bool IsPanelVisible() const { return services_.GetLayoutService().IsPanelVisible(); }
    bool IsSecondarySidebarVisible() const { return services_.GetLayoutService().IsSecondarySidebarVisible(); }

    // 新增：primary sidebar 显示状态
    bool IsPrimarySidebarVisible() const { return services_.GetLayoutService().IsPrimarySidebarVisible(); }
    void TogglePrimarySidebar();
    void SetPrimarySidebarVisible(bool visible);
    void CloseSecondarySidebar();

    // 通知 service 访问
    NotificationService& GetNotificationService() { return services_.GetNotificationService(); }

    // 布局 service 访问
    LayoutService& GetLayoutService() { return services_.GetLayoutService(); }
    const LayoutService& GetLayoutService() const { return services_.GetLayoutService(); }

    // 面板 service 访问
    PanelService& GetPanelService() { return services_.GetPanelService(); }
    const PanelService& GetPanelService() const { return services_.GetPanelService(); }

    // ActivityBar service 访问
    ActivityBarService& GetActivityBarService() { return services_.GetActivityBarService(); }
    const ActivityBarService& GetActivityBarService() const { return services_.GetActivityBarService(); }

    // PrimarySidebar service 访问
    PrimarySidebarService& GetPrimarySidebarService() { return services_.GetPrimarySidebarService(); }
    const PrimarySidebarService& GetPrimarySidebarService() const { return services_.GetPrimarySidebarService(); }

    // SecondarySidebar service 访问
    SecondarySidebarService& GetSecondarySidebarService() { return services_.GetSecondarySidebarService(); }
    const SecondarySidebarService& GetSecondarySidebarService() const { return services_.GetSecondarySidebarService(); }

    // 处理窗口和菜单动作
    void HandleWindowAndMenuActions(bool& done, SDL_Window* window);
    // EditorArea service 访问
    EditorAreaService& GetEditorAreaService() { return services_.GetEditorAreaService(); }
    const EditorAreaService& GetEditorAreaService() const { return services_.GetEditorAreaService(); }

    // 渲染编辑器区域
    void RenderEditorArea(float left_offset, float right_offset, float title_h, float status_bar_h, float panel_h, bool panel_visible, bool block_tab_clicks);
private:
    void RegisterCommands();
    SDL_Window* window_;
    CommandService command_service_;
    bool pending_exit_ = false;
    ServiceCollection services_;
    TitleBarPart title_bar_part_;
    StatusBarPart status_bar_part_;
    PanelPart panel_part_;
    ActivityBarPart activity_bar_part_;
    PrimarySidebarPart primary_sidebar_part_;
    SecondarySidebarPart secondary_sidebar_part_;
    bool last_sidebar_visible_ = true;
    ActivityBarItem last_activity_item_ = ActivityBarItem::None;
    EditorAreaPart editor_area_part_;
    WorkbenchConfig config_;
    LayoutManager layout_manager_;
    bool allow_panel_without_editor_ = false;
    bool allow_secondary_without_editor_ = false;
};
