#pragma once

#include <vector>

namespace mlang {
    template <typename T> class stack {
    private:
        std::vector<T> vec;
    public:
        T top();
        void pop();
        void push(T);
    };
}

#include "stack_impl.h"
