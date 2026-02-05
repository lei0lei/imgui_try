/**
 * @file command_handlers.cpp
 * @brief 应用程序命令处理器的实现
 * @author Your Name
 * @date 2026-02-05
 */

#include "command_handlers.h"
#include "file_commands.h"
#include "edit_commands.h"
#include "view_commands.h"
#include "help_commands.h"
#include "window_commands.h"

void RegisterWorkbenchCommands(CommandService& service, CommandHandlersContext ctx)
{
    RegisterFileCommands(service, ctx);
    RegisterEditCommands(service, ctx);
    RegisterViewCommands(service, ctx);
    RegisterHelpCommands(service, ctx);
    RegisterWindowCommands(service, ctx);
}
