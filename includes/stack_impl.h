#include <memory>

using namespace mlang;

template <typename T> T stack<T>::top() {
    return lst.front();
}

template <typename T> void stack<T>::pop() {
    lst.pop_front();
}

template <typename T> void stack<T>::push(T val) {
    lst.push_front(val);
}
