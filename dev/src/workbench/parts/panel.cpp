 #include "panel.h"

PanelPart::PanelPart(IPanelService& service)
	: service_(service)
{
}

 PanelResult PanelPart::Render(
	 float left_offset,
	 float right_offset,
	 float status_bar_h,
	 float panel_h,
	 IViewRegistry& view_registry,
	 SceneType mode,
	 EditorTab* active_tab)
 {
	 PanelViewModel vm{};
	 vm.views = &view_registry.GetViews(mode, ViewContainer::Panel);
	 vm.active_index = view_registry.GetActiveViewIndex(mode, ViewContainer::Panel);
	 vm.on_select_tab = [&view_registry, mode](int index) {
		 view_registry.SetActiveViewIndex(mode, ViewContainer::Panel, index);
	 };
	 vm.active_view = view_registry.GetActiveView(mode, ViewContainer::Panel);
	 vm.has_active_tab = (active_tab != nullptr);
	 return DrawPanelUI(left_offset, right_offset, status_bar_h, panel_h, vm, active_tab);
 }

 IPanelService& PanelPart::GetService()
 {
	 return service_;
 }

 const IPanelService& PanelPart::GetService() const
 {
	 return service_;
 }
