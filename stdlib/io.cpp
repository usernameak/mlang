#include <string>
#include "types.h"
#include <iostream>
#include "vals.h"
#include "mmodule.h"

mlang_module("nf_output") {
	mlang_function(nf_output) {
		std::string val = *((std::string*) stack->top()->castTo(MTYPE_STRING)->get());
		stack->pop();
		std::cout << val << std::endl;
	}
}
