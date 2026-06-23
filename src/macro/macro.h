#pragma once

#include <string>
#include <vector>

struct Macro {
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::string> body;
};

class MacroProcessor {
public:
    MacroProcessor(const std::string& inputFileName, const std::string& outputFileName);
    void process();

private:
    std::string inputFileName;
    std::string outputFileName;

    std::vector<std::string> program;

    void loadFile();
    void saveFile(const std::vector<std::string>& preprocessedProgram);
};