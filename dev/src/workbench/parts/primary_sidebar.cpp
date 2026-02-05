 #include "primary_sidebar.h"

PrimarySidebarPart::PrimarySidebarPart(IPrimarySidebarService& service)
	: service_(service)
{
}

 PrimarySidebarResult PrimarySidebarPart::Render(
	 float activity_bar_w,
	 float title_h,
	 float status_bar_h,
	 float width,
	 IViewRegistry& view_registry,
	 SceneType mode,
	 EditorTab* active_tab,
	 ActivityBarItem active_item)
 {
	 PrimarySidebarViewModel vm{};
	 vm.active_view = view_registry.GetActiveView(mode, ViewContainer::PrimarySidebar);
	 vm.fallback_view = UI::GetDefaultPrimaryView(active_item);
	 vm.is_visible = service_.IsVisible();
	 vm.active_item = active_item;
	 return DrawPrimarySidebarUI(activity_bar_w, title_h, status_bar_h, width, vm, active_tab);
 }

 IPrimarySidebarService& PrimarySidebarPart::GetService()
 {
	 return service_;
 }

 const IPrimarySidebarService& PrimarySidebarPart::GetService() const
 {
	 return service_;
 }
