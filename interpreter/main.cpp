#include <fstream>
#include <iostream>
#include "runtime.h"
int main(int argc, char **argv) {
	if(argc != 2) {
		std::cout << std::flush << "usage: mlang <compiled source name>" << std::endl;
		return 1;
	}
	std::ifstream* cfile = new std::ifstream(argv[1]);
	Runtime* rt = new Runtime(cfile);
	rt->run();
}