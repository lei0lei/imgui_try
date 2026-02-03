 #pragma once

 #include "../../services/primary_sidebar_service.h"
 #include "../../ui/primary_sidebar_ui.h"

 class PrimarySidebarPart {
 public:
	explicit PrimarySidebarPart(PrimarySidebarService& service);

	 PrimarySidebarResult Render(
		 ActivityBarItem active_item,
		 float activity_bar_w,
		 float title_h,
		 float status_bar_h,
		 float width
	 );

	PrimarySidebarService& GetService();
	const PrimarySidebarService& GetService() const;

 private:
	PrimarySidebarService& service_;
 };
