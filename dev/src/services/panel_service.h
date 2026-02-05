#pragma once
#include <string>
#include <vector>
#include "service_interfaces.h"

// 面板类型（可扩展）
enum class PanelTab {
    Terminal,
    Output,
    Problems,
    DebugConsole
};

struct PanelState {
    bool visible = false;
    PanelTab active_tab = PanelTab::Terminal;
    std::vector<PanelTab> tabs = {PanelTab::Terminal, PanelTab::Output, PanelTab::Problems, PanelTab::DebugConsole};
};

// Service 负责管理底部面板的状态
class PanelService : public IPanelService {
public:
    PanelService();
    void Reset();

    void SetVisible(bool v) override;
    bool IsVisible() const override;

    void ToggleVisible() override;

    void SetActiveTab(PanelTab tab) override;
    PanelTab GetActiveTab() const override;
    const std::vector<PanelTab>& GetTabs() const;

    PanelState& GetState();
    const PanelState& GetState() const;

private:
    PanelState state_;
};
