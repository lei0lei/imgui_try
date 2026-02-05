#pragma once

#include <functional>
#include "command_service.h"
#include "../services/service_interfaces.h"

struct CommandHandlersContext {
    INotificationService& notification;
    IPanelService& panel;
    IActivityBarService& activity_bar;
    IEditorAreaService& editor_area;
    ILayoutService& layout;
    std::function<void(bool)> set_primary_sidebar_visible;
    std::function<void()> request_exit;
    std::function<void()> window_minimize;
    std::function<void()> window_maximize;
    std::function<void()> window_close;
    std::function<void()> toggle_primary_sidebar;
    std::function<void()> toggle_panel;
    std::function<void()> toggle_secondary_sidebar;
};

void RegisterWorkbenchCommands(CommandService& service, CommandHandlersContext ctx);
