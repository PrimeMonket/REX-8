<div align="center">
  <h1 style="border: none;">CPU Emulator User Guide</h1>
</div>

## 1. Overview

This CPU emulator is a simple, programmable CPU written in C++. It features:

- 8 general-purpose registers (`R1-R8`), an accumulator (`ACC`), and program counter (`PC`).
- Memory size of 1024 bytes.
- Arithmetic, logical, and control operations.
- Flags for Zero (`ZF`), Negative (`NF`), and Carry (`CF`).
<br>

The emulator executes programs in binary format (.`bin`), with each instruction consisting of 3 bytes:

- **Opcode**: The operation to perform identified by the [Opcode value](#instruction-set).
- **arg1**: The register or memory address to use.
- **arg2**: Additional data or a second operand.
<br>

## 2. Writing a Program
### Instruction Set
Here are the supported opcodes and their descriptions:

| Opcode | Instruction  | Description                                                                       |
|--------|--------------|-----------------------------------------------------------------------------------|
| 0x01   | LOAD         | Load a value into a register.                                                     |
| 0x02   | MEM_LOAD     | Load a value from memory to a register.                                           |
| 0x03   | ADD          | Add a value to a register.                                                        |
| 0x04   | ADD_REG      | Add the value of one register to another storing it in the first register.        |
| 0x05   | SUB          | Subtract a value from a register.                                                 |
| 0x06   | SUB_REG      | Subtract the value of one register from another storing it in the first register. |
| 0x07   | MUL          | Multiply a register value by a interger.                                          |
| 0x08   | MUL_REG      | Multiply the values of two registers together stroing it in the first register.   |
| 0x09   | DIV          | Divide a register value by a interger.                                            |
| 0x10   | DIV_REG      | Divide the values of two registers storing it in the first register.              |
| 0x11   | JMP          | Jump to a memory address.                                                         |
| 0x12   | JMPZ         | Jump if the register value is zero.                                               |
| 0x13   | JMPNZ        | Jump if the register value is not zero.                                           |
| 0x14   | INPUT        | Take user input and store it in a register.                                       |
| 0x15   | OUTPUT       | Print the value of a register to the console.                                     |
| 0x16   | COMPARE      | Compare a register value with a interger.                                         |
| 0x17   | COMPARE_REG  | Compare the values of two registers.                                              |
| 0x18   | STORE        | Store the value of a register into memory.                                        |
| 0x19   | MEM_STORE    | Store a value in memory at a specific address.                                    |
| 0xFF   | HALT         | Stop program execution.                                                           |


### Example Program
Programs can be written in two forms either directly into machine code that the REX-8 CPU can understand or you can use the REX-8 assembler to convert pseudo machine code into machine code using the [Instruction string value](#instruction-set), comments can be written into pseudo machine code to be interpreted by the REX-8 assembler but not written into machine code, comments are made using `;` followed by a string of text.

#### Program Goal
- Load the value of 3 into the R1 register.
- Load the value of 4 into the R2 register.
- Add the value of R1 register to the R2 register, storing the result in the R1 register.
- Print the value of the R1 register.
- Halt the program.

**Program Code (pseudo machine code format)**
```assembly
LOAD R1, 0x03   ; Loads 3 into R1.
LOAD R2, 0x04   ; Loads 4 into R2.
ADD_REG R1, R2  ; Adds R1 and R2 storing the value in R1.
OUTPUT R1       ; Outputs the value of R1 to console.
HALT            ; Terminates the program.
```

**Program Code (machine code format)**
```python
0x01, 0x02, 0x03,
0x01, 0x03, 0x04,
0x04, 0x02, 0x03,
0x15, 0x02, 0x00,
0xFF
```

## 3. Creating a Program
### Step 1: Write the Program
Use the [instruction set table](#instruction-set) above to design the program, Either representend in machine code to be written directly to a `.bin` file or use the REX-8 assembler to write pseudo machine code to be written to a `.bin` file automatically.

### Step 2: Converting to a Binary
#### Manually
To manually write the program and save as a binary file (`program.bin`), you can use a python script to generate the `program.bin` from maching code which can directly used by the REX-8 CPU.
```python
program = [
  0x01, 0x02, 0x03,
  0x01, 0x03, 0x04,
  0x04, 0x02, 0x03,
  0x15, 0x02, 0x00,
  0xFF               
]

with open("program.bin", "wb") as f:
    f.write(bytearray(program))
```

#### Assembler
To use the REX-8 Assembler to create a `program.bin` you must create a `.asm` file and use the [Instruction set table](#instruction-set) to create a program which should look like:
```assembly
LOAD R1, 0x03   
LOAD R2, 0x04   
ADD_REG R1, R2  
OUTPUT R1       
HALT            
```

Then to convert this to machine code using the assembler you will have to run:

**Windows**
```pwsh
.\Assembler.exe --in=input.asm --out=program.bin
```

**Linux**
```bash
./Assembler --in=input.asm --out=program.bin
```

## 4. Running the Program
### Step 1: Compile the Emulator
Compile the CPU emulator using a C++ compiler via CMake generating the `CPU` and `Assembler` binaries:
```bash
git clone https://github.com/PrimeMonket/REX-8.git
cd REX-8
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

### Step 2: Run the Emulator
Run the REX-8 CPU, specifying the binary program file to execute:

```bash
./CPU --program==program.bin
```

The emulator will then:
  - Load the binary program into memory.
  - Execute the instructions sequentially.
  - Print the output to the console.

## 5. Debugging and Troubleshooting
  - Check Program Size: Ensure the program is ≤ 1024 bytes.
  - Instruction Errors: Verify that all instructions follow the correct format and arguments.
  - Memory Access: Ensure memory accesses are within valid bounds (0–1023).
  - Memory Inspection: The emulator prints non-zero memory contents after execution.
