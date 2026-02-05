/**
 * @file window_commands.cpp
 * @brief 窗口相关命令的实现（最小化、关闭等）
 * @author Your Name
 * @date 2026-02-05
 */

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
