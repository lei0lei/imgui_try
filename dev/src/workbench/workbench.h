#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <memory>

typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkDevice_T* VkDevice;
typedef struct VkQueue_T* VkQueue;
typedef struct VkAllocationCallbacks VkAllocationCallbacks;

#include "../command/command_service.h"
#include "parts/title_bar.h"
#include "parts/status_bar.h"
#include "parts/activity_bar.h"
#include "parts/primary_sidebar.h"
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
    bool InitializeIconSystem(
        VkPhysicalDevice physical_device,
        VkDevice device,
        uint32_t queue_family,
        VkQueue queue,
        VkAllocationCallbacks* allocator
    );
    void ShutdownIconSystem();
    const WorkbenchMetrics& GetUiMetrics() const;
    // 更新帧统计信息
    void UpdateFrameStats(float fps);
    // 布局渲染
    void RenderActivityBar(const WorkbenchMetrics& metrics);
    void RenderTitleBar(const WorkbenchMetrics& metrics);
    void RenderStatusBar(const WorkbenchMetrics& metrics);
    void RenderPrimarySidebar(const WorkbenchMetrics& metrics);
    void RenderEditorArea(const WorkbenchMetrics& metrics, const LayoutInfo& layout);
    LayoutInfo ComputeLayout(const WorkbenchMetrics& metrics);
    // 标题栏事件已改为命令处理

    // 新增：primary sidebar 显示状态
    bool IsPrimarySidebarVisible() const { return services_.GetLayoutService().IsPrimarySidebarVisible(); }
    void TogglePrimarySidebar();
    void SetPrimarySidebarVisible(bool visible);

    // 通知 service 访问
    INotificationService& GetNotificationService() { return services_.GetNotificationService(); }

    // 布局 service 访问
    ILayoutService& GetLayoutService() { return services_.GetLayoutService(); }
    const ILayoutService& GetLayoutService() const { return services_.GetLayoutService(); }

    // ActivityBar service 访问
    IActivityBarService& GetActivityBarService() { return services_.GetActivityBarService(); }
    const IActivityBarService& GetActivityBarService() const { return services_.GetActivityBarService(); }

    // PrimarySidebar service 访问
    IPrimarySidebarService& GetPrimarySidebarService() { return services_.GetPrimarySidebarService(); }
    const IPrimarySidebarService& GetPrimarySidebarService() const { return services_.GetPrimarySidebarService(); }

    // 处理窗口和菜单动作
    void HandleWindowAndMenuActions(bool& done, SDL_Window* window);
    // EditorArea service 访问
    IEditorAreaService& GetEditorAreaService() { return services_.GetEditorAreaService(); }
    const IEditorAreaService& GetEditorAreaService() const { return services_.GetEditorAreaService(); }

private:
    SDL_Window* window_;
    CommandService command_service_;
    ServiceCollection services_;
    TitleBarPart title_bar_part_;
    StatusBarPart status_bar_part_;
    ActivityBarPart activity_bar_part_;
    PrimarySidebarPart primary_sidebar_part_;
    EditorAreaPart editor_area_part_;
    WorkbenchConfig config_;
    LayoutManager layout_manager_;
    WorkbenchCommandController command_controller_;
    WorkbenchRenderer renderer_;
};
