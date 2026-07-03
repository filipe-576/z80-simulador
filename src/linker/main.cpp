#include "linker.h"
#include <string>
#include <iostream>

#ifndef PROJ_DIR
#define PROJ_DIR "."
#endif

int main() {

    std::string jsonPath = std::string(PROJ_DIR) + "/src/linker/modules.json";
    Linker linker(jsonPath);
    linker.Link();
    linker.linkDebug();

    return 0;
}