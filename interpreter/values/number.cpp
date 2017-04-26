#include "vals.h"
#include "opcodes.h"

#include <string>
#include <sstream>
#include <cstdint>
#include <iomanip>

static std::string doubleToString(double val) {
	std::stringstream ss;
	ss << std::setprecision(20) << val;
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

std::shared_ptr<MValue> MNumberValue::operate(Opcode op, std::shared_ptr<MValue> val2) {
	switch(val2->getType()) {
		case MTYPE_NUMBER:
			switch(op) {
				case OPCODE_ADD:
					return std::make_shared<MNumberValue>(val+ *(double*)val2->get());
				break;
				case OPCODE_SUB:
					return std::make_shared<MNumberValue>(val- *(double*)val2->get());
				case OPCODE_MUL:
					return std::make_shared<MNumberValue>(val* *(double*)val2->get());
				case OPCODE_DIV:
					return std::make_shared<MNumberValue>(val/ *(double*)val2->get());
				case OPCODE_LEQ:
					return std::make_shared<MBooleanValue>(val<= *(double*)val2->get());
				case OPCODE_GRT:
					return std::make_shared<MBooleanValue>(val> *(double*)val2->get());
				case OPCODE_GEQ:
					return std::make_shared<MBooleanValue>(val>= *(double*)val2->get());
				case OPCODE_LESS:
					return std::make_shared<MBooleanValue>(val< *(double*)val2->get());
				case OPCODE_EQ:
					return std::make_shared<MBooleanValue>(val== *(double*)val2->get());
				case OPCODE_NEQ:
					return std::make_shared<MBooleanValue>(val!= *(double*)val2->get());
				case OPCODE_LSH:
					return std::make_shared<MNumberValue>((int32_t)val<<(int32_t)(*(double*)val2->get()));
				case OPCODE_RSH:
					return std::make_shared<MNumberValue>((int32_t)val>>(int32_t)(*(double*)val2->get()));
				case OPCODE_AND:
					return std::make_shared<MNumberValue>((int32_t)val&(int32_t)(*(double*)val2->get()));
				case OPCODE_OR:
					return std::make_shared<MNumberValue>((int32_t)val|(int32_t)(*(double*)val2->get()));
			}
		break;
		case MTYPE_STRING:
			switch(op) {
				case OPCODE_ADD:
					return std::make_shared<MStringValue>(doubleToString(val) + *(std::string*)val2->get());
			}
		default: return std::make_shared<MNullValue>();
	}
	switch(op) {
		case OPCODE_EQ:
			return std::make_shared<MBooleanValue>(false);
		case OPCODE_NEQ:
			return std::make_shared<MBooleanValue>(true);
	}
	return std::make_shared<MNullValue>();
}
