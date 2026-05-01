 #pragma once

 #include <memory>
#include "../../services/service_interfaces.h"
#include "../../services/editor_area_service.h"
#include "../../services/event_bus.h"
 #include "../../ui/editor_area_ui.h"

 class EditorAreaPart {
 public:
	EditorAreaPart(IEditorAreaService& service, IEventBus& event_bus);

	 void Render(
		 float left_offset,
		 float right_offset,
		 float title_h,
		 float status_bar_h,
		 bool block_tab_clicks
	 );

	IEditorAreaService& GetService();
	const IEditorAreaService& GetService() const;

 private:
	IEditorAreaService& service_;
	IEventBus& event_bus_;
	 std::unique_ptr<UI::EditorArea> ui_;
 };
