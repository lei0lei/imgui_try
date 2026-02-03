 #include "primary_sidebar.h"

PrimarySidebarPart::PrimarySidebarPart(PrimarySidebarService& service)
	: service_(service)
{
}

 PrimarySidebarResult PrimarySidebarPart::Render(
	 ActivityBarItem active_item,
	 float activity_bar_w,
	 float title_h,
	 float status_bar_h,
	 float width)
 {
	 return DrawPrimarySidebarUI(active_item, activity_bar_w, title_h, status_bar_h, width, service_);
 }

 PrimarySidebarService& PrimarySidebarPart::GetService()
 {
	 return service_;
 }

 const PrimarySidebarService& PrimarySidebarPart::GetService() const
 {
	 return service_;
 }
