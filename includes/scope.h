#pragma once

#include "stack.h"
#include <unordered_map>
#include <string>
#include "vals.h"

namespace mlang {
    struct scope {
    public:
        std::unordered_map<std::string, std::shared_ptr<MValue> > vars;
    };
}
