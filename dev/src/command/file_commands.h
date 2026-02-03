#pragma once

#include "command_service.h"
#include "command_handlers.h"

void RegisterFileCommands(CommandService& service, CommandHandlersContext ctx);
