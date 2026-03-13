 #pragma once

#include "../../services/service_interfaces.h"
#include "../../ui/primary_sidebar_ui.h"
#include "../../services/view_registry.h"

 class PrimarySidebarPart {
 public:
	explicit PrimarySidebarPart(IPrimarySidebarService& service);

	 PrimarySidebarResult Render(
		 float activity_bar_w,
		 float title_h,
		 float status_bar_h,
		 float width,
		 IViewRegistry& view_registry,
		 const std::string& scene_plugin_id,
		 EditorTab* active_tab,
		 ActivityBarItem active_item
	 );

	IPrimarySidebarService& GetService();
	const IPrimarySidebarService& GetService() const;

 private:
	IPrimarySidebarService& service_;
 };
