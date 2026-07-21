#include "linker.h"
#include <iostream>
#include <string>

#include <iostream>
#include <vector>
#include <string>

static void invalidCommand(){
    std::cerr << "Uso: ./linker <arquivo1.o> <arquivo2.o> (inserir todos os arquivos .o) [-r <endereco_base>] [-o <arquivo_saida>]\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        invalidCommand();
        return 1;
    }

    std::vector<std::string> input_files;
    std::string output_file;
    int baseAddress = 0;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-r") {
            if (i + 1 > argc) {
                invalidCommand();
                return 1;
            }
            baseAddress = std::stoi(argv[++i]);
        } else if( arg == "-o" ) {
            if( i + 1 > argc ){
                invalidCommand();
                return 1;
            }
            output_file = argv[++i];    
        } else {
            input_files.push_back(arg);
        }
    }

    if (input_files.empty()) {
        invalidCommand();
        return 1;
    }

    if( output_file.empty() ){
        output_file = "a.out";
    }

    try {
        Linker linker(input_files, baseAddress);
        linker.Link(output_file);
    } catch (const std::exception& e) {
        std::cerr << "Erro durante a ligação: " << e.what() << "\n";
        return 1;
    }

    return 0;
}