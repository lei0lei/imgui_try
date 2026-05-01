#pragma once

#include "imgui.h"
#include "../scene_context.h"
#include "../../services/editor_area_service.h"

namespace Scenes::Scene3DViews {

void RenderCanvas(const SceneContext& ctx, EditorTab& tab);
void RenderHierarchy(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab);
void RenderOutline(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab);
void RenderProperties(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab);
void RenderPanelOutput(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab);

} // namespace Scenes::Scene3DViews
