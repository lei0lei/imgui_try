#pragma once
#include <string>

// 状态栏状态结构体
struct StatusBarState {
    std::string message;
    float progress = -1.0f; // <0 表示无进度条
};

// 状态栏服务，负责状态管理和事件分发
class StatusBarService {
public:
    StatusBarService();
    void SetMessage(const std::string& msg);
    void SetProgress(float value); // 0~1
    void Clear();
    const StatusBarState& GetState() const;
private:
    StatusBarState state_;
};
