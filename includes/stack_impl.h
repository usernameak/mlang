#include <memory>

using namespace mlang;

template <typename T> T stack<T>::top() {
    return std::move(vec.back());
}

template <typename T> void stack<T>::pop() {
    vec.pop_back();
}

template <typename T> void stack<T>::push(T val) {
    vec.push_back(val);
}
