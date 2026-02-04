 #include "secondary_sidebar.h"

SecondarySidebarPart::SecondarySidebarPart(SecondarySidebarService& service)
	: service_(service)
{
}

 SecondarySidebarResult SecondarySidebarPart::Render(
	 float title_h,
	 float status_bar_h,
	 float panel_h,
	 float width,
	 ViewRegistry& view_registry,
	 SceneType mode,
	 EditorTab* active_tab)
 {
	 return DrawSecondarySidebarUI(title_h, status_bar_h, panel_h, width, service_, view_registry, mode, active_tab);
 }

 SecondarySidebarService& SecondarySidebarPart::GetService()
 {
	 return service_;
 }

 const SecondarySidebarService& SecondarySidebarPart::GetService() const
 {
	 return service_;
 }
