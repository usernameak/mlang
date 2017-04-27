#include "runtime_state.h"

#include <string>
#include "vals.h"
#include <memory>

using namespace mlang;

std::shared_ptr<MValue> runtime_state::get_var(std::string &name) {
    for(auto it = scopes.cbegin(); it != scopes.cend(); it++) {
        auto scope = *it;
        const auto var_it = scope->vars.find(name);
        if(var_it != scope->vars.end()) {
            return (*var_it).second;
        }
    }
    return nullptr;
}
void runtime_state::set_var(std::string name, std::shared_ptr<MValue> val) {
    for(auto it = scopes.begin(); it != scopes.end(); it++) {
        auto scope = *it;
        if(scope->vars.count(name) != 0) {
            scope->vars[name] = val;
            return;
        }
    }
    (*scopes.begin())->vars[name] = val;
}
void runtime_state::push_scope() {
    scopes.push_front(new scope());
}
void runtime_state::pop_scope() {
    delete scopes.front();
    scopes.pop_front();
}
