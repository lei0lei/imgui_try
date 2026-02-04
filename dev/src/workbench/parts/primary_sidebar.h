 #pragma once

 #include "../../services/primary_sidebar_service.h"
 #include "../../ui/primary_sidebar_ui.h"
 #include "../../services/view_registry.h"

 class PrimarySidebarPart {
 public:
	explicit PrimarySidebarPart(PrimarySidebarService& service);

	 PrimarySidebarResult Render(
		 float activity_bar_w,
		 float title_h,
		 float status_bar_h,
		 float width,
		 ViewRegistry& view_registry,
		 SceneType mode,
		 EditorTab* active_tab,
		 ActivityBarItem active_item
	 );

	PrimarySidebarService& GetService();
	const PrimarySidebarService& GetService() const;

 private:
	PrimarySidebarService& service_;
 };
