#include "vals.h"
#include <string>
#include <sstream>

static std::string doubleToString(double val) {
	std::stringstream ss;
	ss << val;
	std::string ret = ss.str();
	return ret;
}

MValue* MValue::castTo(MType type) {
	return this;
}

void* MValue::get() {
	return nullptr;
}

MValue* MNullValue::castTo(MType type) {
	switch(type) {
		case MTYPE_NULL: return this;
		case MTYPE_NUMBER: return new MNumberValue(0);
		case MTYPE_STRING: return new MStringValue("null");
		case MTYPE_FUNCTION: return nullptr;
		case MTYPE_BOOL: return new MBooleanValue(false);
	}
}

void* MNullValue::get() {
	return nullptr;
}

MNumberValue::MNumberValue(double val) : val(val) {}

MValue* MNumberValue::castTo(MType type) {
	switch(type) {
		case MTYPE_NULL: return new MNullValue;
		case MTYPE_NUMBER: return this;
		case MTYPE_STRING: return new MStringValue(doubleToString(val));
		case MTYPE_FUNCTION: return nullptr;
		case MTYPE_BOOL: return new MBooleanValue(val == 0 ? false : true);
	}
}

void* MNumberValue::get() {
	return &val;
}

MStringValue::MStringValue(std::string val) : val(val) {}

MValue* MStringValue::castTo(MType type) {
	switch(type) {
		case MTYPE_NULL: return new MNullValue;
		case MTYPE_NUMBER: return new MNumberValue(std::stod(val));
		case MTYPE_STRING: return this;
		case MTYPE_FUNCTION: return nullptr;
		case MTYPE_BOOL: return new MBooleanValue(!val.empty());
	}
}

void* MStringValue::get() {
	return &val;
}

MFunctionValue::MFunctionValue(std::string fname) : fname(fname) {}

MValue* MFunctionValue::castTo(MType type) {
	switch(type) {
		case MTYPE_NULL: return new MNullValue;
		case MTYPE_NUMBER: return nullptr;
		case MTYPE_STRING: return nullptr;
		case MTYPE_FUNCTION: return this;
		case MTYPE_BOOL: return new MBooleanValue(true);
	}
}

void* MFunctionValue::get() {
	return &fname;
}

MBooleanValue::MBooleanValue(bool val) : val(val) {}

MValue* MBooleanValue::castTo(MType type) {
	switch(type) {
		case MTYPE_NULL: return new MNullValue;
		case MTYPE_NUMBER: return new MNumberValue(val ? 1 : 0);
		case MTYPE_STRING: return new MStringValue(val ? "true" : "false");
		case MTYPE_FUNCTION: return nullptr;
		case MTYPE_BOOL: return this;
	}
}

void* MBooleanValue::get() {
	return &val;
}
