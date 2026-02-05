/**
 * @file edit_commands.cpp
 * @brief 编辑相关命令的实现（撤销、重做等）
 * @author Your Name
 * @date 2026-02-05
 */

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
