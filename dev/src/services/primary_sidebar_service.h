#pragma once

// PrimarySidebarService: 管理主侧边栏（Primary Sidebar）的状态和逻辑
// 负责选中项、可见性等状态的存储和操作
#include <string>
#include <vector>
#include "service_interfaces.h"

class PrimarySidebarService : public IPrimarySidebarService {
public:
    PrimarySidebarService();

    // 获取/设置当前选中的项目
    int GetSelectedItem() const override;
    void SetSelectedItem(int index) override;

    // 获取/设置侧边栏可见性
    bool IsVisible() const override;
    void SetVisible(bool visible) override;
    void ToggleVisible() override;

    // 可选：管理主侧边栏的项目列表
    const std::vector<std::string>& GetItems() const override;
    void SetItems(const std::vector<std::string>& items) override;

private:
    int selected_item_ = 0;
    bool visible_ = true;
    std::vector<std::string> items_;
};
