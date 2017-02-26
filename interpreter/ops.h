#pragma once

#include <cstdint>
#include <string>

struct MOp {

	int type;
};

struct MAddOp  : MOp {
};

struct MSubOp :  MOp {
};

struct MMulOp :  MOp {
};

struct MDivOp :  MOp {
};

struct MPushOp:   MOp {
	int32_t value;
};

struct MPushvOp   :MOp {
	std::string name;
};

struct MAssnOp :  MOp {
	std::string name;
};

struct MRtclOp  : MOp {
	std::string name;
};