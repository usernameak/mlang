#pragma once

#include <iostream>
#include <stack>
#include <cstdint>
#include <map>
#include <string>

enum Opcode {
	OPCODE_ADD = 1,
	OPCODE_SUB = 2,
	OPCODE_MUL = 3,
	OPCODE_DIV = 4,
	OPCODE_PUSH = 5,
	OPCODE_PUSHV = 6,
	OPCODE_ASSN = 7,
	OPCODE_RTCL = 8
};

class Runtime {
private:
	std::istream* bcstream;
	std::stack<int32_t>* rstack;
	std::map<std::string, int32_t>* rvars;
public:
	Runtime(std::istream*);
	void run();
};