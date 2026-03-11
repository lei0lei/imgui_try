#pragma once

#include <functional>
#include "../command/command_service.h"
#include "../services/service_interfaces.h"

enum class LayoutRegion {
    Panel,
    SecondarySidebar,
};

class WorkbenchCommandController {
public:
    WorkbenchCommandController(CommandService& command_service,
                               ITitleBarService& title_bar,
                               INotificationService& notification,
                               IPanelService& panel,
                               IActivityBarService& activity_bar,
                               IEditorAreaService& editor_area,
                               ILayoutService& layout,
                               ISecondarySidebarService& secondary_sidebar);

    void RegisterCommands(std::function<void(bool)> set_primary_sidebar_visible,
                          std::function<void()> request_exit,
                          std::function<void()> window_minimize,
                          std::function<void()> window_maximize,
                          std::function<void()> window_close,
                          std::function<void()> toggle_primary_sidebar,
                          std::function<void()> toggle_panel,
                          std::function<void()> toggle_secondary_sidebar);

    void HandleWindowAndMenuActions(bool& done);

    void SetAllowWithoutEditor(LayoutRegion region, bool value);
    bool AllowWithoutEditor(LayoutRegion region) const;

private:
    CommandService& command_service_;
    ITitleBarService& title_bar_service_;
    INotificationService& notification_;
    IPanelService& panel_;
    IActivityBarService& activity_bar_;
    IEditorAreaService& editor_area_;
    ILayoutService& layout_;
    ISecondarySidebarService& secondary_sidebar_;

    bool pending_exit_ = false;
    bool allow_panel_without_editor_ = false;
    bool allow_secondary_without_editor_ = false;
};
