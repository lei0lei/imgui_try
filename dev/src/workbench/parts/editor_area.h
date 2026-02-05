 #pragma once

 #include <memory>
#include "../../services/service_interfaces.h"
#include "../../services/editor_area_service.h"
 #include "../../ui/editor_area_ui.h"

 class EditorAreaPart {
 public:
	explicit EditorAreaPart(IEditorAreaService& service);

	 void Render(
		 float left_offset,
		 float right_offset,
		 float title_h,
		 float status_bar_h,
		 float panel_h,
		 bool panel_visible,
		 bool block_tab_clicks
	 );

	IEditorAreaService& GetService();
	const IEditorAreaService& GetService() const;

 private:
	IEditorAreaService& service_;
	 std::unique_ptr<UI::EditorArea> ui_;
 };
