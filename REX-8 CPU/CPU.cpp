#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <string>
#include <iomanip>

class CPU {
public:
    int ACC = 0;
    int PC = 0;

    int R1 = 0;
    int R2 = 0;
    int R3 = 0;
    int R4 = 0;
    int R5 = 0;
    int R6 = 0;
    int R7 = 0;
    int R8 = 0;

    bool ZF = false;
    bool NF = false;
    bool CF = false;

    bool verbose = false;
    bool dumpmemory = false;
    bool showprogram = false;

    int progam_size = 0;
    std::vector<uint8_t> memory = std::vector<uint8_t>(1024, 0);

    void initializememory(const std::vector<uint8_t>& program) {
        if (program.size() > memory.size()) {
            throw std::runtime_error("program too large");
        }

        for (size_t i = 0; i < program.size(); ++i) {
            memory[i] = program[i];
        }

        progam_size = program.size();
        std::cout << "memory initialized\n" << std::endl;
    }

    void printProgram() {
        std::cout << "Loaded Program:\n";

        for (size_t i = 0; i < progam_size; i += 3) {
            std::cout << "@" << std::hex << i << ": ";
            for (size_t j = 0; j < 3 && i + j < progam_size; ++j) {
                std::cout << std::hex << (int)memory[i + j] << " ";
            }
            std::cout << "\n";
        }
    }

    void printMemoryvalues() {
        std::cout << "memory contents " << std::endl;
        for (size_t i = 0; i < memory.size(); ++i) {
            if (memory[i] != 0) {
                std::cout << "arg2ess " << i << ": " << (int)memory[i] << std::endl;
            }
        }
    }

    void memory_out_of_range() {
        throw std::out_of_range("Memory arg2ess out of range");
    }

    void execute() {

        //displays the bytres of the load program good for cheking for corrupt programs
        if (showprogram) {
            printProgram();
        }

        // nicer formating for verbose falg for easy reading
        if (verbose) {
            std::cout << "\nExecuting:" << std::endl;
        }

        while (PC < memory.size()) {
            uint8_t opcode = memory[PC];
            uint8_t arg1 = memory[PC + 1];
            uint8_t arg2 = memory[PC + 2];

            //part of the verbose flag to analize what instruction is running or could be caysing a issue
            if (verbose) {
                std::cout << "  Opcode: 0x" << std::hex << static_cast<int>(opcode) << std::endl;
            }

            try {
                switch (opcode) {
                case 0x01:  // LOAD
                    loadreg(arg1, arg2);
                    break;

                case 0x02: // MEM_LOAD
                    loadMem(arg1, arg2);
                    break;

                case 0x03:  // ADD
                    addreg(arg1, arg2);
                    break;

                case 0x04: // ADD_REG
                    addregs(arg1, arg2);
                    break;

                case 0x05:  // SUB
                    subtractreg(arg1, arg2);
                    break;

                case 0x06:  // SUB_REG
                    subtractregs(arg1, arg2);
                    break;

                case 0x07:  // MUL
                    MultiplyRegister(arg1, arg2);
                    break;

                case 0x08:  // MUL_REG
                    multiplyregs(arg1, arg2);
                    break;

                case 0x09:  // DIV
                    dividereg(arg1, arg2);
                    break;

                case 0x10:  // DIV_REG
                    divideRegistrys(arg1, arg2);
                    break;

                case 0x11:  // JMP
                    jumptoarg2ess(arg2);
                    continue;

                case 0x12:  // JMPZ
                    jumpifzero(arg1);
                    if (getRegister_value(arg1) != 0) {
                        PC += 3;
                    }
                    continue;

                case 0x13:  // JMPNZ
                    jumpifnotzero(arg1);
                    if (getRegister_value(arg1) == 0) {
                        PC += 3;
                    }
                    continue;

                case 0x14:  // INPUT
                    inputtoreg(arg1);
                    break;

                case 0x15:  // OUTPUT
                    outputRegister(arg1);
                    break;

                case 0x16: // COMPARE
                    comparereg(arg1, arg2);
                    break;

                case 0x17: // COMPARE_REG
                    compareRegs(arg1, arg2);
                    break;

                case 0x18:  // STORE
                    storeRegister(arg1, arg2);
                    break;

                case 0x19:  // MEM_STORE
                    store_memory(arg2, memory[PC + 3]);
                    break;

                case 0xFF:  // HALT
                    return;

                default:
                    std::cerr << "unknown opcode " << opcode << "\n";
                }
            }
            catch (const std::exception& err) {
                std::cerr << "error executing " << err.what() << std::endl;
                return;
            }

            PC += 3;
        }
        if (dumpmemory) {
            printMemoryvalues();
        }
    }


private:
    void loadreg(uint8_t arg1, int arg2) {
        setregvalue(arg1, arg2);
    }

    // loads value stored in memory to a registry
    void loadMem(uint8_t arg1, int arg2) {
        if (arg2 >= memory.size()) {
            memory_out_of_range();
        }

        setregvalue(arg1, memory[arg2]);
    }

    // stores reg value into a memory addres 
    void storeRegister(uint8_t arg1, int arg2) {
        if (arg2 >= memory.size()) {
            memory_out_of_range();
        }

        memory[arg2] = getRegister_value(arg1);
    }

    // store a value directly into a memory address
    void store_memory(uint8_t arg2, uint8_t value) {
        if (arg2 >= memory.size()) {
            memory_out_of_range();
        }

        memory[arg2] = value;
    }

    void addreg(uint8_t arg1, int arg2) {
        setregvalue(arg1, getRegister_value(arg1) + arg2);
    }

    void addregs(uint8_t arg1, uint8_t arg2) {
        setregvalue(arg1, (getRegister_value(arg1) + getRegister_value(arg2)));
    }

    void subtractreg(uint8_t arg1, int arg2) {
        setregvalue(arg1, getRegister_value(arg1) - arg2);
    }

    void subtractregs(uint8_t arg1, uint8_t arg2) {
        setregvalue(arg1, (getRegister_value(arg1) - getRegister_value(arg2)));
    }

    void MultiplyRegister(uint8_t arg1, int arg2) {
        setregvalue(arg1, getRegister_value(arg1) * arg2);
    }

    void multiplyregs(uint8_t arg1, uint8_t arg2) {
        setregvalue(arg1, (getRegister_value(arg1) * getRegister_value(arg2)));
    }

    void dividereg(uint8_t arg1, int arg2) {
        if (arg2 == 0) throw std::invalid_argument("dividing by zero");

        setregvalue(arg1, getRegister_value(arg1) / arg2);
    }

    void divideRegistrys(uint8_t arg1, uint8_t arg2) {
        if (arg2 == 0) {
            std::cerr << "trying to divide by zero" << std::endl;
            return;
        }

        setregvalue(arg1, getRegister_value(arg1) / getRegister_value(arg2));
    }

    // jump to a memory adress
    void jumptoarg2ess(int arg2) {
        if (arg2 >= memory.size()) {
            memory_out_of_range();
        }

        PC = arg2;
    }

    void jumpifzero(uint8_t arg1) {
        if (getRegister_value(arg1) == 0) {
            jumptoarg2ess(memory[PC + 2]);
        }
        else {
            PC += 3;
        }
    }

    void inputtoreg(uint8_t arg1) {
        int inputValue;

        std::cout << "Enter a interger: ";
        std::cin >> inputValue;

        setregvalue(arg1, inputValue);
    }

    void jumpifnotzero(uint8_t arg1) {
        if (getRegister_value(arg1) != 0) {
            jumptoarg2ess(memory[PC + 2]);
        }
        else {
            PC += 3;
        }
    }

    //compare a value against a regisrty
    // if both values are the same then set the zero flag to be true
    // if the value in arg2 is bigger then reggistry value in arg1 them set
    // the negative and carry flag to ture
    void comparereg(uint8_t arg1, int arg2) {
        if (getRegister_value(arg1) == arg2) {
            ZF = true;
            NF = false;
            CF = false;
        }
        else if (getRegister_value(arg1) < arg2) {
            ZF = false;
            NF = true;
            CF = true;
        }
        else {
            ZF = false;
            NF = false;
            CF = false;
        }
    }

    // same logic as above but with t wo registers 
    void compareRegs(uint8_t arg1, uint8_t arg2) {
        if (getRegister_value(arg1) == getRegister_value(arg2)) {
            ZF = true;
            NF = false;
            CF = false;
        }
        else if (getRegister_value(arg1) < getRegister_value(arg2)) {
            ZF = false;
            NF = true;
            CF = true;
        }
        else {
            ZF = false;
            NF = false;
            CF = false;
        }
    }

    void outputRegister(uint8_t arg1) {
        std::cout << "registry: " << (int)arg1 << " has value: " << getRegister_value(arg1) << std::endl;
    }

    int getRegister_value(uint8_t arg1) {
        switch (arg1) {
        case 0x01: return ACC;
        case 0x02: return R1;
        case 0x03: return R2;
        case 0x04: return R3;
        case 0x05: return R4;
        case 0x06: return R5;
        case 0x07: return R6;
        case 0x08: return R7;
        case 0x09: return R8;
        default: throw std::invalid_argument("invalid reg index");
        }
    }

    void setregvalue(uint8_t arg1, int value) {
        switch (arg1) {
        case 0x01: ACC = value; break;
        case 0x02: R1 = value; break;
        case 0x03: R2 = value; break;
        case 0x04: R3 = value; break;
        case 0x05: R4 = value; break;
        case 0x06: R5 = value; break;
        case 0x07: R6 = value; break;
        case 0x08: R7 = value; break;
        case 0x09: R8 = value; break;
        default: throw std::invalid_argument("invalid reg index");
        }
    }
};

std::vector<uint8_t> loadProgramFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::ios_base::failure("failed to open program file " + filename);
    }

    std::vector<uint8_t> program((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    std::cout << "program loaded with size: " << program.size() << " bytes out of 1024 bytes" << std::endl;
    return program;
}

int main(int argc, char* argv[]) {
    CPU cpu;
    std::string programFile;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-v") {
            cpu.verbose = true;
        }
        else if (arg == "-m") {
            cpu.dumpmemory = true;
        }
        else if (arg == "-p") {
            cpu.showprogram = true;
        }
        else if (arg.substr(0, 5) == "--in=") {
            programFile = arg.substr(5);
        }
        else {
            std::cerr << "unknown argument " << arg << std::endl;
            return 1;
        }
    }

    if (programFile.empty()) {
        std::cerr << "no input file specified to execute" << std::endl;
        return 1;
    }

    std::vector<uint8_t> program = loadProgramFromFile(programFile);
    cpu.initializememory(program);

    cpu.execute();

    return 0;
}
