#include "vals.h"

#include <string>

MStringValue::MStringValue(std::string val) : val(val) {}

MValue* MStringValue::castTo(MType type) {
	switch(type) {
		case MTYPE_NULL: return new MNullValue;
		case MTYPE_NUMBER: return new MNumberValue(std::stod(val));
		case MTYPE_STRING: return this;
		case MTYPE_FUNCTION: return new MNullValue;
		case MTYPE_BOOL: return new MBooleanValue(!val.empty());
	}
}

void* MStringValue::get() {
	return &val;
}

MType MStringValue::getType() const {
	return type;
}

MValue* MStringValue::operate(Opcode op, MValue *val2) {
	switch(val2->getType()) {
		case MTYPE_NUMBER:
			switch(op) {
				case OPCODE_ADD:
					return new MStringValue(val + *(std::string*)val2->get());
				break;
			}
		break;
		case MTYPE_STRING:
			switch(op) {
				case OPCODE_ADD:
					return new MStringValue(val + *(std::string*)val2->castTo(MTYPE_STRING)->get());
				case OPCODE_EQ:
					return new MBooleanValue(val.compare(*(std::string*)val2->get()) == 0);
				case OPCODE_NEQ:
					return new MBooleanValue(val.compare(*(std::string*)val2->get()) != 0);
			}
		case MTYPE_NULL:
			switch(op) {
				case OPCODE_ADD:
					return new MStringValue(val + "null");
			}
		case MTYPE_BOOL:
			switch(op) {
				case OPCODE_ADD:
					return new MStringValue(val + *(std::string*)val2->castTo(MTYPE_STRING)->get());
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