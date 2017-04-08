#pragma once

#include <string>
#include "ops.h"

enum MType {
	MTYPE_NULL,
	MTYPE_NUMBER,
	MTYPE_STRING,
	MTYPE_FUNCTION,
	MTYPE_BOOL
};

class MValue {
public:
	static const MType type = MTYPE_NULL;
	virtual MValue* castTo(MType);
	virtual void* get();
	//virtual MValue& operator+(MValue&) const;
	virtual MType getType() const;
	virtual MValue* operate(Opcode, MValue*);
};

class MNullValue : public MValue {
public:
	MValue* castTo(MType);
	void* get();
	MType getType() const;
	MValue* operate(Opcode, MValue*);
};

class MNumberValue : public MValue {
private:
	double val;
public:
	static const MType type = MTYPE_NUMBER;
	MNumberValue(double);
	MValue* castTo(MType);
	void* get();
	MType getType() const;
	MValue* operate(Opcode, MValue*);
};

class MStringValue : public MValue {
private:
	std::string val;
public:
	static const MType type = MTYPE_STRING;
	MStringValue(std::string);
	MValue* castTo(MType);
	void* get();
	MType getType() const;
	MValue* operate(Opcode, MValue*);
};

class MFunctionValue : public MValue {
private:
	std::string fname;
public:
	static const MType type = MTYPE_FUNCTION;
	MFunctionValue(std::string);
	MValue* castTo(MType);
	void* get();
	MType getType() const;
	MValue* operate(Opcode, MValue*);
};

class MBooleanValue : public MValue {
private:
	bool val;
public:
	static const MType type = MTYPE_BOOL;
	MBooleanValue(bool);
	MValue* castTo(MType);
	void* get();
	MType getType() const;
	MValue* operate(Opcode, MValue*);
};