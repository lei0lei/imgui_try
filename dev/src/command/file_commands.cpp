#include "file_commands.h"
#include "imgui.h"

void RegisterFileCommands(CommandService& service, CommandHandlersContext ctx)
{
	service.Register(CommandId::FileNew2D, [ctx]() mutable {
		EditorTab new_tab;
		new_tab.name = "2D-Scene-" + std::to_string(ctx.editor_area.GetTabs().size() + 1);
		new_tab.path = "";
		new_tab.modified = false;
		new_tab.active = true;
		new_tab.scene_type = SceneType::Scene2D;
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileNew, [ctx]() mutable {
		EditorTab new_tab;
		new_tab.name = "3D-Scene-" + std::to_string(ctx.editor_area.GetTabs().size() + 1);
		new_tab.path = "";
		new_tab.modified = false;
		new_tab.active = true;
		new_tab.scene_type = SceneType::Scene3D;
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileNew3D, [ctx]() mutable {
		EditorTab new_tab;
		new_tab.name = "3D-Scene-" + std::to_string(ctx.editor_area.GetTabs().size() + 1);
		new_tab.path = "";
		new_tab.modified = false;
		new_tab.active = true;
		new_tab.scene_type = SceneType::Scene3D;
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
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileOpen, [ctx]() mutable {
		EditorTab new_tab;
		new_tab.name = "example.cpp";
		new_tab.path = "C:/path/to/example.cpp";
		new_tab.modified = false;
		new_tab.active = true;
		new_tab.scene_type = SceneType::Scene3D;
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileSave, [ctx]() mutable {
		ctx.notification.SetMessage("File saved");
	});

	service.Register(CommandId::FileExit, [ctx]() mutable {
		ctx.request_exit();
	});
}
