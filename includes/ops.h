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
	OPCODE_PUSHB = 14,
	OPCODE_LEQ = 15,
	OPCODE_GRT = 16,
	OPCODE_GEQ = 17,
	OPCODE_LESS = 18,
	OPCODE_EQ = 19,
	OPCODE_NEQ = 20,
	OPCODE_LSH = 21,
	OPCODE_RSH = 22,
	OPCODE_AND = 23,
	OPCODE_OR = 24,
	OPCODE_JN = 25,
	OPCODE_JMP = 26,
	OPCODE_UNMAP = 27,
	OPCODE_PUSHMAP = 28,
	OPCODE_MASSN = 29,
	OPCODE_ECALL = 30
};

struct MOp {
	Opcode type;
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

struct MLeqOp : MOp {};
struct MGrtOp : MOp {};
struct MGeqOp : MOp {};
struct MLessOp : MOp {};
struct MEqOp : MOp {};
struct MNeqOp : MOp {};
struct MLshOp : MOp {};
struct MRshOp : MOp {};
struct MAndOp : MOp {};
struct MOrOp : MOp {};

struct MJnOp : MOp {
	uint32_t ptr;
};

struct MJmpOp : MOp {
	uint32_t ptr;
};

struct MUnmapOp : MOp {};
struct MPushmapOp : MOp {};
struct MMassnOp : MOp {};
struct MEcallOp : MOp {};
