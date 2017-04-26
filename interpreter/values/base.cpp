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

std::shared_ptr<MValue> MValue::operate(Opcode op, std::shared_ptr<MValue> val2) {
	return nullptr;
}
