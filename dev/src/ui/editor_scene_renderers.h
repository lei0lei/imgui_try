#pragma once

#include "imgui.h"
#include "../services/editor_area_service.h"
#include "editor_scene_registry.h"

namespace UI {

void DrawScene2D(ImVec2 content_min, ImVec2 content_max, const EditorTab& tab);
void DrawScene3D(ImVec2 content_min, ImVec2 content_max, const EditorTab& tab);
void DrawNodeEditorCanvas(ImVec2 content_min, ImVec2 content_max, EditorTab& tab);

void RegisterDefaultEditorSceneRenderers(EditorSceneRegistry& registry);

} // namespace UI
