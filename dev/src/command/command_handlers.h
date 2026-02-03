#pragma once

#include <functional>
#include "command_service.h"
#include "../services/notification_service.h"
#include "../services/panel_service.h"
#include "../services/activity_bar_service.h"
#include "../services/editor_area_service.h"
#include "../services/layout_service.h"

struct CommandHandlersContext {
    NotificationService& notification;
    PanelService& panel;
    ActivityBarService& activity_bar;
    EditorAreaService& editor_area;
    LayoutService& layout;
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
