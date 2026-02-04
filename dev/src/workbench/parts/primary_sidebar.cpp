 #include "primary_sidebar.h"

PrimarySidebarPart::PrimarySidebarPart(PrimarySidebarService& service)
	: service_(service)
{
}

 PrimarySidebarResult PrimarySidebarPart::Render(
	 float activity_bar_w,
	 float title_h,
	 float status_bar_h,
	 float width,
	 ViewRegistry& view_registry,
	 SceneType mode,
	 EditorTab* active_tab,
	 ActivityBarItem active_item)
 {
	 return DrawPrimarySidebarUI(activity_bar_w, title_h, status_bar_h, width, service_, view_registry, mode, active_tab, active_item);
 }

 PrimarySidebarService& PrimarySidebarPart::GetService()
 {
	 return service_;
 }

 const PrimarySidebarService& PrimarySidebarPart::GetService() const
 {
	 return service_;
 }
