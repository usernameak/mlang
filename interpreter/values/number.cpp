#include "vals.h"
#include "opcodes.h"

#include <string>
#include <sstream>
#include <cstdint>

static std::string doubleToString(double val) {
	std::stringstream ss;
	ss << val;
	std::string ret = ss.str();
	return ret;
}

MNumberValue::MNumberValue(double val) : val(val) {}

MValue* MNumberValue::castTo(MType type) {
	switch(type) {
		case MTYPE_NULL: return new MNullValue;
		case MTYPE_NUMBER: return this;
		case MTYPE_STRING: return new MStringValue(doubleToString(val));
		case MTYPE_FUNCTION: return new MNullValue;
		case MTYPE_BOOL: return new MBooleanValue(val == 0 ? false : true);
		default: return new MNullValue;
	}
}

void* MNumberValue::get() {
	return &val;
}

MType MNumberValue::getType() const {
	return type;
}

MValue* MNumberValue::operate(Opcode op, MValue *val2) {
	switch(val2->getType()) {
		case MTYPE_NUMBER:
			switch(op) {
				case OPCODE_ADD:
					return new MNumberValue(val+ *(double*)val2->get());
				break;
				case OPCODE_SUB:
					return new MNumberValue(val- *(double*)val2->get());
				case OPCODE_MUL:
					return new MNumberValue(val* *(double*)val2->get());
				case OPCODE_DIV:
					return new MNumberValue(val/ *(double*)val2->get());
				case OPCODE_LEQ:
					return new MBooleanValue(val<= *(double*)val2->get());
				case OPCODE_GRT:
					return new MBooleanValue(val> *(double*)val2->get());
				case OPCODE_GEQ:
					return new MBooleanValue(val>= *(double*)val2->get());
				case OPCODE_LESS:
					return new MBooleanValue(val< *(double*)val2->get());
				case OPCODE_EQ:
					return new MBooleanValue(val== *(double*)val2->get());
				case OPCODE_NEQ:
					return new MBooleanValue(val!= *(double*)val2->get());
				case OPCODE_LSH:
					return new MNumberValue((int32_t)val<<(int32_t)(*(double*)val2->get()));
				case OPCODE_RSH:
					return new MNumberValue((int32_t)val>>(int32_t)(*(double*)val2->get()));
				case OPCODE_AND:
					return new MNumberValue((int32_t)val&(int32_t)(*(double*)val2->get()));
				case OPCODE_OR:
					return new MNumberValue((int32_t)val|(int32_t)(*(double*)val2->get()));
			}
		break;
		case MTYPE_STRING:
			switch(op) {
				case OPCODE_ADD:
					return new MStringValue(doubleToString(val) + *(std::string*)val2->get());
			}
		default: return new MNullValue;
	}
	switch(op) {
		case OPCODE_EQ:
			return new MBooleanValue(false);
		case OPCODE_NEQ:
			return new MBooleanValue(true);
	}
	return new MNullValue;
}
