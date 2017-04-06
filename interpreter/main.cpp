#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>
#include "runtime.h"
#include "vals.h"
int main(int argc, char **argv) {
	if(argc < 2 || argc > 3) {
		std::cerr << std::flush << "usage: mlang [-v] <compiled source name>" << std::endl;
		return 1;
	}
	bool verbose = false;
	if(argc == 3) {
		if(!strcmp(argv[1], "-v")) {
			verbose = true;
		} else {
			std::cerr << std::flush << "usage: mlang [-v] <compiled source name>" << std::endl;
			return 1;
		}
	}
	char *filename = argv[argc == 3 ? 2 : 1];
	std::ifstream* cfile = new std::ifstream(filename, std::ios_base::binary);
	Runtime* rt = new Runtime(cfile);
	rt->load();
	std::chrono::steady_clock::time_point begin;
	if(verbose) {
		begin = std::chrono::steady_clock::now();
	}
	rt->run();
	if(verbose) {
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		std::cout << "Run time: " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << " nanoseconds" << std::endl;
	}
}