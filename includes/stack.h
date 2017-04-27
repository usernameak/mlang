#pragma once

#include <forward_list>

namespace mlang {
    template <typename T> class stack {
    private:
        std::forward_list<T> lst;
    public:
        T top();
        void pop();
        void push(T);
    };
}

#include "stack_impl.h"
