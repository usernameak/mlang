#include "vals.h"
#include "opcodes.h"

MValue* MValue::castTo(MType type) {
	return this;
}

void* MValue::get() {
	return nullptr;
}

MType MValue::getType() const {
	return type;
}

MValue* MValue::operate(Opcode op, MValue *val2) {
	return nullptr;
}
