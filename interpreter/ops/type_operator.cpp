#include "ops.h"
#include <stack>
#include "types.h"

namespace mlang {
    void ops_n::type_operator::execute(runtime_stack &stack) {
        MValue *mv2 = stack.top();
        stack.pop();
        MValue *mv1 = stack.top();
        stack.pop();
        stack.push(mv1->operate(type, mv2));
    }
}
