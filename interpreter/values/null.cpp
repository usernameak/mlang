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

std::shared_ptr<MValue> MNullValue::operate(Opcode op, std::shared_ptr<MValue> val2) {
	switch(op) {
		case OPCODE_ADD:
			switch(val2->getType()) {
				case MTYPE_STRING:
					return std::make_shared<MStringValue>("null"+*(std::string*)val2->get());
				break;
				default:
					return shared_from_this();
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
			return shared_from_this();
		break;
		case OPCODE_EQ:
			switch(val2->getType()) {
				case MTYPE_NULL:
					return std::make_shared<MBooleanValue>(true);
				break;
				default:
					return std::make_shared<MBooleanValue>(false);
				break;
			}
		break;
		case OPCODE_NEQ:
			switch(val2->getType()) {
				case MTYPE_NULL:
					return std::make_shared<MBooleanValue>(false);
				break;
				default:
					return std::make_shared<MBooleanValue>(true);
				break;
			}
		break;

		case OPCODE_AND:
			switch(val2->getType()) {
				case MTYPE_BOOL:
					return std::make_shared<MBooleanValue>(false);
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
			return shared_from_this();
		break;
	}
	return shared_from_this();
}
