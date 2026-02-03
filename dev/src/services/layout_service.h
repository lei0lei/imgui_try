#pragma once

struct LayoutState {
    bool primary_sidebar_visible = true;
    bool secondary_sidebar_visible = false;
    bool panel_visible = false;
};

class LayoutService {
public:
    LayoutService() = default;

    bool IsPrimarySidebarVisible() const { return state_.primary_sidebar_visible; }
    void SetPrimarySidebarVisible(bool visible) { state_.primary_sidebar_visible = visible; }
    void TogglePrimarySidebar() { state_.primary_sidebar_visible = !state_.primary_sidebar_visible; }

    bool IsSecondarySidebarVisible() const { return state_.secondary_sidebar_visible; }
    void SetSecondarySidebarVisible(bool visible) { state_.secondary_sidebar_visible = visible; }
    void ToggleSecondarySidebar() { state_.secondary_sidebar_visible = !state_.secondary_sidebar_visible; }

    bool IsPanelVisible() const { return state_.panel_visible; }
    void SetPanelVisible(bool visible) { state_.panel_visible = visible; }
    void TogglePanel() { state_.panel_visible = !state_.panel_visible; }

    const LayoutState& GetState() const { return state_; }

private:
    LayoutState state_;
};
