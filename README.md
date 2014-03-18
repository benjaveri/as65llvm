as65llvm
========

A 6502 assembler front-end with an LLVM back-end.

This is a purely academic project.

I needed some way to become familiar with the LLVM compiler tools, and decided 
 to create a simple 6502 assembler with it.

Assembled executables can run on any architecture LLVM has back-ends for. This
 means you can run compile code for your x86 machine. We do not emulate a 6502
 processor, so there are some major differences to what you'd expect from real
 hardware or any of the numerous emulators out there.

We define a 6502 virtual machine that models data and registers almost exactly.
 For code, the assembler understands the standard 6502 mnemonics, but they are
 translated into the target processor's machine code, instead of 6502 code.
 The target processor's code is managed independently from the 64k addressable
 data space the 6502 has, so it is unreadable from the VM. Similarly, absolute
 addressing does not make much sense in this model.

Notes:
 - The VM implements the Harvard Architecture. Code bytes are totally 
   independent and unreachable. 
 - Data space is 64k, code space is practically unlimited.
 - No absolute jumps.
 - No self-modifying code.
 - Code & data stacks are separate. Data stack uses SP, code stack is 
   managed separately and inaccessible (e.g. no absolute jumps by pushing 
   address & RTS)
 - Branches can be as far as needed (there are no 6502 code bytes, so no 
   restrictions on encoding relative displacements)
 - The VM implements custom escapes to access I/O

Design details can be found in other ALL-CAPS files in this folder:
 DESIGN  - describes the assembler's architecture & design
 PLAN    - a rough list of what to do and what has been done
 VM      - describes the 6502 virtual machine, including the I/O model and
           the various limitations and caveats compared to real processors
 BNF     - shows the assembler's grammar
 LICENSE - the open source license for this project
 NOTICES - notices regarding third party code
