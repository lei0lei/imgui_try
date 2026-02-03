#include "title_bar_service.h"

TitleBarService::TitleBarService() {
    Reset();
}

void TitleBarService::Reset() {
    state_ = TitleBarState();
}

void TitleBarService::SetActiveMenu(TitleBarMenu menu) {
    state_.active_menu = menu;
}

TitleBarMenu TitleBarService::GetActiveMenu() const {
    return state_.active_menu;
}

void TitleBarService::TriggerCommand(CommandId cmd) {
    state_.last_command = cmd;
}

CommandId TitleBarService::ConsumeLastCommand() {
    CommandId cmd = state_.last_command;
    state_.last_command = CommandId::None;
    return cmd;
}

const TitleBarState& TitleBarService::GetState() const {
    return state_;
}
