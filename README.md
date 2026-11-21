# NOVA Virtual Machine

A **Vole machine simulator** written in C++ with a **Qt graphical interface**.
Write or load a machine-language program, store it in memory, then run it at once or
one instruction at a time while you watch the registers, the memory and the screen change.

> CS213 Object-Oriented Programming - Assignment 2 (Part 2)
> Faculty of Computers and Artificial Intelligence, Cairo University

![NOVA Virtual Machine](docs/screenshot.png)

## Want to use the project?

- **Installation and running:** read the file
  [How to install & run the program.txt](How%20to%20install%20%26%20run%20the%20program.txt)
  for the step-by-step instructions.
- **Try it with ready-made programs:** the file
  [Two programs.txt](Two%20programs.txt) contains **2 test programs**. Copy the content of a program
  and paste it into the **Machine code** box of the running application, then press **Store in memory**
  and **Run program** (or **Step**).

## Project document and video

| Resource | Link |
|----------|------|
| Project document | [Open the document on Google Drive](https://drive.google.com/file/d/1mmXVR0HQA6fmmI3T8J33dXm7JhBxcKCD/view?usp=sharing) |
| Explanation video | [Watch the video on YouTube](https://youtu.be/YZ5NlwV2QFE?si=f9nKcQergLrLtx_F) |

## Features

- Full Vole machine: 256 memory cells (8 bit), 16 registers (8 bit), program counter, instruction register and a screen at address `FF`
- **Run program** (everything at once) and **Step** (one instruction at a time), with a **Stop** button
- Programs can be typed in **Binary, Decimal or Hex**, or loaded from a file on your computer (**Load file...**)
- Live **CPU registers** table (binary and hex) and **memory matrix** with the program counter row highlighted
- **Output screen** that shows the program output as ASCII, plus status and error messages
- Protection against invalid programs (bad hex, bad start address, program too long) and against endless loops
- Standard C++ simulator core (no third-party libraries); Qt is used only for the interface

## Getting started

### Requirements

| Tool | Version |
|------|---------|
| Qt (Widgets) | 6.x (Qt 5 also works) |
| CMake | 3.16 or newer |
| C++ compiler | C++17 (MinGW or MSVC on Windows, g++ on Linux) |
| Qt Creator | recommended |

### Clone

```bash
git clone https://github.com/USERNAME/vole-simulator.git
cd vole-simulator
```

### Build and run with Qt Creator (recommended)

1. **File -> Open File or Project...** and choose `CMakeLists.txt`.
2. Select a Desktop kit and press **Configure Project**.
3. Build with **Ctrl+B**, run with **Ctrl+R**.

### Build from the command line

```bash
mkdir build && cd build
cmake ..
cmake --build .
./NovaVM          # NovaVM.exe on Windows
```

On Windows, if the program is started outside Qt Creator and a Qt DLL is missing, run
`windeployqt NovaVM.exe` once next to the executable.

## How to use it

1. Write the program in the **Machine code** box (one instruction per line) or press **Load file...**.
2. Choose the format of your code: **Binary**, **Decimal** or **Hex**.
3. Choose the **Start address** (hex, even, from `00` to `FE`).
4. Press **Store in memory**.
5. Press **Run program** (`F5`) or **Step** (`F10`).

The program stops at the halt instruction `C000`, at an invalid instruction, or after 100000
instructions (possible endless loop). Press **Store in memory** again to run it from the start.

### Program format

| Format  | One line looks like     |
|---------|-------------------------|
| Hex     | `2145`                  |
| Binary  | `0010 0001 0100 0101`   |
| Decimal | `8517`                  |

Empty lines are ignored and everything after a `;` is a comment. If a single line is invalid,
nothing is loaded and the output screen shows the line number and the reason.

Example (prints `Hi` on the output screen):

```
2148 ; R1 = 0x48 ('H')
31FF ; write R1 to the screen (address FF)
2269 ; R2 = 0x69 ('i')
32FF ; write R2 to the screen
C000 ; halt
```

## Instruction set

`R`, `S`, `T` are register numbers (2nd, 3rd and 4th hex digit) and `XY` are the last two hex digits.

| Instruction | Meaning |
|-------------|---------|
| `1RXY` | Load register R from memory cell XY |
| `2RXY` | Load register R with the value XY |
| `3RXY` | Store register R in memory cell XY (`XY = FF` writes to the screen) |
| `40RS` | Move register R to register S |
| `5RST` | Add S and T as two's complement, result in R |
| `6RST` | Add S and T as floating point (1 sign bit, 3-bit exponent excess-4, 4-bit mantissa), result in R |
| `7RST` | R = S OR T |
| `8RST` | R = S AND T |
| `9RST` | R = S XOR T |
| `AR0X` | Rotate register R to the right X times |
| `BRXY` | Jump to address XY if register R equals register 0 |
| `C000` | Halt |

## Project structure

```
.
├── CMakeLists.txt           project file (NovaVM.pro for qmake users)
├── main.cpp                 starts the application
│
├── MainWindow.h/.cpp/.ui    the window and all the button logic
├── CodeEditor.h/.cpp        machine code box with line numbers
├── CodeParser.h/.cpp        converts Binary / Decimal / Hex lines to hex
├── WaveformWidget.h/.cpp    small animated widget (currently hidden)
├── style.qss                colors and styling of the interface
├── resources.qrc            embeds style.qss in the program
│
├── Memory.h/.cpp            256 memory cells
├── RegisterFile.h/.cpp      16 registers
├── Instruction.h/.cpp       decodes a 16-bit instruction
├── Alu.h/.cpp               arithmetic and logic operations
├── CPU.h/.cpp               fetch - decode - execute cycle
├── Screen.h/.cpp            collects what is written to address FF
├── ProgramLoader.h/.cpp     reads and validates a program
├── Machine.h/.cpp           owns memory, screen and CPU
│
├── UI.h/.cpp                old console menu (not used by the GUI)
└── docs/screenshot.png
```

## Team

| Member | Area |
|--------|------|
| Ahmed Esmail | Memory, registers, screen headers, GUI |
| Abdulrahman Shawky | Program loader, user interface, GUI |
| Mahmoud Waleed | CPU, instruction decoding, execution cycle, repository |
| Ahmed Magdy | ALU, screen, machine, integration, testing (team leader) |

### Git workflow

- One branch per member (for example `member1-memory`), merged into `main` through the team leader.
- Never push code to `main` that does not compile.
- Commit regularly with descriptive messages, and merge small and often.

## Troubleshooting

| Problem | Fix |
|---------|-----|
| `ui_MainWindow.h: No such file or directory` | **Build -> Clear CMake Configuration**, then **Run CMake**, then **Rebuild Project** |
| `undefined reference to qMain` | `main.cpp` is missing or empty in the project |
| The window is plain light grey | `style.qss` and `resources.qrc` must be in the project folder; rebuild |
| A new file does not appear in Qt Creator | Add it to `CMakeLists.txt`, save, then **Build -> Run CMake** |
