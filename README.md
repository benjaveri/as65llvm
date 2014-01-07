a s 6 5 l l v m
===============

A 6502 assembler front-end with an LLVM back-end.

This is a purely academic project.

I needed some way to become familiar with the LLVM compiler tools, and decided 
 to create a simple 6502 assembler with it.

Assembled executables can run on any architecture LLVM has back-ends for. This
 means you can run compile code for your x86 machine. We do not emulate a 6502
 processor, so there are some major differences to what you'd expect from the
 numerous emulators out there.

We do however have to define a 6502 virtual machine that makes sense in this
 context. Similarities exist mostly with the register and data model (views)
 but it does not translate well to actual code.

Notes:
 - The VM implements the Harvard Architecture. Code bytes are totally 
   independent and unreachable. No self-modifying code.
 - Data space is 64k, code space is essentially unlimited.
 - No absolute jumps
 - Code & data stacks are separate. Data stack uses SP, code stack is 
   inaccessible (e.g. no absolute jumps by pushing address & RTS)
 - Branches can be as far as needed (there are no code bytes, so no restrictions
   on encoding relative displacements)
 - The VM implements custom escapes to access I/O

Design details can be found in other ALL-CAPS files in this folder:
 DESIGN  - describes the assembler's architecture & design
 PLAN    - a rough list of what to do and what has been done
 VM      - describes the 6502 virtual machine, including the I/O model and
           the various limitations and caveats compared to real processors
 BNF     - shows the assembler's grammar
 LICENSE - the open source license for this project
 NOTICES - notices regarding third party code
 SETUP   - how to set up the build environment


S E T U P
=========

MAC OSX Mavericks

- get lastest xcode
- using macports (or otherwise), obtain llvm-3.4 and boost (1.55)
  macports puts llvm include files here: /opt/local/libexec/llvm-3.4/include/llvm
  macports puts boost files here:  /opt/local/include/boost

Ubuntu Linux 3.10

- use apt-get to get llvm-3.4 and boost (1.55)

