/**
 * @file workbench.cpp
 * @brief 主工作台实现，协调所有UI组件
 * @author Your Name
 * @date 2026-02-05
 */

#include "workbench.h"
#include "../command/command_handlers.h"
#include "../ui/activity_bar_ui.h"
#include <vector>
#include <cstdio>

bool Workbench::InitializeIconSystem(
    VkPhysicalDevice physical_device,
    VkDevice device,
    uint32_t queue_family,
    VkQueue queue,
    VkAllocationCallbacks* allocator)
{
    return InitializeActivityBarIconSystem(physical_device, device, queue_family, queue, allocator);
}

void Workbench::ShutdownIconSystem()
{
    ShutdownActivityBarIconSystem();
}

/**
 * @brief 渲染活动栏
 * @param metrics 工作台度量信息
 */
void Workbench::RenderActivityBar(const WorkbenchMetrics& metrics)
{
    renderer_.RenderActivityBar(metrics);
}

/**
 * @brief 渲染标题栏
 * @param metrics 工作台度量信息
 */
void Workbench::RenderTitleBar(const WorkbenchMetrics& metrics)
{
    renderer_.RenderTitleBar(metrics, window_);
}

/**
 * @brief 渲染状态栏
 * @param metrics 工作台度量信息
 */
void Workbench::RenderStatusBar(const WorkbenchMetrics& metrics)
{
    renderer_.RenderStatusBar(metrics, window_);
}

/**
 * @brief 渲染主侧边栏
 * @param metrics 工作台度量信息
 */
void Workbench::RenderPrimarySidebar(const WorkbenchMetrics& metrics)
{
    renderer_.RenderPrimarySidebar(metrics);
}

/**
 * @brief 渲染编辑器区域
 * @param metrics 工作台度量信息
 * @param layout 布局信息
 */
void Workbench::RenderEditorArea(const WorkbenchMetrics& metrics, const LayoutInfo& layout)
{
    renderer_.RenderEditorArea(metrics, layout);
}

/**
 * @brief 渲染面板
 * @param metrics 工作台度量信息
 * @param layout 布局信息
 */
void Workbench::RenderPanel(const WorkbenchMetrics& metrics, const LayoutInfo& layout)
{
    renderer_.RenderPanel(metrics, layout);
}

/**
 * @brief 渲染辅助侧边栏
 * @param metrics 工作台度量信息
 * @param layout 布局信息
 */
void Workbench::RenderSecondarySidebar(const WorkbenchMetrics& metrics, const LayoutInfo& layout)
{
    renderer_.RenderSecondarySidebar(metrics, layout);
}

/**
 * @brief 渲染面板和辅助侧边栏
 * @param metrics 工作台度量信息
 * @param layout_before 之前的布局信息
 */
void Workbench::RenderPanelAndSecondary(const WorkbenchMetrics& metrics, const LayoutInfo& layout_before)
{
    renderer_.RenderPanelAndSecondary(metrics, layout_before);
}

/**
 * @brief 计算布局信息
 * @param metrics 工作台度量信息
 * @return LayoutInfo 计算得到的布局信息
 */
LayoutInfo Workbench::ComputeLayout(const WorkbenchMetrics& metrics)
{
     return renderer_.ComputeLayout(metrics);
}

/**
 * @brief 获取UI度量信息
 * @return const WorkbenchMetrics& UI度量信息的常量引用
 */
const WorkbenchMetrics& Workbench::GetUiMetrics() const
{
    return renderer_.GetUiMetrics();
}

/**
 * @brief 更新帧统计信息
 * @param fps 当前帧率
 */
void Workbench::UpdateFrameStats(float fps)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "FPS: %.1f", fps);
    services_.GetNotificationService().SetMessage(buf);
}

/**
 * @brief Workbench构造函数
 * @param window SDL窗口指针，用于渲染上下文
 *
 * 初始化所有UI部件、服务和渲染器，设置默认状态。
 */
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

/**
 * @brief 设置主侧边栏可见性
 * @param visible 是否可见
 */
void Workbench::SetPrimarySidebarVisible(bool visible)
{
    services_.GetLayoutService().SetPrimarySidebarVisible(visible);
}

/**
 * @brief 切换主侧边栏可见性
 */
void Workbench::TogglePrimarySidebar()
{
    SetPrimarySidebarVisible(!services_.GetLayoutService().IsPrimarySidebarVisible());
}

/**
 * @brief 关闭辅助侧边栏
 */
void Workbench::CloseSecondarySidebar()
{
    services_.GetLayoutService().SetSecondarySidebarVisible(false);
    secondary_sidebar_part_.GetService().SetVisible(false);
}

/**
 * @brief 处理窗口和菜单动作
 * @param done 引用参数，指示是否完成
 * @param window SDL窗口指针
 */
void Workbench::HandleWindowAndMenuActions(bool& done, SDL_Window* window) {
    (void)window;
    command_controller_.HandleWindowAndMenuActions(done);
}
