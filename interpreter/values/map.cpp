#include "vals.h"
#include "opcodes.h"

MMapValue::MMapValue() {

}

MValue* MMapValue::castTo(MType type) {
	switch(type) {
		case MTYPE_NULL: return new MNullValue;
		case MTYPE_NUMBER: return new MNumberValue(0);
		case MTYPE_STRING: return new MStringValue("(map)");
		case MTYPE_FUNCTION: return new MNullValue;
		case MTYPE_BOOL: return new MBooleanValue(true);
        case MTYPE_MAP: return this;
        default: return new MNullValue;
	}
}

void* MMapValue::get() {
	return &val;
}

MType MMapValue::getType() const {
	return type;
}

MValue* MMapValue::operate(Opcode op, MValue *val2) {
	return new MNullValue;
}

void MMapValue::add(std::string key, MValue *value) {
    val[key] = value;
}
