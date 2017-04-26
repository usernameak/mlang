#include "ops.h"
#include <stack>
#include "types.h"
#include <memory>

namespace mlang {
    void ops_n::type_operator::execute(runtime_stack &stack) {
        std::shared_ptr<MValue> mv2 = stack.top();
        stack.pop();
        std::shared_ptr<MValue> mv1 = stack.top();
        stack.pop();
        stack.push(mv1->operate(type, mv2));
    }
}
