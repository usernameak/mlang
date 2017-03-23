#include "runtime.h"

#include <iostream>
#include <stack>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "ops.h"

std::map<std::string, void(*)(std::stack<int32_t>*)> nativefunctions;

void nf_output(std::stack<int32_t>* stack) {
	int32_t val = stack->top();
	stack->pop();
	std::cout << val << std::endl;
}

Runtime::Runtime(std::istream* _bcstream) {
	bcstream = _bcstream;
	rstack = new std::stack<int32_t>();
	NATIVEFUNC_SET("output", nf_output);
}

void Runtime::load() {
	while(true) {
		MFrame* frame = loadFrame();
		if(frame == nullptr) return;
		frames.push_back(frame);
	}
}

MFrame* Runtime::loadFrame() {
	MFrame* frame = new MFrame;
	uint8_t curbyte;

	std::getline(*bcstream, frame->name, '\0');
	
	if(frame->name.empty()) {
		return nullptr;
	}
	
	while(true) {
		bcstream->read((char*)&curbyte, 1);
		if(curbyte==0) break;
		switch(curbyte) {
			case OPCODE_ADD:{
				MAddOp *addop = new MAddOp();
				addop->type = curbyte;
				frame->ops.push_back((MOp*)addop);
			}break;
			case OPCODE_SUB:{
				MSubOp *subop = new MSubOp();
				subop->type = curbyte;
				frame->ops.push_back((MOp*)subop);
			}break;
			case OPCODE_MUL:{
				MMulOp *mulop = new MMulOp();
				mulop->type = curbyte;
				frame->ops.push_back((MOp*)mulop);
			}break;
			case OPCODE_DIV:{
				MDivOp *divop = new MDivOp();
				divop->type = curbyte;
				frame->ops.push_back((MOp*)divop);
			}break;
			case OPCODE_PUSH:{
				MPushOp *pushop = new MPushOp();
				pushop->type = curbyte;
				bcstream->read((char*)&pushop->value, 4);
				frame->ops.push_back((MOp*)pushop);
			}break;
			case OPCODE_PUSHV:{
				MPushvOp *pushvop = new MPushvOp();
				pushvop->type = curbyte;
				std::getline(*bcstream, pushvop->name, '\0');
				frame->ops.push_back((MOp*)pushvop);
			}break;
			case OPCODE_ASSN:{
				MAssnOp *assnop = new MAssnOp();
				assnop->type = curbyte;
				std::getline(*bcstream, assnop->name, '\0');
				frame->ops.push_back((MOp*)assnop);
			}break;
			case OPCODE_RTCL:{
				MRtclOp *rtclop = new MRtclOp();
				rtclop->type = curbyte;
				std::getline(*bcstream, rtclop->name, '\0');
				frame->ops.push_back((MOp*)rtclop);
			}break;
			case OPCODE_FSTART:{
				MFstartOp *fstartop = new MFstartOp();
				fstartop->type = curbyte;
				frame->ops.push_back((MOp*)fstartop);
			}break;
			case OPCODE_RET:{
				MRetOp *retop = new MRetOp();
				retop->type = curbyte;
				frame->ops.push_back((MOp*)retop);
			}break;
			case OPCODE_PUSHS:{
				MPushsOp *pushsop = new MPushsOp();
				pushsop->type = curbyte;
				std::getline(*bcstream, pushsop->str, '\0');
				frame->ops.push_back((MOp*)pushsop);
			}break;
			case OPCODE_CALL:{
				MCallOp *callop = new MCallOp();
				callop->type = curbyte;
				frame->ops.push_back((MOp*)callop);
			}break;
			case OPCODE_POP:{
				MPopOp *popop = new MPopOp();
				popop->type = curbyte;
				frame->ops.push_back((MOp*)popop);
			}break;
		}
	}

	while(true) {
		MFrame* sframe = loadFrame();
		if(sframe == nullptr) break;
		frame->subframes.push_back(sframe);
	}

	return frame;
}

MFrame* Runtime::findFrame(std::vector<MFrame*> framev, std::string framename) {
	for(MFrame* frame : framev) {
		if(frame->name.compare(framename) == 0) {
			return frame;
		}
	}
	std::cout << "Error: frame \"" + framename + "\" not found" << std::endl;
	return nullptr;
}

void Runtime::run() {
	runFrame(frames, "main");
}

void Runtime::runFrame(std::vector<MFrame*> ldframes, std::string framename) {
	MFrame* frame = findFrame(ldframes, framename);
	for(MOp* op : frame->ops) {
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
				rstack->push(frame->vars[((MPushvOp*)op)->name]);
			break;
			case OPCODE_ASSN:
				val = rstack->top();
				rstack->pop();
				frame->vars[((MAssnOp*)op)->name] = val;
			break;
			case OPCODE_RTCL:
				nativefunctions[((MRtclOp*)op)->name](rstack);
			break;
			case OPCODE_PUSHS:
				stringpool.push_back(&((MPushsOp*)op)->str);
				rstack->push(stringpool.size()-1);
			break;
			case OPCODE_CALL:
				val = rstack->top();
				rstack->pop();
				Runtime::runFrame(frame->subframes, *stringpool[val]); // TODO: PARENT FRAME CALLS
			break;
			case OPCODE_RET:
			return;
			case OPCODE_POP:
				rstack->pop();
			break;
		}
	}
}