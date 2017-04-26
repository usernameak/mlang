#include "ops.h"
#include <stack>
#include "types.h"
#include <memory>

namespace mlang {
    void ops_n::pushsop::execute(runtime_stack &stack) {
        stack.push(std::make_shared<MStringValue>(str));
    }
}
