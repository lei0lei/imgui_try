/**
 * @file editor_scene_renderers.cpp
 * @brief 各种编辑器场景渲染器的实现，用于不同文件类型
 * @author Your Name
 * @date 2026-02-05
 */

#include "editor_scene_renderers.h"
#include "../scenes/2DScene/scene_views.h"
#include "../scenes/3DScene/scene_views.h"
#include "../scenes/NodeEditorScene/scene_views.h"

namespace UI {

namespace {

void DrawScene2DAdapter(ImVec2 content_min, ImVec2 content_max, EditorTab& tab)
{
    DrawScene2D(content_min, content_max, tab);
}

void DrawScene3DAdapter(ImVec2 content_min, ImVec2 content_max, EditorTab& tab)
{
    DrawScene3D(content_min, content_max, tab);
}

} // namespace

void RegisterDefaultEditorSceneRenderers(EditorSceneRegistry& registry)
{
    registry.RegisterRenderer(SceneType::Scene2D, DrawScene2DAdapter);
    registry.RegisterRenderer(SceneType::Scene3D, DrawScene3DAdapter);
    registry.RegisterRenderer(SceneType::NodeEditor, DrawNodeEditorCanvas);
}

void DrawScene2D(ImVec2 content_min, ImVec2 content_max, const EditorTab& tab)
{
    Scenes::Scene2DViews::RenderCanvas(content_min, content_max, tab);
}

void DrawScene3D(ImVec2 content_min, ImVec2 content_max, const EditorTab& tab)
{
    Scenes::Scene3DViews::RenderCanvas(content_min, content_max, tab);
}

void DrawNodeEditorCanvas(ImVec2 content_min, ImVec2 content_max, EditorTab& tab)
{
    Scenes::NodeEditorViews::RenderCanvas(content_min, content_max, tab);
}

} // namespace UI
