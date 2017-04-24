#include "vals.h"

#include <string>

MFunctionValue::MFunctionValue(std::string fname) : fname(fname) {}

MValue* MFunctionValue::castTo(MType type) {
	switch(type) {
		case MTYPE_NULL: return new MNullValue;
		case MTYPE_NUMBER: return new MNullValue;
		case MTYPE_STRING: return new MNullValue;
		case MTYPE_FUNCTION: return this;
		case MTYPE_BOOL: return new MBooleanValue(true);
		default: return new MNullValue;
	}
}

void* MFunctionValue::get() {
	return &fname;
}

MType MFunctionValue::getType() const {
	return type;
}

MValue* MFunctionValue::operate(Opcode op, MValue *val2) {
	switch(val2->getType()) {
		case MTYPE_FUNCTION:
			switch(op) {
				case OPCODE_EQ:
					return new MBooleanValue(fname.compare(*(std::string*)val2->get()) == 0);
				case OPCODE_NEQ:
					return new MBooleanValue(fname.compare(*(std::string*)val2->get()) != 0);
			}
	}
	switch(op) {
		case OPCODE_EQ:
			return new MBooleanValue(false);
		case OPCODE_NEQ:
			return new MBooleanValue(true);
	}
	return new MNullValue;
}
