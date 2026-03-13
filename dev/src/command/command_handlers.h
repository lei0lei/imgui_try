#pragma once

#include <functional>
#include "command_service.h"
#include "../services/service_interfaces.h"

struct CommandHandlersContext {
    INotificationService& notification;
    IActivityBarService& activity_bar;
    IEditorAreaService& editor_area;
    std::function<void(bool)> set_primary_sidebar_visible;
    std::function<void()> request_exit;
    std::function<void()> window_minimize;
    std::function<void()> window_maximize;
    std::function<void()> window_close;
    std::function<void()> toggle_primary_sidebar;
};

void RegisterWorkbenchCommands(CommandService& service, CommandHandlersContext ctx);
