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
#include "../scenes/scene_plugin_registry.h"

namespace {

const Scenes::ScenePluginDescriptor* ResolveDefaultPlugin()
{
    Scenes::ScenePluginRegistry::Instance().EnsureLoaded();
    const std::string preferred_id = UI::GetDefaultScenePluginId();
    if (!preferred_id.empty()) {
        if (const auto* preferred = Scenes::ScenePluginRegistry::Instance().FindPluginById(preferred_id)) {
            return preferred;
        }
    }

    const auto& plugins = Scenes::ScenePluginRegistry::Instance().GetPlugins();
    if (plugins.empty()) {
        return nullptr;
    }
    return &plugins.front();
}

EditorTab MakeDefaultSceneTab(const IEditorAreaService& editor_area)
{
    EditorTab tab;
    const auto* plugin = ResolveDefaultPlugin();
    if (plugin) {
        int same_type_count = 0;
        for (const auto& existing : editor_area.GetTabs()) {
            if (existing.scene_plugin_id == plugin->id) {
                ++same_type_count;
            }
        }

        tab.name = plugin->name + "-" + std::to_string(same_type_count + 1);
        tab.scene_plugin_id = plugin->id;
    } else {
        tab.name = "Untitled-" + std::to_string(editor_area.GetTabs().size() + 1);
    }

    tab.path = "";
    tab.modified = false;
    tab.active = true;
    return tab;
}

}

void RegisterFileCommands(CommandService& service, CommandHandlersContext ctx)
{
	service.Register(CommandId::FileNew2D, [ctx]() mutable {
		EditorTab new_tab = MakeDefaultSceneTab(ctx.editor_area);
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileNew, [ctx]() mutable {
		EditorTab new_tab = MakeDefaultSceneTab(ctx.editor_area);
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileNew3D, [ctx]() mutable {
		EditorTab new_tab = MakeDefaultSceneTab(ctx.editor_area);
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileNewNodeGraph, [ctx]() mutable {
		EditorTab new_tab = MakeDefaultSceneTab(ctx.editor_area);
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileOpen, [ctx]() mutable {
		EditorTab new_tab = MakeDefaultSceneTab(ctx.editor_area);
		new_tab.name = "example.cpp";
		new_tab.path = "C:/path/to/example.cpp";
		ctx.editor_area.AddTab(new_tab);
	});

	service.Register(CommandId::FileSave, [ctx]() mutable {
		ctx.notification.SetMessage("File saved");
	});

	service.Register(CommandId::FileExit, [ctx]() mutable {
		ctx.request_exit();
	});
}
