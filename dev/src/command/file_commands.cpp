/**
 * @file file_commands.cpp
 * @brief 文件相关命令的实现（打开、保存等）
 * @author Your Name
 * @date 2026-02-05
 */

#include "file_commands.h"
#include "imgui.h"
#include "../services/editor_area_service.h"
#include "../ui/view_registry_defaults.h"

void RegisterFileCommands(CommandService& service, CommandHandlersContext ctx)
{
	service.Register(CommandId::FileNew2D, [ctx]() mutable {
		EditorTab new_tab;
		new_tab.name = "2D-Scene-" + std::to_string(ctx.editor_area.GetTabs().size() + 1);
		new_tab.path = "";
		new_tab.modified = false;
		new_tab.active = true;
		new_tab.scene_type = SceneType::Scene2D;
		new_tab.secondary_active_view_id = UI::GetDefaultSecondaryView(SceneType::Scene2D).id;
		new_tab.panel_active_view_id = UI::GetDefaultPanelView(SceneType::Scene2D).id;
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileNew, [ctx]() mutable {
		EditorTab new_tab;
		new_tab.name = "3D-Scene-" + std::to_string(ctx.editor_area.GetTabs().size() + 1);
		new_tab.path = "";
		new_tab.modified = false;
		new_tab.active = true;
		new_tab.scene_type = SceneType::Scene3D;
		new_tab.secondary_active_view_id = UI::GetDefaultSecondaryView(SceneType::Scene3D).id;
		new_tab.panel_active_view_id = UI::GetDefaultPanelView(SceneType::Scene3D).id;
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileNew3D, [ctx]() mutable {
		EditorTab new_tab;
		new_tab.name = "3D-Scene-" + std::to_string(ctx.editor_area.GetTabs().size() + 1);
		new_tab.path = "";
		new_tab.modified = false;
		new_tab.active = true;
		new_tab.scene_type = SceneType::Scene3D;
		new_tab.secondary_active_view_id = UI::GetDefaultSecondaryView(SceneType::Scene3D).id;
		new_tab.panel_active_view_id = UI::GetDefaultPanelView(SceneType::Scene3D).id;
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileNewNodeGraph, [ctx]() mutable {
		EditorTab new_tab;
		new_tab.name = "Node-Graph-" + std::to_string(ctx.editor_area.GetTabs().size() + 1);
		new_tab.path = "";
		new_tab.modified = false;
		new_tab.active = true;
		new_tab.scene_type = SceneType::NodeEditor;
		new_tab.node_canvas_pan = ImVec2(0.0f, 0.0f);
		new_tab.node_canvas_zoom = 1.0f;
		new_tab.secondary_active_view_id = UI::GetDefaultSecondaryView(SceneType::NodeEditor).id;
		new_tab.panel_active_view_id = UI::GetDefaultPanelView(SceneType::NodeEditor).id;
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileOpen, [ctx]() mutable {
		EditorTab new_tab;
		new_tab.name = "example.cpp";
		new_tab.path = "C:/path/to/example.cpp";
		new_tab.modified = false;
		new_tab.active = true;
		new_tab.scene_type = SceneType::Scene3D;
		new_tab.secondary_active_view_id = UI::GetDefaultSecondaryView(SceneType::Scene3D).id;
		new_tab.panel_active_view_id = UI::GetDefaultPanelView(SceneType::Scene3D).id;
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileSave, [ctx]() mutable {
		ctx.notification.SetMessage("File saved");
	});

	service.Register(CommandId::FileExit, [ctx]() mutable {
		ctx.request_exit();
	});
}
