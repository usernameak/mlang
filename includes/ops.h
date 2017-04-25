#pragma once

#include <cstdint>
#include <string>
#include <stack>
#include "types.h"
#include "vals.h"
#include "opcodes.h"


namespace mlang {
	namespace ops_n {
		class base {
		public:
			Opcode type;
			virtual void execute(runtime_stack&);
		};

		class type_operator : public base {
		public:
			void execute(runtime_stack&);
		};

		/*class addop : public type_operator {};
		class subop : public type_operator {};
		class mulop : public type_operator {};
		class divop : public type_operator {};
		class leqop : public type_operator {};
		class grtop : public type_operator {};
		class geqop : public type_operator {};
		class lessop : public type_operator {};
		class eqop : public type_operator {};
		class neqop : public type_operator {};
		class lshop : public type_operator {};
		class rshop : public type_operator {};
		class andop : public type_operator {};
		class orop : public type_operator {};*/

		class pushop : public base {
		public:
			double value;
			void execute(runtime_stack&);
		};

		class pushvop : public base {
		public:
			std::string name;
		};

		class assnop : public base {
		public:
			std::string name;
		};

		class rtclop : public base {
		public:
			std::string name;
		};

		class pushfop : public base {
		public:
			std::string name;
		};

		class retop : public base {};

		class pushsop : public base {
		public:
			std::string str;
			void execute(runtime_stack&);
		};

		class callop : public base {};

		class popop : public base {
		public:
			void execute(runtime_stack&);
		};

		class pushbop : public base {
		public:
			bool value;
			void execute(runtime_stack&);
		};



		class jnop : public base {
		public:
			uint32_t ptr;
		};

		class jmpop : public base {
		public:
			uint32_t ptr;
		};

		class unmapop : public base {};
		class pushmapop : public base {
		public:
			void execute(runtime_stack&);
		};

		class massnop : public base {};
		class ecallop : public base {};

	}
}
