# RISC-Instruction-Mapper

A utility for generating an instruction encoding system for a RISC-like architecture based on JSON requirements

## Requirements
* C++ compiler with **C++17** support or higher (GCC 7+, Clang 5+, MSVC 2017+).
* **nlohmann/json** library (header-only). Ensure `json.hpp` is available in your compiler's include paths.

## Build instructions

From the root folder:
``` bash
  mkdir build
  cd build
  cmake ..
  cmake --build .
```

### Usage
The program takes two command-line arguments: the path to the input JSON file containing the architecture requirements, and the path where the output JSON will be saved. The example for the first one is located in `data/` folder:
```bash
  ./RISC_Instruction_Mapper ../data/input.json ../data/output.json
```

## Example Input File (`data/input.json`)
<img width="787" height="530" alt="image" src="https://github.com/user-attachments/assets/fe52618e-5a4f-40b5-a267-26b92a775e8d" />
