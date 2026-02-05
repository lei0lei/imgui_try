/**
 * @file help_commands.cpp
 * @brief 帮助相关命令的实现（关于等）
 * @author Your Name
 * @date 2026-02-05
 */

#include "help_commands.h"

void RegisterHelpCommands(CommandService& service, CommandHandlersContext ctx)
{
	service.Register(CommandId::HelpAbout, [ctx]() mutable {
		ctx.notification.SetMessage("ImGui Try - VSCode-style UI");
	});
}
