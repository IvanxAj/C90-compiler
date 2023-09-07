C90 to RISC-V Compiler
==============================

This repository contains a pre-processed C90 to RISC-V assembly compiler.

Program build and execution
---------------------------

The compiler is built using the command:

     make bin/c_compiler

The compilation function is invoked using the flag `-S`, with the source file and output file specified on the command line:

    bin/c_compiler -S [source-file.c] -o [dest-file.s]

The test script can be run by

    ./test.sh

Specification
==========

All input source code is valid and there is no expectation for the compiler to *optimise* its input. [Spec](https://github.com/LangProc/langproc-2022-cw/blob/main/c_compiler.md)

Acknowledgements
================

* The coursework was originally designed by [David Thomas](https://www.southampton.ac.uk/people/5z9bmb/professor-david-thomas), who lectured this module until 2017-18. It is nowadays maintained by [John Wickerson](https://johnwickerson.github.io/), to whom any feedback should be sent.
* Thanks to [Yann Herklotz](https://yannherklotz.com/) for making various improvements to the compiler-testing scripts.
* Thanks to [Archie Crichton](https://www.doc.ic.ac.uk/~ac11018/) for providing a basic "getting started" compiler.
* Extra-special thanks to [James Nock](https://www.linkedin.com/in/jpnock) for overhauling the scripts for configuring the development environment, for writing detailed instructions for setting this up on various operating systems, and for creating GitHub actions capable of automatically testing compilers.
