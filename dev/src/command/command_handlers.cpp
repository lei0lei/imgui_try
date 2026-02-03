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
