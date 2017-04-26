#include "ops.h"
#include <stack>
#include "types.h"
#include <memory>

namespace mlang {
    void ops_n::pushop::execute(runtime_stack &stack) {
        stack.push(std::make_shared<MNumberValue>(value));
    }
}
