#include "window_commands.h"

void RegisterWindowCommands(CommandService& service, CommandHandlersContext ctx)
{
    service.Register(CommandId::WindowMinimize, [ctx]() mutable {
        ctx.window_minimize();
    });

    service.Register(CommandId::WindowMaximize, [ctx]() mutable {
        ctx.window_maximize();
    });

    service.Register(CommandId::WindowClose, [ctx]() mutable {
        ctx.window_close();
    });
}
