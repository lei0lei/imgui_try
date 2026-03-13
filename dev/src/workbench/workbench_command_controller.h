#pragma once

#include <functional>
#include "../command/command_service.h"
#include "../services/service_interfaces.h"

class WorkbenchCommandController {
public:
    WorkbenchCommandController(CommandService& command_service,
                               ITitleBarService& title_bar,
                               INotificationService& notification,
                               IActivityBarService& activity_bar,
                               IEditorAreaService& editor_area);

    void RegisterCommands(std::function<void(bool)> set_primary_sidebar_visible,
                          std::function<void()> request_exit,
                          std::function<void()> window_minimize,
                          std::function<void()> window_maximize,
                          std::function<void()> window_close,
                          std::function<void()> toggle_primary_sidebar);

    void HandleWindowAndMenuActions(bool& done);

private:
    CommandService& command_service_;
    ITitleBarService& title_bar_service_;
    INotificationService& notification_;
    IActivityBarService& activity_bar_;
    IEditorAreaService& editor_area_;

    bool pending_exit_ = false;
};
