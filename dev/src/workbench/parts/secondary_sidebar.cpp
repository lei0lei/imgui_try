 #include "secondary_sidebar.h"

SecondarySidebarPart::SecondarySidebarPart(SecondarySidebarService& service)
	: service_(service)
{
}

 SecondarySidebarResult SecondarySidebarPart::Render(
	 float title_h,
	 float status_bar_h,
	 float panel_h,
	 float width)
 {
	 return DrawSecondarySidebarUI(title_h, status_bar_h, panel_h, width, service_);
 }

 SecondarySidebarService& SecondarySidebarPart::GetService()
 {
	 return service_;
 }

 const SecondarySidebarService& SecondarySidebarPart::GetService() const
 {
	 return service_;
 }
