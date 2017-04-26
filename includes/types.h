#pragma once

#include "stack.h"
#include "vals.h"
#include <memory>

namespace mlang {
    typedef stack<std::shared_ptr<MValue> > runtime_stack;
}
