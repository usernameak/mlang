#pragma once

#include <cstdint>
#include <string>

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
	OPCODE_POP = 13,
	OPCODE_PUSHB = 14
};

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

struct MPushbOp : MOp {
	bool value;
};