# PKU Compiler Course

- Course Documentation: https://pku-minic.github.io/online-doc/#/
- Environment Repository (Upstream): https://github.com/pku-minic/compiler-dev

## Project Structure

```
src/
├── frontend/             # Frontend: lexical analysis, syntax analysis, AST
│   ├── ast.c/h          # Abstract syntax tree definition and operations
│   ├── sysy.l           # Flex lexical analyzer
│   └── sysy.y           # Bison syntax analyzer
├── midend/               # Middle-end: intermediate code generation
│   ├── codegen.c/h      # Koopa IR code generator
│   └── koopa_ir.c/h     # Koopa IR processing utilities
├── backend/              # Backend: target code generation
│   └── riscv_gen.c/h    # RISC-V assembly code generator
└── main.c                # Main program entry point
```

## Quick start

```bash
# pull image
docker pull maxxing/compiler-dev
```

```bash
# mount
docker run -it --rm -v "/home/uednd/Code/PKU-Compiler-Course:/workspace" maxxing/compiler-dev bash
cd /workspace
```

```bash
# config and build
cmake -DCMAKE_BUILD_TYPE=Debug -B build && cmake --build build
```

## Usage

### Generate Koopa IR
```bash
./build/compiler -koopa test/hello.c -o hello.koopa
cat hello.koopa
```

### Generate RISC-V Assembly
```bash
./build/compiler -riscv test/hello.c -o hello.s
cat hello.s
```

### Show AST Structure (Debug)
```bash
./build/compiler -ast test/hello.c -o hello.ast
```

## Example

Given input file `test/hello.c`:
```c
int main() {
  return 0;
}
```

**Koopa IR Output** (`-koopa` mode):
```
fun @main(): i32 {
%entry:
  ret 0
}
```

**RISC-V Assembly Output** (`-riscv` mode):
```assembly
  .text
  .globl main
main:
  li a0, 0
  ret
```

> **Note:**  
> If you are using Linux and the repository is on a mounted external hard drive, Docker may not have the necessary permissions when running the mount command above, and however, it won't tell you about it. This shit wasted 2hs of my life.