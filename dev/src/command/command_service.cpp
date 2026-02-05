/**
 * @file command_service.cpp
 * @brief 命令服务的实现，用于管理应用程序命令
 * @author Your Name
 * @date 2026-02-05
 */

#include "command_service.h"

void CommandService::Register(CommandId id, Handler handler)
{
    handlers_[id] = std::move(handler);
}

bool CommandService::Execute(CommandId id) const
{
    auto it = handlers_.find(id);
    if (it == handlers_.end())
        return false;
    it->second();
    return true;
}

bool CommandService::Has(CommandId id) const
{
    return handlers_.find(id) != handlers_.end();
}
