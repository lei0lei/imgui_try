/**
 * @file view_commands.cpp
 * @brief 视图相关命令的实现
 * @author Your Name
 * @date 2026-02-05
 */

#include "view_commands.h"

void RegisterViewCommands(CommandService& service, CommandHandlersContext ctx)
{
	service.Register(CommandId::TogglePrimarySidebar, [ctx]() mutable {
		ctx.toggle_primary_sidebar();
	});

	service.Register(CommandId::ViewExplorer, [ctx]() mutable {
		ctx.activity_bar.SetSelectedItem(1); // Explorer
		ctx.set_primary_sidebar_visible(true);
	});

	service.Register(CommandId::ViewConsole, [ctx]() mutable {
		ctx.notification.SetMessage("Console is scene-owned inside editor area");
	});
}
