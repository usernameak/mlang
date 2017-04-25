#include "ops.h"
#include <stack>
#include "types.h"

namespace mlang {
    void ops_n::pushsop::execute(runtime_stack &stack) {
        stack.push(new MStringValue(str));
    }
}
