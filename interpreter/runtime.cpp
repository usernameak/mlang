// FIXME: Very baaad code

#include "runtime.h"

#include <iostream>
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
#include "types.h"
#include "runtime_state.h"

using namespace mlang;

std::map<std::string, void(*)(runtime_stack*)> nativefunctions;

#ifdef _WIN32
	void nf_nativeload(runtime_stack* stack) {
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
			void (*func)(runtime_stack*) = (void(*)(runtime_stack*)) GetProcAddress(handle, list[i]);
			if(func == nullptr) {
				std::cout << "Failed to load library " << libname <<
					": function " << list[i] << " cannot be loaded" << std::endl;
				exit(1);
			}
			nativefunctions[list[i]] = func;
		}
	}
#else
	void nf_nativeload(runtime_stack* stack) {
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
			void (*func)(runtime_stack*) = (void(*)(runtime_stack*)) dlsym(handle, list[i]);
			if(func == nullptr) {
				std::cout << dlerror() << std::endl;
				exit(1);
			}
			nativefunctions[list[i]] = func;
		}
	}
#endif

runtime::runtime() : rstack(new runtime_stack()) {
	NATIVEFUNC_SET("nativeload", nf_nativeload);
}

void runtime::load(std::istream& bcstream, const std::string prefix) {
	while(true) {
		MFrame* frame = loadFrame(bcstream, prefix);
		if(frame == nullptr) return;
		frames.push_back(frame);
	}
}

MFrame* runtime::loadFrame(std::istream& bcstream, const std::string prefix) {
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

			case OPCODE_PUSH:{
				ops_n::pushop *pushop = new ops_n::pushop();
				pushop->type = (Opcode) curbyte;
				bcstream.read((char*)&pushop->value, 8);
				frame->ops.push_back((ops_n::base*)pushop);
			}break;
			case OPCODE_PUSHV:{
				ops_n::pushvop *pushvop = new ops_n::pushvop();
				pushvop->type = (Opcode) curbyte;
				std::getline(bcstream, pushvop->name, '\0');
				frame->ops.push_back((ops_n::base*)pushvop);
			}break;
			case OPCODE_ASSN:{
				ops_n::assnop *assnop = new ops_n::assnop();
				assnop->type = (Opcode) curbyte;
				std::getline(bcstream, assnop->name, '\0');
				frame->ops.push_back((ops_n::base*)assnop);
			}break;
			case OPCODE_RTCL:{
				ops_n::rtclop *rtclop = new ops_n::rtclop();
				rtclop->type = (Opcode) curbyte;
				std::getline(bcstream, rtclop->name, '\0');
				frame->ops.push_back((ops_n::base*)rtclop);
			}break;
			case OPCODE_PUSHF:{
				ops_n::pushfop *pushfop = new ops_n::pushfop();
				pushfop->type = (Opcode) curbyte;
				std::getline(bcstream, pushfop->name, '\0');
				pushfop->name = prefix + pushfop->name;
				frame->ops.push_back((ops_n::base*)pushfop);
			}break;
			case OPCODE_RET:{
				ops_n::retop *retop = new ops_n::retop();
				retop->type = (Opcode) curbyte;
				frame->ops.push_back((ops_n::base*)retop);
			}break;
			case OPCODE_PUSHS:{
				ops_n::pushsop *pushsop = new ops_n::pushsop();
				pushsop->type = (Opcode) curbyte;
				std::getline(bcstream, pushsop->str, '\0');
				frame->ops.push_back((ops_n::base*)pushsop);
			}break;
			case OPCODE_CALL:{
				ops_n::callop *callop = new ops_n::callop();
				callop->type = (Opcode) curbyte;
				frame->ops.push_back((ops_n::base*)callop);
			}break;
			case OPCODE_POP:{
				ops_n::popop *popop = new ops_n::popop();
				popop->type = (Opcode) curbyte;
				frame->ops.push_back((ops_n::base*)popop);
			}break;
			case OPCODE_PUSHB:{
				ops_n::pushbop *pushbop = new ops_n::pushbop();
				pushbop->type = (Opcode) curbyte;
				bcstream.read((char*)&pushbop->value, 1);
				frame->ops.push_back((ops_n::base*)pushbop);
			}break;
			case OPCODE_JN:{
				ops_n::jnop *jnop = new ops_n::jnop();
				jnop->type = (Opcode) curbyte;
				bcstream.read((char*)&jnop->ptr, 4);
				frame->ops.push_back((ops_n::base*)jnop);
			}break;
			case OPCODE_JMP:{
				ops_n::jmpop *jmpop = new ops_n::jmpop();
				jmpop->type = (Opcode) curbyte;
				bcstream.read((char*)&jmpop->ptr, 4);
				frame->ops.push_back((ops_n::base*)jmpop);
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
			case OPCODE_LSH:
			case OPCODE_RSH:
			case OPCODE_AND:
			case OPCODE_OR:{
				ops_n::type_operator *op = new ops_n::type_operator();
				op->type = (Opcode) curbyte;
				frame->ops.push_back(op);
			}break;
			case OPCODE_UNMAP:
			case OPCODE_MASSN:
			case OPCODE_ECALL:{
				ops_n::base *op = new ops_n::base();
				op->type = (Opcode) curbyte;
				frame->ops.push_back(op);
			}break;
			case OPCODE_PUSHMAP:{
				ops_n::pushmapop *op = new ops_n::pushmapop();
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

MFrame* runtime::findFrame(std::vector<MFrame*> framev, const std::string framename) {
	for(MFrame* frame : framev) {
		if(frame->name.compare(framename) == 0) {
			return frame;
		}
	}
	std::cout << "Error: frame \"" + framename + "\" not found" << std::endl;
	exit(2);
	return nullptr;
}

MValue* runtime::run(const std::string mainname) {
	runFrame(frames, mainname);
	return rstack->top();
}

void runtime::runFrame(std::vector<MFrame*> ldframes, const std::string framename) {
	state.push_scope();
	MFrame* frame = findFrame(ldframes, framename);
	for(auto i = frame->ops.begin(); i != frame->ops.end(); i++) {
		ops_n::base* op = *i;
		switch(op->type) {
			case OPCODE_PUSH:
			case OPCODE_PUSHS:
			case OPCODE_POP:
			case OPCODE_PUSHB:
			case OPCODE_PUSHMAP:
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
			case OPCODE_RSH:
				op->execute(*rstack);
			break;
			case OPCODE_PUSHV:{;
				rstack->push(state.get_var(((ops_n::pushvop*)op)->name));
			}break;
			case OPCODE_ASSN:{
				state.set_var(((ops_n::assnop*)op)->name, rstack->top());
				rstack->pop();
			}break;
			case OPCODE_RTCL:{
				nativefunctions[((ops_n::rtclop*)op)->name](rstack);
			}break;
			case OPCODE_PUSHF:{
				rstack->push(new MFunctionValue(((ops_n::pushfop*)op)->name));
			}break;
			case OPCODE_CALL:{
				std::string str = *((std::string*) rstack->top()->castTo(MTYPE_FUNCTION)->get());
				rstack->pop();
				runFrame(this->frames, str);
			}break;
			case OPCODE_RET:
				state.pop_scope();
			return;
			case OPCODE_JN:{
				uint32_t ptr = static_cast<ops_n::jnop*>(op)->ptr;
				bool condition = *((bool*) rstack->top()->castTo(MTYPE_BOOL)->get());
				rstack->pop();
				if(!condition) {
					i = frame->ops.begin() + ptr - 1;
				}
			}break;
			case OPCODE_JMP:{
				uint32_t ptr = static_cast<ops_n::jmpop*>(op)->ptr;
				i = frame->ops.begin() + ptr - 1;
			}break;
			case OPCODE_ECALL:{
				std::string modname = *((std::string*) rstack->top()->castTo(MTYPE_STRING)->get());
				rstack->pop();
				std::ifstream cfile(modname + ".mo", std::ios_base::binary);
				load(dynamic_cast<std::istream&>(cfile), modname + ".");
				MValue* retval = run(modname + ".main");
			}break;
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
					state.set_var(it.first, dynamic_cast<MValue*>(it.second));
				}
			}break;
		}
	}
	state.pop_scope();
}
runtime::~runtime() {
	delete rstack;
}
