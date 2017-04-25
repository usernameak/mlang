// FIXME: Very baaad code

#include "runtime.h"

#include <iostream>
#include <stack>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#ifdef _WIN32
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif

#include "ops.h"
#include "vals.h"

std::map<std::string, void(*)(std::stack<MValue*>*)> nativefunctions;

#ifdef _WIN32
	void nf_nativeload(std::stack<MValue*>* stack) {
		std::string prefixes[] = {"lib", "", ""};
		std::string suffixes[] = {".dll", ".dll", ""};
		HINSTANCE handle;
		std::string libname = (*(std::string*) stack->top()->castTo(MTYPE_STRING)->get());
		stack->pop();
		for(int i = 0; i < sizeof(prefixes)/sizeof(prefixes[0]); i++) {
			handle = LoadLibrary((prefixes[i]+libname+suffixes[i]).c_str());
			if(!handle) {
				if(i == sizeof(prefixes)/sizeof(prefixes[0])-1) {
					std::cout << "Failed to load library " << libname << std::endl;
					exit(1);
				}
			} else {
				break;
			}
		}

		int count = *(int*) GetProcAddress(handle, "mlang_nativefunctions_count");
		char** list = *(char***) GetProcAddress(handle, "mlang_nativefunctions_list");
		for(int i = 0; i < count; i++) {
			void (*func)(std::stack<MValue*>*) = (void(*)(std::stack<MValue*>*)) GetProcAddress(handle, list[i]);
			if(func == nullptr) {
				std::cout << "Failed to load library " << libname <<
					": function " << list[i] << " cannot be loaded" << std::endl;
				exit(1);
			}
			nativefunctions[list[i]] = func;
		}
	}
#else
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
#endif

Runtime::Runtime() : rstack(new std::stack<MValue*>()) {
	NATIVEFUNC_SET("nativeload", nf_nativeload);
}


void Runtime::load(std::istream& bcstream) {
	load(bcstream, "");
}

void Runtime::load(std::istream& bcstream, const std::string prefix) {
	while(true) {
		MFrame* frame = loadFrame(bcstream, prefix);
		if(frame == nullptr) return;
		frames.push_back(frame);
	}
}

MFrame* Runtime::loadFrame(std::istream& bcstream) {
	return loadFrame(bcstream, "");
}

MFrame* Runtime::loadFrame(std::istream& bcstream, const std::string prefix) {
	MFrame* frame = new MFrame;
	uint8_t curbyte;

	std::getline(bcstream, frame->name, '\0');

	if(frame->name.empty()) {
		return nullptr;
	}

	frame->name = prefix + frame->name;

	while(true) {
		bcstream.read((char*)&curbyte, 1);
		if(curbyte==0) break;
		switch(curbyte) {
			case OPCODE_ADD:{
				MAddOp *addop = new MAddOp();
				addop->type = (Opcode) curbyte;
				frame->ops.push_back((MOp*)addop);
			}break;
			case OPCODE_SUB:{
				MSubOp *subop = new MSubOp();
				subop->type = (Opcode) curbyte;
				frame->ops.push_back((MOp*)subop);
			}break;
			case OPCODE_MUL:{
				MMulOp *mulop = new MMulOp();
				mulop->type = (Opcode) curbyte;
				frame->ops.push_back((MOp*)mulop);
			}break;
			case OPCODE_DIV:{
				MDivOp *divop = new MDivOp();
				divop->type = (Opcode) curbyte;
				frame->ops.push_back((MOp*)divop);
			}break;
			case OPCODE_PUSH:{
				MPushOp *pushop = new MPushOp();
				pushop->type = (Opcode) curbyte;
				bcstream.read((char*)&pushop->value, 8);
				frame->ops.push_back((MOp*)pushop);
			}break;
			case OPCODE_PUSHV:{
				MPushvOp *pushvop = new MPushvOp();
				pushvop->type = (Opcode) curbyte;
				std::getline(bcstream, pushvop->name, '\0');
				frame->ops.push_back((MOp*)pushvop);
			}break;
			case OPCODE_ASSN:{
				MAssnOp *assnop = new MAssnOp();
				assnop->type = (Opcode) curbyte;
				std::getline(bcstream, assnop->name, '\0');
				frame->ops.push_back((MOp*)assnop);
			}break;
			case OPCODE_RTCL:{
				MRtclOp *rtclop = new MRtclOp();
				rtclop->type = (Opcode) curbyte;
				std::getline(bcstream, rtclop->name, '\0');
				frame->ops.push_back((MOp*)rtclop);
			}break;
			case OPCODE_PUSHF:{
				MPushfOp *pushfop = new MPushfOp();
				pushfop->type = (Opcode) curbyte;
				std::getline(bcstream, pushfop->name, '\0');
				pushfop->name = prefix + pushfop->name;
				frame->ops.push_back((MOp*)pushfop);
			}break;
			case OPCODE_RET:{
				MRetOp *retop = new MRetOp();
				retop->type = (Opcode) curbyte;
				frame->ops.push_back((MOp*)retop);
			}break;
			case OPCODE_PUSHS:{
				MPushsOp *pushsop = new MPushsOp();
				pushsop->type = (Opcode) curbyte;
				std::getline(bcstream, pushsop->str, '\0');
				frame->ops.push_back((MOp*)pushsop);
			}break;
			case OPCODE_CALL:{
				MCallOp *callop = new MCallOp();
				callop->type = (Opcode) curbyte;
				frame->ops.push_back((MOp*)callop);
			}break;
			case OPCODE_POP:{
				MPopOp *popop = new MPopOp();
				popop->type = (Opcode) curbyte;
				frame->ops.push_back((MOp*)popop);
			}break;
			case OPCODE_PUSHB:{
				MPushbOp *pushbop = new MPushbOp();
				pushbop->type = (Opcode) curbyte;
				bcstream.read((char*)&pushbop->value, 1);
				frame->ops.push_back((MOp*)pushbop);
			}break;
			case OPCODE_JN:{
				MJnOp *jnop = new MJnOp();
				jnop->type = (Opcode) curbyte;
				bcstream.read((char*)&jnop->ptr, 4);
				frame->ops.push_back((MOp*)jnop);
			}break;
			case OPCODE_JMP:{
				MJmpOp *jmpop = new MJmpOp();
				jmpop->type = (Opcode) curbyte;
				bcstream.read((char*)&jmpop->ptr, 4);
				frame->ops.push_back((MOp*)jmpop);
			}break;
			case OPCODE_LEQ:
			case OPCODE_GRT:
			case OPCODE_GEQ:
			case OPCODE_LESS:
			case OPCODE_EQ:
			case OPCODE_NEQ:
			case OPCODE_LSH:
			case OPCODE_RSH:
			case OPCODE_AND:
			case OPCODE_OR:
			case OPCODE_UNMAP:
			case OPCODE_PUSHMAP:
			case OPCODE_MASSN:
			case OPCODE_ECALL:{
				MOp *op = new MOp();
				op->type = (Opcode) curbyte;
				frame->ops.push_back(op);
			}break;
		}
	}

	while(true) {
		MFrame* sframe = loadFrame(bcstream, prefix);
		if(sframe == nullptr) break;
		frame->subframes.push_back(sframe);
	}

	return frame;
}

MFrame* Runtime::findFrame(std::vector<MFrame*> framev, const std::string framename) {
	for(MFrame* frame : framev) {
		if(frame->name.compare(framename) == 0) {
			return frame;
		}
	}
	std::cout << "Error: frame \"" + framename + "\" not found" << std::endl;
	exit(2);
	return nullptr;
}

MValue* Runtime::run(const std::string mainname) {
	runFrame(frames, mainname);
	return rstack->top();
}

void Runtime::runFrame(std::vector<MFrame*> ldframes, const std::string framename) {
	MFrame* frame = findFrame(ldframes, framename);
	for(auto i = frame->ops.begin(); i != frame->ops.end(); i++) {
		MOp* op = *i;
		switch(op->type) {
			case OPCODE_PUSH:{
				rstack->push(new MNumberValue(((MPushOp*)op)->value));
			}break;
			case OPCODE_PUSHV:{
				rstack->push(frame->vars[((MPushvOp*)op)->name]);
			}break;
			case OPCODE_ASSN:{
				frame->vars[((MAssnOp*)op)->name] = rstack->top();
				rstack->pop();
			}break;
			case OPCODE_RTCL:{
				nativefunctions[((MRtclOp*)op)->name](rstack);
			}break;
			case OPCODE_PUSHF:{
				rstack->push(new MFunctionValue(((MPushfOp*)op)->name));
			}break;
			case OPCODE_PUSHS:{
				rstack->push(new MStringValue(((MPushsOp*)op)->str));
			}break;
			case OPCODE_CALL:{
				std::string str = *((std::string*) rstack->top()->castTo(MTYPE_FUNCTION)->get());
				rstack->pop();
				Runtime::runFrame(this->frames, str); // TODO: PARENT FRAME CALLS
			}break;
			case OPCODE_RET: return;
			case OPCODE_POP:{
				rstack->pop();
			}break;
			case OPCODE_PUSHB:{
				rstack->push(new MBooleanValue(((MPushbOp*)op)->value));
			}break;
			case OPCODE_ADD:
			case OPCODE_SUB:
			case OPCODE_MUL:
			case OPCODE_DIV:
			case OPCODE_LEQ:
			case OPCODE_GRT:
			case OPCODE_GEQ:
			case OPCODE_LESS:
			case OPCODE_EQ:
			case OPCODE_NEQ:
			case OPCODE_AND:
			case OPCODE_OR:
			case OPCODE_LSH:
			case OPCODE_RSH:{
				MValue *mv2 = rstack->top();
				rstack->pop();
				MValue *mv1 = rstack->top();
				rstack->pop();
				rstack->push(mv1->operate(op->type, mv2));
			}break;
			case OPCODE_JN:{
				uint32_t ptr = static_cast<MJnOp*>(op)->ptr;
				bool condition = *((bool*) rstack->top()->castTo(MTYPE_BOOL)->get());
				rstack->pop();
				if(!condition) {
					i = frame->ops.begin() + ptr - 1; // XXX: - 2 is magic
				}
			}break;
			case OPCODE_JMP:{
				uint32_t ptr = static_cast<MJmpOp*>(op)->ptr;
				i = frame->ops.begin() + ptr - 1; // XXX: - 2 is even more magic
			}break;
			case OPCODE_ECALL:{
				std::string modname = *((std::string*) rstack->top()->castTo(MTYPE_STRING)->get());
				rstack->pop();
				std::ifstream cfile(modname + ".mo", std::ios_base::binary);
				load(dynamic_cast<std::istream&>(cfile), modname + ".");
				MValue* retval = run(modname + ".main");
				/*rstack->push(retval);*/
			}break;
			case OPCODE_PUSHMAP:
				rstack->push(new MMapValue());
			break;
			case OPCODE_MASSN:{
				MValue *key = rstack->top();
				rstack->pop();
				MValue *value = rstack->top();
				rstack->pop();
				dynamic_cast<MMapValue*>(rstack->top()->castTo(MTYPE_MAP))->add(*((std::string*) key->castTo(MTYPE_STRING)->get()), value);
			}break;
			case OPCODE_UNMAP:{
				std::unordered_map<std::string, MValue*>* map = static_cast<std::unordered_map<std::string, MValue*>*>(rstack->top()->castTo(MTYPE_MAP)->get());
				rstack->pop();
				for(auto &it : *map) {
					frame->vars[it.first] = dynamic_cast<MValue*>(it.second);
				}
			}break;
		}
	}
}
Runtime::~Runtime() {
	/*delete rstack;
	delete bcstream;*/
}
