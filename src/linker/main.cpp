#include "linker.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 4) {
        std::cerr << "Uso: ./linker <arquivo.o> [-r <endereco_base>]\n";
        return 1;
    }

    std::string input_file = argv[1];
    int baseAddress = (argc == 4 && std::string(argv[2]) == "-r") ? std::stoi(argv[3]) : 0;

    std::string output_file;
    size_t last_dot = input_file.find_last_of(".");
    if (last_dot != std::string::npos && input_file.substr(last_dot) == ".o") {
        output_file = input_file.substr(0, last_dot) + ".out";
    } else {
        output_file = input_file + ".out";
    }

    try {
        Linker linker(input_file, baseAddress);
        linker.Link(output_file);
    } catch (const std::exception& e) {
        std::cerr << "Erro durante a ligação: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
