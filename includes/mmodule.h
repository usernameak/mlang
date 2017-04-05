#ifdef _WIN32

	#define mlang_module(funcnames...) extern "C" {\
		char** mlang_nativefunctions_list = (char *[]){funcnames};\
		int mlang_nativefunctions_count = sizeof(mlang_nativefunctions_list)/sizeof(mlang_nativefunctions_list[0]);\
	}\
	extern "C"

	#define mlang_function(name) void __declspec(dllexport) name(std::stack<MValue*>* stack)

#else

	#define mlang_module(funcnames...) extern "C" {\
		char** mlang_nativefunctions_list = (char *[]){funcnames};\
		int mlang_nativefunctions_count = sizeof(mlang_nativefunctions_list)/sizeof(mlang_nativefunctions_list[0]);\
	}\
	extern "C"

	#define mlang_function(name) void name(std::stack<MValue*>* stack)
	
#endif