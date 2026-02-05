 #pragma once

#include "../../services/service_interfaces.h"
#include "../../ui/panel_ui.h"
#include "../../services/view_registry.h"

 class PanelPart {
 public:
	explicit PanelPart(IPanelService& service);

	 PanelResult Render(
		 float left_offset,
		 float right_offset,
		 float status_bar_h,
		 float panel_h,
		 IViewRegistry& view_registry,
		 SceneType mode,
		 EditorTab* active_tab
	 );

	IPanelService& GetService();
	const IPanelService& GetService() const;

 private:
	IPanelService& service_;
 };
