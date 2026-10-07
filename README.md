# Memory Allocator in C

A custom memory allocator written in C to understand how dynamic memory allocation works internally.

## What it does

This project implements a basic memory management system with its own heap and block metadata.

Currently implemented:

* `malloc`
* `free`
* `calloc`
* `realloc`
* Block splitting
* Free block coalescing
* Custom heap management
* Windows `VirtualAlloc` backend
* Tests
* Benchmarks

## How it works

The allocator requests a large memory region from the operating system and manages that region itself.

Each memory block contains metadata:

```text
┌──────────────┬─────────────────────┐
│ Block header │     User memory     │
└──────────────┴─────────────────────┘
```

The block header stores information such as:

* Block size
* Whether the block is free
* Pointer to the next block

When memory is requested, the allocator searches for a suitable free block. Large blocks can be split into smaller blocks.

When memory is released, adjacent free blocks can be merged together.

## Project structure

```text
memory-allocator/
├── headers/
│   └── allocator.h
├── source/
│   └── allocator.c
├── tests/
│   └── my_malloc.c
│   └── my_calloc.c
│   └── my_realloc.c
│   └── my_free.c
├── benchmark/
│   └── allocator_benchmark.c
└── README.md
```

## Build

Compile the allocator together with the test:

```bash
gcc -Iheaders tests/{TEST FILE} source/allocator.c -o test
```

Run:

```bash
./test
```

## Benchmarks

The project includes benchmarks comparing the custom allocator with the system allocator.

The benchmarks test operations such as:

* `malloc` + `free`
* `realloc`
* Different allocation patterns

Example benchmark results will be added here as the allocator develops.

## Why I built this

I built this project to understand how memory allocation works at a lower level and to practice:

* Pointers
* Memory layout
* Heap management
* Data structures
* OS memory APIs
* C systems programming

## Future improvements

* [ ] Memory alignment
* [ ] Stress testing
* [ ] More extensive tests
* [ ] Better fragmentation handling
* [ ] Thread safety
