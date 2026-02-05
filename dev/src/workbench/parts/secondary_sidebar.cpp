 #include "secondary_sidebar.h"

SecondarySidebarPart::SecondarySidebarPart(ISecondarySidebarService& service)
	: service_(service)
{
}

 SecondarySidebarResult SecondarySidebarPart::Render(
	 float title_h,
	 float status_bar_h,
	 float panel_h,
	 float width,
	 IViewRegistry& view_registry,
	 SceneType mode,
	 EditorTab* active_tab)
 {
	 SecondarySidebarViewModel vm{};
	 vm.is_visible = service_.IsVisible();
	 vm.has_active_tab = (active_tab != nullptr);
	 vm.active_view = view_registry.GetActiveView(mode, ViewContainer::SecondarySidebar);
	 return DrawSecondarySidebarUI(title_h, status_bar_h, panel_h, width, vm, active_tab);
 }

 ISecondarySidebarService& SecondarySidebarPart::GetService()
 {
	 return service_;
 }

 const ISecondarySidebarService& SecondarySidebarPart::GetService() const
 {
	 return service_;
 }
