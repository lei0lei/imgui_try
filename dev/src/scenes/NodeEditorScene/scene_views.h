#pragma once

#include "imgui.h"
#include "../../services/editor_area_service.h"

namespace Scenes::NodeEditorViews {

void RenderLibrary(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab);
void RenderOutline(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab);
void RenderProperties(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab);
void RenderPanelOutput(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab);

} // namespace Scenes::NodeEditorViews
