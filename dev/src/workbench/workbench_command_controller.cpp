/**
 * @file workbench_command_controller.cpp
 * @brief 工作台专用命令的命令控制器
 * @author Your Name
 * @date 2026-02-05
 */

#include "workbench_command_controller.h"
#include "../command/command_handlers.h"
#include "../services/editor_area_service.h"
#include <utility>

WorkbenchCommandController::WorkbenchCommandController(CommandService& command_service,
                                                       ITitleBarService& title_bar,
                                                       INotificationService& notification,
                                                       IActivityBarService& activity_bar,
                                     IEditorAreaService& editor_area)
    : command_service_(command_service),
      title_bar_service_(title_bar),
      notification_(notification),
      activity_bar_(activity_bar),
    editor_area_(editor_area)
{
}

void WorkbenchCommandController::RegisterCommands(std::function<void(bool)> set_primary_sidebar_visible,
                                                  std::function<void()> request_exit,
                                                  std::function<void()> window_minimize,
                                                  std::function<void()> window_maximize,
                                                  std::function<void()> window_close,
                                                  std::function<void()> toggle_primary_sidebar)
{
    CommandHandlersContext ctx{
        notification_,
        activity_bar_,
        editor_area_,
        std::move(set_primary_sidebar_visible),
        [this, request_exit]() {
            pending_exit_ = true;
            if (request_exit)
                request_exit();
        },
        std::move(window_minimize),
        std::move(window_maximize),
        [this, window_close]() {
            pending_exit_ = true;
            if (window_close)
                window_close();
        },
        std::move(toggle_primary_sidebar)
    };

    RegisterWorkbenchCommands(command_service_, ctx);
}

void WorkbenchCommandController::HandleWindowAndMenuActions(bool& done)
{
    if (pending_exit_) {
        done = true;
        pending_exit_ = false;
    }

    CommandId cmd = title_bar_service_.ConsumeLastCommand();
    if (cmd != CommandId::None)
        command_service_.Execute(cmd);
}

