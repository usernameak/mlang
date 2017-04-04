#include <string>
#include <stack>
#include <iostream>
#include "vals.h"

extern "C" {
	int mlang_nativefunctions_count = 1;
	char** mlang_nativefunctions_list = (char *[]){"nf_output"};

	void nf_output(std::stack<MValue*>* stack) {
		std::string val = *((std::string*) stack->top()->castTo(MTYPE_STRING)->get());
		stack->pop();
		std::cout << val << std::endl;
	}
}
