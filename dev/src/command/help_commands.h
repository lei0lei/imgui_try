#pragma once

#include "command_service.h"
#include "command_handlers.h"

void RegisterHelpCommands(CommandService& service, CommandHandlersContext ctx);
