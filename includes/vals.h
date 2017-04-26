#pragma once

#include <string>
#include "opcodes.h"
#include <unordered_map>
#include <memory>

enum MType {
	MTYPE_NULL,
	MTYPE_NUMBER,
	MTYPE_STRING,
	MTYPE_FUNCTION,
	MTYPE_BOOL,
	MTYPE_MAP
};

class MValue : public std::enable_shared_from_this<MValue> {
public:
	static const MType type = MTYPE_NULL;
	virtual MValue* castTo(MType);
	virtual void* get();
	//virtual MValue& operator+(MValue&) const;
	virtual MType getType() const;
	virtual std::shared_ptr<MValue> operate(Opcode, std::shared_ptr<MValue>);
};

class MNullValue : public MValue {
public:
	MValue* castTo(MType);
	void* get();
	MType getType() const;
	std::shared_ptr<MValue> operate(Opcode, std::shared_ptr<MValue>);
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
	std::shared_ptr<MValue> operate(Opcode, std::shared_ptr<MValue>);
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
	std::shared_ptr<MValue> operate(Opcode, std::shared_ptr<MValue>);
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
	std::shared_ptr<MValue> operate(Opcode, std::shared_ptr<MValue>);
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
	std::shared_ptr<MValue> operate(Opcode, std::shared_ptr<MValue>);
};

class MMapValue : public MValue {
private:
	std::unordered_map<std::string, std::shared_ptr<MValue> > val;
public:
	MMapValue();
	static const MType type = MTYPE_MAP;
	MValue* castTo(MType);
	void* get();
	MType getType() const;
	std::shared_ptr<MValue> operate(Opcode, std::shared_ptr<MValue>);
	void add(std::string, std::shared_ptr<MValue>);
};
