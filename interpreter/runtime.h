#pragma once

#include <iostream>
#include <stack>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "ops.h"
#include "vals.h"

#define NATIVEFUNC_SET(name, func) nativefunctions[name] = &func

enum Opcode {
	OPCODE_ADD = 1,
	OPCODE_SUB = 2,
	OPCODE_MUL = 3,
	OPCODE_DIV = 4,
	OPCODE_PUSH = 5,
	OPCODE_PUSHV = 6,
	OPCODE_ASSN = 7,
	OPCODE_RTCL = 8,
	OPCODE_PUSHF = 9,
	OPCODE_RET = 10,
	OPCODE_PUSHS = 11,
	OPCODE_CALL = 12,
	OPCODE_POP = 13
};

struct MFrame {
	std::vector<MOp*> ops;
	std::string name;
	std::vector<MFrame*> subframes;
	std::map<std::string, MValue*> vars;
	
};

class Runtime {
private:
	std::vector<MFrame*> frames;
	std::istream* bcstream;
	std::stack<MValue*>* rstack;
	std::vector<std::string*> stringpool; // unsafe workaround pool that can cause some memory leaks, will be replaced with something better
public:
	Runtime(std::istream*);
	void load();
	MFrame* loadFrame();
	void run();
	static MFrame* findFrame(std::vector<MFrame*>, std::string);
	void runFrame(std::vector<MFrame*>, std::string);
};