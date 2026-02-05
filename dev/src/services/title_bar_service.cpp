/**
 * @file title_bar_service.cpp
 * @brief 标题栏服务的实现，用于窗口标题和控件
 * @author Your Name
 * @date 2026-02-05
 */

#include "title_bar_service.h"

TitleBarService::TitleBarService() {
    Reset();
}

void TitleBarService::Reset() {
    state_ = TitleBarState();
    block_tab_clicks_once_ = false;
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

void TitleBarService::RequestBlockTabClicksOnce() {
    block_tab_clicks_once_ = true;
}

bool TitleBarService::ConsumeBlockTabClicksOnce() {
    bool value = block_tab_clicks_once_;
    block_tab_clicks_once_ = false;
    return value;
}

const TitleBarState& TitleBarService::GetState() const {
    return state_;
}
