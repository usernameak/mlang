#include "runtime.h"

#include <iostream>
#include <stack>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "ops.h"

Runtime::Runtime(std::istream* _bcstream) {
	bcstream = _bcstream;
	rstack = new std::stack<int32_t>();
	rvars = new std::map<std::string, int32_t>();
	opsv = new std::vector<MOp*>();
	fwstack = new std::stack<int32_t>();
	callstack = new std::stack<int32_t>();
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
			}break;
			case OPCODE_FSTART:{
				MFstartOp *fstartop = new MFstartOp();
				fstartop->type = op;
				opsv->push_back((MOp*)fstartop);
			}break;
			case OPCODE_RET:{
				MRetOp *retop = new MRetOp();
				retop->type = op;
				opsv->push_back((MOp*)retop);
			}break;
			case OPCODE_PUSHF:{
				MPushfOp *pushfop = new MPushfOp();
				pushfop->type = op;
				opsv->push_back((MOp*)pushfop);
			}break;
			case OPCODE_CALL:{
				MCallOp *callop = new MCallOp();
				callop->type = op;
				opsv->push_back((MOp*)callop);
			}break;
			case OPCODE_POP:{
				MPopOp *popop = new MPopOp();
				popop->type = op;
				opsv->push_back((MOp*)popop);
			}break;
		}
		
	}
}

void Runtime::run() {
	int32_t fnum = 0;
	for(auto i=opsv->begin();i<opsv->end();++i) {
		MOp* op = *i;
		if(!fwstack->empty()) {
			switch(op->type) {
				case OPCODE_FSTART:
					fwstack->push(i - opsv->begin());
				break;
				case OPCODE_PUSHF:
					rstack->push(fwstack->top());
					fwstack->pop();
				break;
			}
		} else {
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
				case OPCODE_FSTART:
					fwstack->push(i - opsv->begin());
				break;
				case OPCODE_RET:
					i = opsv->begin() + callstack->top();
					callstack->pop();
				break;
				case OPCODE_PUSHF:
					std::cout << "Error: unexpected `pushf` opcode" << std::endl;
				break;
				case OPCODE_CALL:
					callstack->push(i - opsv->begin());
					i = opsv->begin() + rstack->top();
					rstack->pop();
				break;
				case OPCODE_POP:
					rstack->pop();
				break;
			}
		}
		
	} 
}