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
#include "../services/service_interfaces.h"
#include "workbench_command_controller.h"
#include "workbench_renderer.h"
#include "workbench_config.h"
#include "layout/layout_manager.h"
#include "../ui/view_registry_defaults.h"

class Workbench {
public:
    Workbench(SDL_Window* window);
    const WorkbenchMetrics& GetUiMetrics() const;
    // 更新帧统计信息
    void UpdateFrameStats(float fps);
    // 布局渲染
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
    INotificationService& GetNotificationService() { return services_.GetNotificationService(); }

    // 布局 service 访问
    ILayoutService& GetLayoutService() { return services_.GetLayoutService(); }
    const ILayoutService& GetLayoutService() const { return services_.GetLayoutService(); }

    // 面板 service 访问
    IPanelService& GetPanelService() { return services_.GetPanelService(); }
    const IPanelService& GetPanelService() const { return services_.GetPanelService(); }

    // ActivityBar service 访问
    IActivityBarService& GetActivityBarService() { return services_.GetActivityBarService(); }
    const IActivityBarService& GetActivityBarService() const { return services_.GetActivityBarService(); }

    // PrimarySidebar service 访问
    IPrimarySidebarService& GetPrimarySidebarService() { return services_.GetPrimarySidebarService(); }
    const IPrimarySidebarService& GetPrimarySidebarService() const { return services_.GetPrimarySidebarService(); }

    // SecondarySidebar service 访问
    ISecondarySidebarService& GetSecondarySidebarService() { return services_.GetSecondarySidebarService(); }
    const ISecondarySidebarService& GetSecondarySidebarService() const { return services_.GetSecondarySidebarService(); }

    // 处理窗口和菜单动作
    void HandleWindowAndMenuActions(bool& done, SDL_Window* window);
    // EditorArea service 访问
    IEditorAreaService& GetEditorAreaService() { return services_.GetEditorAreaService(); }
    const IEditorAreaService& GetEditorAreaService() const { return services_.GetEditorAreaService(); }

    // 渲染编辑器区域
    void RenderEditorArea(float left_offset, float right_offset, float title_h, float status_bar_h, float panel_h, bool panel_visible, bool block_tab_clicks);
private:
    SDL_Window* window_;
    CommandService command_service_;
    ServiceCollection services_;
    TitleBarPart title_bar_part_;
    StatusBarPart status_bar_part_;
    PanelPart panel_part_;
    ActivityBarPart activity_bar_part_;
    PrimarySidebarPart primary_sidebar_part_;
    SecondarySidebarPart secondary_sidebar_part_;
    EditorAreaPart editor_area_part_;
    WorkbenchConfig config_;
    LayoutManager layout_manager_;
    WorkbenchCommandController command_controller_;
    WorkbenchRenderer renderer_;
};
