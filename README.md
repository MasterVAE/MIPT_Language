# Ultimate Programming Language
Fully functional C-like programming language based on C

## Prerequisites

- C++ compiler
- make
- Graphviz

## Build

```bash
make all
```
or
```bash
make descent_b
make compilator_b
```

## Usage 

```bash
make run
```
or
```bash
make descent
make compilator
./asm.out
./spu.out
```
or
```bash
./build/descent.out <code filename> <tree filename>
./build/compilator.out <tree filename> <out filename>
./asm.out
./spu.out
```

## Notes

This project consist of **reader** and **compilator**. Reader use **recursive descent** to read user's program and process it into tree structure.

Example of tree:

![generated ](examples/images/tree1.png)

Then tree transfers to **compilator** which process tree into Assembler code for **SPU** (Soft Processor Unit) from previous project.