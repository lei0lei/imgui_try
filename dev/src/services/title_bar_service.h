#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>

#include "../command/command_ids.h"


// 标题栏菜单类型
enum class TitleBarMenu {
    None = 0,
    File,
    Edit,
    View,
    Help
};


// 标题栏状态结构体
struct TitleBarState {
    TitleBarMenu active_menu = TitleBarMenu::None; // 当前激活菜单
    CommandId last_command = CommandId::None; // 最近一次命令
    
    // 可扩展更多状态
};

// 标题栏服务接口，负责状态管理和事件分发
class TitleBarService {
public:
    TitleBarService();
    void Reset();

    // 菜单激活/关闭
    void SetActiveMenu(TitleBarMenu menu);
    TitleBarMenu GetActiveMenu() const;

    // 处理菜单命令
    void TriggerCommand(CommandId cmd);
    CommandId ConsumeLastCommand(); // 取出并清空最近命令

    void RequestBlockTabClicksOnce();
    bool ConsumeBlockTabClicksOnce();

    // 窗口控制/布局命令转交给 CommandService
    // 获取当前状态
    const TitleBarState& GetState() const;

private:
    TitleBarState state_;
    bool block_tab_clicks_once_ = false;
};
