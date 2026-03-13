 #pragma once

#include "../../services/service_interfaces.h"
#include "../../ui/secondary_sidebar_ui.h"
#include "../../services/view_registry.h"

 class SecondarySidebarPart {
 public:
	explicit SecondarySidebarPart(ISecondarySidebarService& service);

	 SecondarySidebarResult Render(
		 float title_h,
		 float status_bar_h,
		 float panel_h,
		 float width,
		 IViewRegistry& view_registry,
		 const std::string& scene_plugin_id,
		 EditorTab* active_tab
	 );

	ISecondarySidebarService& GetService();
	const ISecondarySidebarService& GetService() const;

 private:
	ISecondarySidebarService& service_;
 };
