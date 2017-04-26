#pragma once

#include <vector>
#include "scope.h"
#include <string>
#include "vals.h"

namespace mlang {
    class runtime_state {
    private:
        std::vector<scope*> scopes;
    public:
        void set_var(std::string, std::shared_ptr<MValue>);
        std::shared_ptr<MValue> get_var(std::string&);
        void push_scope();
        void pop_scope();
    };
}
