#include "ops.h"
#include <stack>
#include "types.h"

namespace mlang {
    void ops_n::pushbop::execute(runtime_stack &stack) {
        stack.push(new MBooleanValue(value));
    }
}
