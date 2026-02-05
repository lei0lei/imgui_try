#pragma once

// ActivityBarService: 管理 Activity Bar（左侧活动栏）的状态和逻辑
// 负责选中项、可见性等状态的存储和操作
#include <string>
#include <vector>
#include "service_interfaces.h"

class ActivityBarService : public IActivityBarService {
public:
    ActivityBarService();

    // 获取/设置当前选中的项目
    int GetSelectedItem() const override;
    void SetSelectedItem(int index) override;


    // 可选：管理 ActivityBar 的项目列表
    const std::vector<std::string>& GetItems() const override;
    void SetItems(const std::vector<std::string>& items) override;

private:
    int selected_item_ = 0;
    std::vector<std::string> items_;
};
