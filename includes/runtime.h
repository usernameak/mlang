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
	std::vector<mlang::ops_n::base*> ops;
	std::string name;
	std::vector<MFrame*> subframes;
	std::map<std::string, MValue*> vars;

};

class Runtime {
private:
	std::vector<MFrame*> frames;
	std::stack<MValue*>* rstack;
public:
	Runtime();
	~Runtime();
	void load(std::istream&, const std::string = "");
	MFrame* loadFrame(std::istream&, const std::string = "");
	MValue* run(const std::string = "main");
	static MFrame* findFrame(std::vector<MFrame*>, const std::string);
	void runFrame(std::vector<MFrame*>, const std::string);
};
