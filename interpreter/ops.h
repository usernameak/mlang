#pragma once

#include <cstdint>
#include <string>

struct MOp {
	int type;
};

struct MAddOp : MOp {};

struct MSubOp : MOp {};

struct MMulOp : MOp {};

struct MDivOp : MOp {};

struct MPushOp : MOp {
	double value;
};

struct MPushvOp : MOp {
	std::string name;
};

struct MAssnOp : MOp {
	std::string name;
};

struct MRtclOp : MOp {
	std::string name;
};

struct MPushfOp : MOp {
	std::string name;
};

struct MRetOp : MOp {};

struct MPushsOp : MOp {
	std::string str;
};

struct MCallOp : MOp {};

struct MPopOp : MOp {};