#pragma once

#include <functional>
#include <unordered_map>
#include "command_ids.h"

class CommandService {
public:
    using Handler = std::function<void()>;

    void Register(CommandId id, Handler handler);
    bool Execute(CommandId id) const;
    bool Has(CommandId id) const;

private:
    std::unordered_map<CommandId, Handler> handlers_;
};
