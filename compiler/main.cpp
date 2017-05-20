#include "scanner.h"
#include <iostream>
#include "parser.h"

int main(int argc, char **argv) {
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
    }
    Scanner scanner;
    if(!scanner.OpenFile(argv[1])) {
        return 1;
    }
    Parser parser(scanner);
    parser.Parse();
    scanner.Close();
}
