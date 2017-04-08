#include "vals.h"

MBooleanValue::MBooleanValue(bool val) : val(val) {}

MValue* MBooleanValue::castTo(MType type) {
	switch(type) {
		case MTYPE_NULL: return new MNullValue;
		case MTYPE_NUMBER: return new MNumberValue(val ? 1 : 0);
		case MTYPE_STRING: return new MStringValue(val ? "true" : "false");
		case MTYPE_FUNCTION: return new MNullValue;
		case MTYPE_BOOL: return this;
	}
}

void* MBooleanValue::get() {
	return &val;
}

MType MBooleanValue::getType() const {
	return type;
}

MValue* MBooleanValue::operate(Opcode op, MValue *val2) {
	switch(val2->getType()) {
		/*case MTYPE_STRING:
			switch(op) {
				case OPCODE_ADD:
					return new MStringValue(val ? "true" : "false" + *(std::string*)val2->get());
			}
		break;
		case MTYPE_BOOL:
			switch(op) {
				case OPCODE_EQ:
					return new MBooleanValue(val == *(bool*)val2->get());
				case OPCODE_NEQ:
					return new MBooleanValue(val != *(bool*)val2->get());
			}
		case MTYPE_NUMBER:
			switch(op) {
				case OPCODE_AND:
					return new MBooleanValue(val && *(bool*)val2->castTo(MTYPE_BOOL)->get());
				case OPCODE_OR:
					return new MBooleanValue(val || *(bool*)val2->castTo(MTYPE_BOOL)->get());
			}*/
		case MTYPE_NULL:
			switch(op) {
				case OPCODE_AND:
					return new MBooleanValue(false);
				case OPCODE_OR:
					return new MBooleanValue(val);
			}
		break;

		case MTYPE_BOOL:
			switch(op) {
				case OPCODE_EQ:
					return new MBooleanValue(val == *(bool*)val2->get());
				case OPCODE_NEQ:
					return new MBooleanValue(val != *(bool*)val2->get());
			}

		case MTYPE_NUMBER:
			switch(op) {
				case OPCODE_AND:
					return new MBooleanValue(val && *(bool*)val2->castTo(MTYPE_BOOL)->get());
				case OPCODE_OR:
					return new MBooleanValue(val || *(bool*)val2->castTo(MTYPE_BOOL)->get());
			}
		break;

		case MTYPE_STRING:
			switch(op) {
				case OPCODE_ADD:
					return new MStringValue(val ? "true" : "false" + *(std::string*)val2->get());
				case OPCODE_AND:
					return new MBooleanValue(val && *(bool*)val2->castTo(MTYPE_BOOL)->get());
				case OPCODE_OR:
					return new MBooleanValue(val || *(bool*)val2->castTo(MTYPE_BOOL)->get());
			}
		break;

	}
	
	switch(op) {
		case OPCODE_EQ:
			return new MBooleanValue(false);
		case OPCODE_NEQ:
			return new MBooleanValue(true);
	}

	return new MNullValue;
}