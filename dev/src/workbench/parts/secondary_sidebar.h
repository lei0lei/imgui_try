 #pragma once

 #include "../../services/secondary_sidebar_service.h"
 #include "../../ui/secondary_sidebar_ui.h"
 #include "../../services/view_registry.h"

 class SecondarySidebarPart {
 public:
	explicit SecondarySidebarPart(SecondarySidebarService& service);

	 SecondarySidebarResult Render(
		 float title_h,
		 float status_bar_h,
		 float panel_h,
		 float width,
		 ViewRegistry& view_registry,
		 SceneType mode,
		 EditorTab* active_tab
	 );

	SecondarySidebarService& GetService();
	const SecondarySidebarService& GetService() const;

 private:
	SecondarySidebarService& service_;
 };
