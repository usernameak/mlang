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
public:
	Runtime();
	~Runtime();
	void load(std::istream&);
	void load(std::istream&, std::string);
	MFrame* loadFrame(std::istream&);
	MFrame* loadFrame(std::istream&, std::string);
	MValue* run(std::string = "main");
	static MFrame* findFrame(std::vector<MFrame*>, std::string);
	void runFrame(std::vector<MFrame*>, std::string);
};
