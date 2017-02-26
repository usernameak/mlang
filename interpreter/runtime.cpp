#include "runtime.h"

#include <iostream>
#include <stack>
#include <cstdint>
#include <map>
#include <string>
#define NDEBUG
#include <cassert>

Runtime::Runtime(std::istream* _bcstream) {
	bcstream = _bcstream;
	rstack = new std::stack<int32_t>();
	rvars = new std::map<std::string, int32_t>();
	opsv = new std::vector<MOp*>();
}

void Runtime::load() {
	uint8_t op;

	while(true) {
		bcstream->read((char*)&op, 1);
		if(!bcstream->good()) {
			break;
		}

		switch(op) {
			case OPCODE_ADD:{
				MAddOp *addop = new MAddOp();
				addop->type = op;
				opsv->push_back((MOp*)addop);
			}break;
			case OPCODE_SUB:{
				MSubOp *subop = new MSubOp();
				subop->type = op;
				opsv->push_back((MOp*)subop);
			}break;
			case OPCODE_MUL:{
				MMulOp *mulop = new MMulOp();
				mulop->type = op;
				opsv->push_back((MOp*)mulop);
			}break;
			case OPCODE_DIV:{
				MDivOp *divop = new MDivOp();
				divop->type = op;
				opsv->push_back((MOp*)divop);
			}break;
			case OPCODE_PUSH:{
				MPushOp *pushop = new MPushOp();
				pushop->type = op;
				bcstream->read((char*)&pushop->value, 4);
				opsv->push_back((MOp*)pushop);
			}break;
			case OPCODE_PUSHV:{
				MPushvOp *pushvop = new MPushvOp();
				pushvop->type = op;
				std::getline(*bcstream, pushvop->name, '\0');
				opsv->push_back((MOp*)pushvop);
			}break;
			case OPCODE_ASSN:{
				MAssnOp *assnop = new MAssnOp();
				assnop->type = op;
				std::getline(*bcstream, assnop->name, '\0');
				opsv->push_back((MOp*)assnop);
			}break;
			case OPCODE_RTCL:{
				MRtclOp *rtclop = new MRtclOp();
				rtclop->type = op;
				std::getline(*bcstream, rtclop->name, '\0');
				opsv->push_back((MOp*)rtclop);
			break;}
		}
		
	}
}

void Runtime::run() {

	for(MOp* op : *opsv) {
		
		int32_t val, a, b;
		switch(op->type) {
			case OPCODE_ADD:
				b = rstack->top();
				rstack->pop();
				a = rstack->top();
				rstack->pop();
				rstack->push(a+b);
			break;
			case OPCODE_SUB:
				b = rstack->top();
				rstack->pop();
				a = rstack->top();
				rstack->pop();
				rstack->push(a-b);
			break;
			case OPCODE_MUL:
				b = rstack->top();
				rstack->pop();
				a = rstack->top();
				rstack->pop();
				rstack->push(a*b);
			break;
			case OPCODE_DIV:
				b = rstack->top();
				rstack->pop();
				a = rstack->top();
				rstack->pop();
				rstack->push(a/b);
			break;
			case OPCODE_PUSH:
				rstack->push(((MPushOp*)op)->value);
			break;
			case OPCODE_PUSHV:
				rstack->push((*rvars)[((MPushvOp*)op)->name]);
			break;
			case OPCODE_ASSN:
				val = rstack->top();
				rstack->pop();
				(*rvars)[((MAssnOp*)op)->name] = val;
			break;
			case OPCODE_RTCL:
				if(((MRtclOp*)op)->name.compare("output") == 0) {

					val = rstack->top();
					rstack->pop();
					std::cout << val << std::endl;
				}
			break;
		}
	} 
}