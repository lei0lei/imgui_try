#include "workbench_command_controller.h"
#include "../command/command_handlers.h"
#include "../services/editor_area_service.h"
#include <utility>

WorkbenchCommandController::WorkbenchCommandController(CommandService& command_service,
                                                       ITitleBarService& title_bar,
                                                       INotificationService& notification,
                                                       IPanelService& panel,
                                                       IActivityBarService& activity_bar,
                                                       IEditorAreaService& editor_area,
                                                       ILayoutService& layout,
                                                       ISecondarySidebarService& secondary_sidebar)
    : command_service_(command_service),
      title_bar_service_(title_bar),
      notification_(notification),
      panel_(panel),
      activity_bar_(activity_bar),
      editor_area_(editor_area),
      layout_(layout),
      secondary_sidebar_(secondary_sidebar)
{
}

void WorkbenchCommandController::RegisterCommands(std::function<void(bool)> set_primary_sidebar_visible,
                                                  std::function<void()> request_exit,
                                                  std::function<void()> window_minimize,
                                                  std::function<void()> window_maximize,
                                                  std::function<void()> window_close,
                                                  std::function<void()> toggle_primary_sidebar,
                                                  std::function<void()> toggle_panel,
                                                  std::function<void()> toggle_secondary_sidebar)
{
    CommandHandlersContext ctx{
        notification_,
        panel_,
        activity_bar_,
        editor_area_,
        layout_,
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
        std::move(toggle_primary_sidebar),
        [this, toggle_panel]() {
            if (auto* tab = editor_area_.GetActiveTab()) {
                tab->panel_visible = !tab->panel_visible;
                layout_.SetPanelVisible(tab->panel_visible);
                allow_panel_without_editor_ = false;
            } else {
                bool will_show = !layout_.IsPanelVisible();
                layout_.TogglePanel();
                allow_panel_without_editor_ = will_show;
            }
            if (toggle_panel)
                toggle_panel();
        },
        [this, toggle_secondary_sidebar]() {
            if (auto* tab = editor_area_.GetActiveTab()) {
                tab->secondary_sidebar_visible = !tab->secondary_sidebar_visible;
                layout_.SetSecondarySidebarVisible(tab->secondary_sidebar_visible);
                allow_secondary_without_editor_ = false;
            } else {
                bool will_show = !layout_.IsSecondarySidebarVisible();
                layout_.ToggleSecondarySidebar();
                allow_secondary_without_editor_ = will_show;
            }
            secondary_sidebar_.SetVisible(layout_.IsSecondarySidebarVisible());
            if (toggle_secondary_sidebar)
                toggle_secondary_sidebar();
        }
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

void WorkbenchCommandController::SetAllowPanelWithoutEditor(bool value)
{
    allow_panel_without_editor_ = value;
}

void WorkbenchCommandController::SetAllowSecondaryWithoutEditor(bool value)
{
    allow_secondary_without_editor_ = value;
}

bool WorkbenchCommandController::AllowPanelWithoutEditor() const
{
    return allow_panel_without_editor_;
}

bool WorkbenchCommandController::AllowSecondaryWithoutEditor() const
{
    return allow_secondary_without_editor_;
}
