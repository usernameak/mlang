#include "ops.h"
#include <stack>
#include "types.h"

namespace mlang {
    void ops_n::pushop::execute(runtime_stack &stack) {
        stack.push(new MNumberValue(value));
    }
}
