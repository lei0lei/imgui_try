#pragma once
#include "service_interfaces.h"

struct LayoutState {
    bool primary_sidebar_visible = true;
    bool secondary_sidebar_visible = false;
    bool panel_visible = false;
};

class LayoutService : public ILayoutService {
public:
    LayoutService() = default;

    bool IsPrimarySidebarVisible() const override { return state_.primary_sidebar_visible; }
    void SetPrimarySidebarVisible(bool visible) override { state_.primary_sidebar_visible = visible; }
    void TogglePrimarySidebar() override { state_.primary_sidebar_visible = !state_.primary_sidebar_visible; }

    bool IsSecondarySidebarVisible() const override { return state_.secondary_sidebar_visible; }
    void SetSecondarySidebarVisible(bool visible) override { state_.secondary_sidebar_visible = visible; }
    void ToggleSecondarySidebar() override { state_.secondary_sidebar_visible = !state_.secondary_sidebar_visible; }

    bool IsPanelVisible() const override { return state_.panel_visible; }
    void SetPanelVisible(bool visible) override { state_.panel_visible = visible; }
    void TogglePanel() override { state_.panel_visible = !state_.panel_visible; }

    const LayoutState& GetState() const override { return state_; }

private:
    LayoutState state_;
};
