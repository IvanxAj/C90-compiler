C90 to RISC-V Compiler
==============================

This repository contains the source code for a pre-processed ANSI C to RISC-V assembly compiler. This project currently scores 84% of all seen test cases.

## Program build and execution

The compiler is built using the command:

     make bin/c_compiler

The compilation function is invoked using the flag `-S`, with the source file and output file specified on the command line:

    bin/c_compiler -S [source-file.c] -o [dest-file.s]

The test script can be run by

    ./test.sh

    ./test_single.sh single.c

## docs

In this directory, a concise overview of the final implementation and an explanation of various components can be found in [summary](docs/summary.md).

For those interested in the development journey, a [journal](docs/journal.md) is also provided, detailing changes over time.

## Specification

All input source code is valid and there is no expectation for the compiler to *optimise* its input. [Specification](https://github.com/LangProc/langproc-2022-cw/blob/main/c_compiler.md)

### Some supported features

Basic
- [x] single function with no arguments
- [x] variables of int type
- [x] local variables
- [x] arithmetic and logical expressions
- [x] if-else statements
- [x] while loops

Intermediate
- [x] multiple functions that call each other
- [x] functions that take up to eight parameters
- [x] for loops
- [x] arrays declared locally (i.e. inside a function)
- [x] reading and writing elements of an array
- [x] recursive function calls
- [x] switch statements
- [x] the break and continue keywords
- [ ] the enum keyword
- [ ] arrays declared globally

Advanced
- [x] variables of double, float, unsigned, and pointer types
- [x] mutually recursive function calls
- [x] locally scoped variable declarations (e.g. a variable that is declared inside the body of a while loop, such as while(...) { int x = ...;}.
- [x] the sizeof(...) function (which takes either a type or a variable)
- [x] taking the address of a variable using the & operator
- [x] dereferencing a pointer-variable using the * operator
- [x] pointer arithmetic
- [ ] typedef keyword
- [ ] declaration and use of structs
- [ ] variables of char and struct types
- [ ] strings (as NULL-terminated character arrays)

### Acknowledgements

* The coursework was originally designed by [David Thomas](https://www.southampton.ac.uk/people/5z9bmb/professor-david-thomas), who lectured this module until 2017-18. It is now maintained by [John Wickerson](https://johnwickerson.github.io/), to whom any feedback should be sent.
* Thanks to [James Nock](https://www.linkedin.com/in/jpnock) for overhauling the scripts for configuring the development environment, for writing detailed instructions for setting this up on various operating systems, and for creating GitHub actions capable of automatically testing compilers.
