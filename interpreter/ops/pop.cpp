#include "ops.h"
#include <stack>
#include "types.h"

namespace mlang {
    void ops_n::popop::execute(runtime_stack &stack) {
        stack.pop();
    }
}
