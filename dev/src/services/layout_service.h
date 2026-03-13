#pragma once
#include "service_interfaces.h"

struct LayoutState {
    bool primary_sidebar_visible = true;
};

class LayoutService : public ILayoutService {
public:
    LayoutService() = default;

    bool IsPrimarySidebarVisible() const override { return state_.primary_sidebar_visible; }
    void SetPrimarySidebarVisible(bool visible) override { state_.primary_sidebar_visible = visible; }
    void TogglePrimarySidebar() override { state_.primary_sidebar_visible = !state_.primary_sidebar_visible; }

    const LayoutState& GetState() const override { return state_; }

private:
    LayoutState state_;
};
