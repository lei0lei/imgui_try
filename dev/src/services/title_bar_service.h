#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>

#include "../command/command_ids.h"
#include "../core/workbench_types.h"
#include "service_interfaces.h"


// 标题栏状态结构体
struct TitleBarState {
    TitleBarMenu active_menu = TitleBarMenu::None; // 当前激活菜单
    CommandId last_command = CommandId::None; // 最近一次命令
    
    // 可扩展更多状态
};

// 标题栏服务接口，负责状态管理和事件分发
class TitleBarService : public ITitleBarService {
public:
    TitleBarService();
    void Reset();

    // 菜单激活/关闭
    void SetActiveMenu(TitleBarMenu menu) override;
    TitleBarMenu GetActiveMenu() const override;

    // 处理菜单命令
    void TriggerCommand(CommandId cmd) override;
    CommandId ConsumeLastCommand() override; // 取出并清空最近命令

    void RequestBlockTabClicksOnce() override;
    bool ConsumeBlockTabClicksOnce() override;

    // 窗口控制/布局命令转交给 CommandService
    // 获取当前状态
    const TitleBarState& GetState() const;

private:
    TitleBarState state_;
    bool block_tab_clicks_once_ = false;
};
