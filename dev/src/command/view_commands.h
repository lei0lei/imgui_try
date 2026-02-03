#pragma once

#include "command_service.h"
#include "command_handlers.h"

void RegisterViewCommands(CommandService& service, CommandHandlersContext ctx);
