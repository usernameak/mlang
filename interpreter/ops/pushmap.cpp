#include "ops.h"
#include <stack>
#include "types.h"

namespace mlang {
    void ops_n::pushmapop::execute(runtime_stack &stack) {
        stack.push(new MMapValue());
    }
}
