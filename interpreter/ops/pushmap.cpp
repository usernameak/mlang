#include "ops.h"
#include <stack>
#include "types.h"
#include <memory>

namespace mlang {
    void ops_n::pushmapop::execute(runtime_stack &stack) {
        stack.push(std::make_shared<MMapValue>());
    }
}
