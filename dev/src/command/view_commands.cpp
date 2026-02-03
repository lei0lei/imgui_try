#include "view_commands.h"

void RegisterViewCommands(CommandService& service, CommandHandlersContext ctx)
{
	service.Register(CommandId::TogglePrimarySidebar, [ctx]() mutable {
		ctx.toggle_primary_sidebar();
	});

	service.Register(CommandId::TogglePanel, [ctx]() mutable {
		ctx.toggle_panel();
	});

	service.Register(CommandId::ToggleSecondarySidebar, [ctx]() mutable {
		ctx.toggle_secondary_sidebar();
	});

	service.Register(CommandId::ViewExplorer, [ctx]() mutable {
		ctx.activity_bar.SetSelectedItem(1); // Explorer
		ctx.set_primary_sidebar_visible(true);
	});

	service.Register(CommandId::ViewConsole, [ctx]() mutable {
		ctx.panel.SetVisible(true);
		ctx.panel.SetActiveTab(PanelTab::DebugConsole);
	});
}
