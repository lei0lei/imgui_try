#include "edit_commands.h"

void RegisterEditCommands(CommandService& service, CommandHandlersContext ctx)
{
	service.Register(CommandId::EditUndo, [ctx]() mutable {
		ctx.notification.SetMessage("Undo");
	});

	service.Register(CommandId::EditRedo, [ctx]() mutable {
		ctx.notification.SetMessage("Redo");
	});
}
