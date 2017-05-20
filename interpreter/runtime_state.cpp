#include "runtime_state.h"

#include <string>
#include "vals.h"

using namespace mlang;

MValue* runtime_state::get_var(std::string &name) {
    for(auto it = scopes.crbegin(); it != scopes.crend(); it++) {
        auto scope = *it;
        const auto var_it = scope->vars.find(name);
        if(var_it != scope->vars.end()) {
            return (*var_it).second;
        }
    }
    return nullptr;
}
void runtime_state::set_var(std::string name, MValue* val) {
    for(auto it = scopes.rbegin(); it != scopes.rend(); it++) {
        auto scope = *it;
        if(scope->vars.count(name) != 0) {
            scope->vars[name] = val;
            return;
        }
    }
    (*scopes.rbegin())->vars[name] = val;
}
void runtime_state::push_scope() {
    scopes.push_back(new scope());
}
void runtime_state::pop_scope() {
    delete scopes.back();
    scopes.pop_back();
}
