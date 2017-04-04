#include "runtime.h"

#include <iostream>
#include <stack>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <dlfcn.h>

#include "ops.h"
#include "vals.h"

std::map<std::string, void(*)(std::stack<MValue*>*)> nativefunctions;

void nf_nativeload(std::stack<MValue*>* stack) {
	std::string prefixes[] = {"./lib", "lib", "", ""};
	std::string suffixes[] = {".so", ".so", ".so", ""};
	void *handle = nullptr;
	for(int i = 0; i < sizeof(prefixes)/sizeof(prefixes[0]); i++) {
		handle = dlopen((prefixes[i]+(*(std::string*) stack->top()->castTo(MTYPE_STRING)->get())+suffixes[i]).c_str(), RTLD_LAZY | RTLD_LOCAL);
		if(!handle) {
			if(i == sizeof(prefixes)/sizeof(prefixes[0])-1) {
				std::cout << dlerror() << std::endl;
				exit(1);
			}
		} else {
			break;
		}
	}
	
	stack->pop();
	int count = *(int*) dlsym(handle, "mlang_nativefunctions_count");
	char** list = *(char***) dlsym(handle, "mlang_nativefunctions_list");
	for(int i = 0; i < count; i++) {
		void (*func)(std::stack<MValue*>*) = (void(*)(std::stack<MValue*>*)) dlsym(handle, list[i]);
		if(func == nullptr) {
			std::cout << dlerror() << std::endl;
			exit(1);
		}
		nativefunctions[list[i]] = func;
	}
}

Runtime::Runtime(std::istream* _bcstream) {
	bcstream = _bcstream;
	rstack = new std::stack<MValue*>();
	NATIVEFUNC_SET("nativeload", nf_nativeload);
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
				bcstream->read((char*)&pushop->value, 8);
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
			case OPCODE_PUSHF:{
				MPushfOp *pushfop = new MPushfOp();
				pushfop->type = curbyte;
				std::getline(*bcstream, pushfop->name, '\0');
				frame->ops.push_back((MOp*)pushfop);
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
		double a, b;
		std::string str;
		switch(op->type) {
			case OPCODE_ADD:

				b = *((double*) rstack->top()->castTo(MTYPE_NUMBER)->get());
				rstack->pop();
				a = *((double*) rstack->top()->castTo(MTYPE_NUMBER)->get());
				rstack->pop();
				rstack->push(new MNumberValue(a+b));
			break;
			case OPCODE_SUB:
				b = *((double*) rstack->top()->castTo(MTYPE_NUMBER)->get());
				rstack->pop();
				a = *((double*) rstack->top()->castTo(MTYPE_NUMBER)->get());
				rstack->pop();
				rstack->push(new MNumberValue(a-b));
			break;
			case OPCODE_MUL:
				b = *((double*) rstack->top()->castTo(MTYPE_NUMBER)->get());
				rstack->pop();
				a = *((double*) rstack->top()->castTo(MTYPE_NUMBER)->get());
				rstack->pop();
				rstack->push(new MNumberValue(a*b));
			break;
			case OPCODE_DIV:
				b = *((double*) rstack->top()->castTo(MTYPE_NUMBER)->get());
				rstack->pop();
				a = *((double*) rstack->top()->castTo(MTYPE_NUMBER)->get());
				rstack->pop();
				rstack->push(new MNumberValue(a/b));
			break;
			case OPCODE_PUSH:
				rstack->push(new MNumberValue(((MPushOp*)op)->value));
			break;
			case OPCODE_PUSHV:
				rstack->push(frame->vars[((MPushvOp*)op)->name]);
			break;
			case OPCODE_ASSN:
				frame->vars[((MAssnOp*)op)->name] = rstack->top();
				rstack->pop();
			break;
			case OPCODE_RTCL:
				nativefunctions[((MRtclOp*)op)->name](rstack);
			break;
			case OPCODE_PUSHF:
				rstack->push(new MFunctionValue(((MPushfOp*)op)->name));
			break;
			case OPCODE_PUSHS:
				rstack->push(new MStringValue(((MPushsOp*)op)->str));
			break;
			case OPCODE_CALL:
				str = *((std::string*) rstack->top()->castTo(MTYPE_FUNCTION)->get());
				rstack->pop();
				Runtime::runFrame(frame->subframes, str); // TODO: PARENT FRAME CALLS
			break;
			case OPCODE_RET:
			return;
			case OPCODE_POP:
				rstack->pop();
			break;
		}
	}
}