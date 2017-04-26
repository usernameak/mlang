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

std::shared_ptr<MValue> MFunctionValue::operate(Opcode op, std::shared_ptr<MValue> val2) {
	switch(val2->getType()) {
		case MTYPE_FUNCTION:
			switch(op) {
				case OPCODE_EQ:
					return std::make_shared<MBooleanValue>(fname.compare(*(std::string*)val2->get()) == 0);
				case OPCODE_NEQ:
					return std::make_shared<MBooleanValue>(fname.compare(*(std::string*)val2->get()) != 0);
			}
	}
	switch(op) {
		case OPCODE_EQ:
			return std::make_shared<MBooleanValue>(false);
		case OPCODE_NEQ:
			return std::make_shared<MBooleanValue>(true);
	}
	return std::make_shared<MNullValue>();
}
