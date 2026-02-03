#include "help_commands.h"

void RegisterHelpCommands(CommandService& service, CommandHandlersContext ctx)
{
	service.Register(CommandId::HelpAbout, [ctx]() mutable {
		ctx.notification.SetMessage("ImGui Try - VSCode-style UI");
	});
}
