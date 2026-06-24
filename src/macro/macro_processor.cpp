#include "macro_processor.h"
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

std::vector<std::string> MacroProcessor::step1(const std::vector<std::string>& sourceLines){
    bool inDefinitionMode = false;
    std::vector<std::string> formalParams;
    std::vector<std::string> intermediateCode;

    for(const auto& line : sourceLines){
        std::string cleanLine = trim( line );
        
        if( cleanLine.empty() ){
            if (!inDefinitionMode) {
                intermediateCode.push_back( cleanLine );
            }
            continue;
        }

        std::vector<std::string> tokens = tokenize( cleanLine );

        if( !inDefinitionMode )
        {   
            //teste para não acessar memória que não existe
            if (tokens.size() >= 2 && tokens[1] == "MACRO") {
                inDefinitionMode = true;
                macroDefinitionTable.push_back( std::vector<std::string>() );
                macroNameTable[tokens[0]] = macroDefinitionTable.size() - 1;

                formalParams.clear();
                
                for( size_t i = 2; i < tokens.size(); i++ ) {
                    formalParams.push_back( tokens[i] );
                }  

            } else {
                intermediateCode.push_back( line );
            }

        } else if( inDefinitionMode ) {

            if( tokens[0] == "MEND" || tokens[0] == "ENDM") {

                inDefinitionMode = false;
                formalParams.clear();

            } else {
                std::string processedLine = line;
                
                // substitui parametros formais
                for(size_t i = 0; i < formalParams.size(); i++) {
                    replaceAll(processedLine, formalParams[i], "?" + std::to_string(i));
                }   
            
                macroDefinitionTable.back().push_back( processedLine );
            }
        }
    }

    return intermediateCode;
}

std::string MacroProcessor::trim(std::string line) {
    size_t begin  = line.find_first_not_of(" \t\n\r");
    size_t end    = line.find_last_not_of(" \t\n\r");

    if (begin == std::string::npos) {
        return "";
    }

    return line.substr(begin, end - begin + 1);
}

std::vector<std::string> MacroProcessor::tokenize(std::string line) {
    std::string token;
    std::vector<std::string> tokens;

    std::replace(line.begin(), line.end(), ',', ' ');
    std::stringstream ss(line);
    
    while (ss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

void MacroProcessor::replaceAll(std::string& str, const std::string& target, const std::string& replacement) {
    if (target.empty()) return;
    
    size_t startPos = 0;
    while((startPos = str.find(target, startPos)) != std::string::npos)
    {
        str.replace(startPos, target.length(), replacement);
        startPos += replacement.length();
    }
}