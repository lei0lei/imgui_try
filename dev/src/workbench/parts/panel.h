 #pragma once

 #include "../../services/panel_service.h"
 #include "../../ui/panel_ui.h"

 class PanelPart {
 public:
	explicit PanelPart(PanelService& service);

	 PanelResult Render(
		 float left_offset,
		 float right_offset,
		 float status_bar_h,
		 float panel_h
	 );

	PanelService& GetService();
	const PanelService& GetService() const;

 private:
	PanelService& service_;
 };
