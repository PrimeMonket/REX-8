#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>
#include <string>

bool cstringStartsWith(char *cstring, char *startsWith) {
    return strncmp(cstring, startsWith, strlen(startsWith)) == 0;
}

// Function to split a string into tokens
const std::vector<std::string> splitStrAt(std::string string, char character = ' ') {
    std::vector<std::string> tokens;
    std::string currentToken;

    if(string.empty()) return {}; // Empty line, ignore
    if (string.at(0) == ';') return {}; // Comment, ignore

    for (char ch : string) {
        if (ch == character) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        }
        else if (ch == ';') {
            // Comment, ignore
            return tokens;
        }
        else {
            currentToken += ch;
        }
    }

    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }

    return tokens;
}

// Convert an instruction line into opcodes
std::vector<std::string> compute(std::string instruction) {
    std::vector<std::string> opcodes;
    const std::vector<std::string> tokens = splitStrAt(instruction);

    if (tokens.empty()) return {}; // Probably a comment, ignore

    // Translate instructions into opcodes
    if (tokens[0] == "HALT") opcodes.push_back("0xFF");
    else if (tokens[0] == "LOAD") opcodes.push_back("0x01,");
    else if (tokens[0] == "MEM_LOAD") opcodes.push_back("0x02,");
    else if (tokens[0] == "ADD") opcodes.push_back("0x03,");
    else if (tokens[0] == "ADD_REG") opcodes.push_back("0x04,");
    else if (tokens[0] == "SUB") opcodes.push_back("0x05,");
    else if (tokens[0] == "SUB_REG") opcodes.push_back("0x06,");
    else if (tokens[0] == "MUL") opcodes.push_back("0x07,");
    else if (tokens[0] == "MUL_REG") opcodes.push_back("0x08,");
    else if (tokens[0] == "DIV") opcodes.push_back("0x09,");
    else if (tokens[0] == "DIV_REG") opcodes.push_back("0x10,");
    else if (tokens[0] == "JMP") opcodes.push_back("0x11,");
    else if (tokens[0] == "JMPZ") opcodes.push_back("0x12,");
    else if (tokens[0] == "JMPNZ") opcodes.push_back("0x13,");
    else if (tokens[0] == "INPUT") opcodes.push_back("0x14,");
    else if (tokens[0] == "OUTPUT") opcodes.push_back("0x15,");
    else if (tokens[0] == "COMPARE") opcodes.push_back("0x16,");
    else if (tokens[0] == "COMPARE_REG") opcodes.push_back("0x17,");
    else if (tokens[0] == "STORE") opcodes.push_back("0x18,");
    else if (tokens[0] == "MEM_STORE") opcodes.push_back("0x19,");
    else {
        std::cerr << "Error: Unknown instruction " << tokens[0] << std::endl;
        return { "err" };
    }

    // I know that could have been done with a for() loop, but eh, if it works it works
    if (tokens[0] != "HALT") {
        if (tokens.size() == 1) opcodes.push_back("0x00, 0x00,");
        else if (tokens.size() == 2) {
            // 1st and only
            if(tokens[1] == "ACC") {
                opcodes.push_back("0x01, 0x00,");
            }
            else if (tokens[1][0] == '0' && tokens[1][1] == 'x' && tokens[1].size() == 4) {
                opcodes.push_back(tokens[1] + ", 0x00");
            }
            else if (tokens[1].size() != 2) {
                std::cerr << "Error: Expected 2 charcaters, got " << tokens[1].size() - 1 << " characters." << std::endl;
                return { "err" };
            }
            else {
                opcodes.push_back(("0x0" + std::string(1, tokens[1][1] + 1) + ", 0x00,"));
            }
        }
        else if (tokens.size() != 3) {
            std::cerr << "Error: Expected 2 registers, got " << tokens.size() - 1 << " registers." << std::endl;
            return { "err" };
        }
        else {
            // 1st register
            if(tokens[1] == "ACC,") {
                opcodes.push_back("0x01,");
            }
            else if (tokens[1][0] == '0' && tokens[1][1] == 'x' && tokens[1].size() == 4) {
                opcodes.push_back(tokens[1] + ",");
            }
            else if (tokens[1].size() != 3) {
                std::cerr << std::endl << "Error: Expected 1st register to have 3 characters, got " << tokens[1].size() << " characters." << std::endl;
                return { "err" };
            }
            else {
                opcodes.push_back(("0x0" + std::string(1, tokens[1][1] + 1) + ","));
            }
            // 2nd register
            if(tokens[2] == "ACC") {
                opcodes.push_back("0x01,");
            }
            else if (tokens[2][0] == '0' && tokens[2][1] == 'x' && tokens[2].size() == 4) {
                opcodes.push_back(tokens[2] + ",");
            }
            else if (tokens[2].size() != 2) {
                std::cerr << std::endl << "Error: Expected 2nd register to have 3 characters, got " << tokens[2].size() << " characters." << std::endl;
                return { "err" };
            }
            else {
                opcodes.push_back(("0x0" + std::string(1, tokens[2][1] + 1) + ","));
            }
        }
    }

    return opcodes;
}

void printHelp(char *command) {
    std::cout << "Usage: " << command << " <input file> <output file>" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string inputFileName = "input.asm";
    std::string outputFileName = "program.bin";

    if(argc != 3 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        printHelp(argv[0]);
        return 0;
    } else {
        const std::string arg1 = argv[1], arg2 = argv[2];
        int inForArg;
        for(int i = 1; i<=2; i++) {
            if(cstringStartsWith(argv[i], (char *)"--in=")) {
                inputFileName = ((i == 1) ? arg1 : arg2).substr(5);
                inForArg = i;
            }
        }

        if(cstringStartsWith(argv[(inForArg == 1) ? 2 : 1], (char *)"--out=")) outputFileName = ((inForArg == 1) ? arg2 : arg1).substr(6);
        else {
            printHelp(argv[0]);
            return 1;
        }
    }

    // Open assembly file for reading
    std::ifstream instructionsFile(inputFileName);
    if (!instructionsFile.is_open()) {
        std::cerr << "Error: Could not open input.asm for reading." << std::endl;
        return 1;
    }

    // Open binary file for writing
    std::ofstream outputFile(outputFileName, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open program.bin for writing." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(instructionsFile, line)) {
        const std::vector<std::string> opcodes = compute(line);
        if (opcodes.empty()) continue; // Probably a comment, ignore

        if (opcodes[0] == "err") {
            std::cerr << "An error occurred while processing the file." << std::endl;
            return 1;
        }

        // Debug: Print opcodes to console
        for (const auto& opcode : opcodes) {
            std::cout << opcode << " ";
        }
        std::cout << std::endl;

        // Write opcodes to binary file
        for (const auto& opcode : opcodes) {
            std::string trimmedOpcode = opcode;
            if (opcode.back() == ',') trimmedOpcode.pop_back(); // Remove trailing comma
            uint8_t byte = std::stoi(trimmedOpcode, nullptr, 16); // Convert to byte
            outputFile.write(reinterpret_cast<const char*>(&byte), 1);
        }
    }

    instructionsFile.close();
    outputFile.close();

    std::cout << "Output successfully written to " << outputFileName << std::endl;
    return 0;
}
