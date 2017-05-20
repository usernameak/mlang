#include "vals.h"
#include "opcodes.h"

MValue* MNullValue::castTo(MType type) {
	switch(type) {
		case MTYPE_NULL: return this;
		case MTYPE_NUMBER: return new MNumberValue(0);
		case MTYPE_STRING: return new MStringValue("null");
		case MTYPE_FUNCTION: return this;
		case MTYPE_BOOL: return new MBooleanValue(false);
		default: return new MNullValue;
	}
}

void* MNullValue::get() {
	return nullptr;
}

MType MNullValue::getType() const {
	return type;
}

MValue* MNullValue::operate(Opcode op, MValue *val2) {
	switch(op) {
		case OPCODE_ADD:
			switch(val2->getType()) {
				case MTYPE_STRING:
					return new MStringValue("null"+*(std::string*)val2->get());
				break;
				default:
					return this;
				break;
			}
		break;

		case OPCODE_SUB:
		case OPCODE_MUL:
		case OPCODE_DIV:
		case OPCODE_LEQ:
		case OPCODE_GRT:
		case OPCODE_GEQ:
		case OPCODE_LESS:
			return this;
		break;
		case OPCODE_EQ:
			switch(val2->getType()) {
				case MTYPE_NULL:
					return new MBooleanValue(true);
				break;
				default:
					return new MBooleanValue(false);
				break;
			}
		break;
		case OPCODE_NEQ:
			switch(val2->getType()) {
				case MTYPE_NULL:
					return new MBooleanValue(false);
				break;
				default:
					return new MBooleanValue(true);
				break;
			}
		break;

		case OPCODE_AND:
			switch(val2->getType()) {
				case MTYPE_BOOL:
					return new MBooleanValue(false);
				break;
			}
		break;
		case OPCODE_OR:
			switch(val2->getType()) {
				case MTYPE_BOOL:
					return val2;
				break;
			}
		break;
		case OPCODE_LSH:
		case OPCODE_RSH:
		default:
			return this;
		break;
	}
	return this;
}
