#include "runtime.h"

#include <iostream>
#include <stack>
#include <cstdint>
#include <map>
#include <string>

Runtime::Runtime(std::istream* _bcstream) {
	bcstream = _bcstream;
	rstack = new std::stack<int32_t>();
	rvars = new std::map<std::string, int32_t>();
}

void Runtime::run() {
	uint8_t op;

	while(true) {
		bcstream->read((char*)&op, 1);
		if(!bcstream->good()) {
			break;
		}

		//if(!rstack->empty()) std::cout << rstack->top() << std::endl;

		int32_t a, b, val;
		std::string name;
		switch(op) {
			case OPCODE_ADD:
				b = rstack->top();
				rstack->pop();
				a = rstack->top();
				rstack->pop();
				rstack->push(a+b);
			break;
			case OPCODE_SUB:
				b = rstack->top();
				rstack->pop();
				a = rstack->top();
				rstack->pop();
				rstack->push(a-b);
			break;
			case OPCODE_MUL:
				b = rstack->top();
				rstack->pop();
				a = rstack->top();
				rstack->pop();
				rstack->push(a*b);
			break;
			case OPCODE_DIV:
				b = rstack->top();
				rstack->pop();
				a = rstack->top();
				rstack->pop();
				rstack->push(a/b);
			break;
			case OPCODE_PUSH:
				bcstream->read((char*)&val, 4);
				rstack->push(val);
			break;
			case OPCODE_PUSHV:
				std::getline(*bcstream, name, '\0');
				rstack->push((*rvars)[name]);
			break;
			case OPCODE_ASSN:
				std::getline(*bcstream, name, '\0');
				val = rstack->top();
				rstack->pop();
				(*rvars)[name] = val;
			break;
			case OPCODE_RTCL:
				std::getline(*bcstream, name, '\0');
				//std::cout << name << std::endl;
				if(name.compare("output") == 0) {

					val = rstack->top();
					rstack->pop();
					std::cout << val << std::endl;
				}
			break;
		}
	}
	//bcstream->get();
}