#pragma once

#include "imgui.h"
#include "../scene_context.h"
#include "../../services/editor_area_service.h"

namespace Scenes::NodeEditorViews {

void RenderCanvas(const SceneContext& ctx, EditorTab& tab);
void RenderLibrary(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab);
void RenderOutline(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab);
void RenderProperties(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab);
void RenderPanelOutput(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab);

} // namespace Scenes::NodeEditorViews
