# Two-pass-8085-Assembler
## Team Members
## Team Members

  ➣ [Koushan De](https://github.com/KoushanDe) (Roll-001910501008)
  
  ➣ [Saptarshi Rakshit](https://github.com/saphere9) (Roll-001910501009)

  ➣ [Jaydip Dey](https://github.com/jaydip1235) (Roll-001910501014)

  ➣ [Shakeeb Ilmi Noor](https://github.com/ShakeebIlmiNoor) (Roll-001910501028)

  ➣[ Soumita Paul](https://github.com/myiotuhris) (Roll-001910501029)

## 8085 ARCHITECTURE
    8085 is an 8-bit microprocessor, designed by Intel in1977. 
    It has the following configuration:

    ◉	8 bit data bus
    ◉	16 bit address bus, which can address upto 64KB
    ◉	A 16 bit program counter
    ◉	A 16 bit stack pointer
    ◉	Six 8 bit registers arranged in pairs: BC, DE, HL
    ◉	An 8 bit accumulator to perform Load/Store, I/O and ALU operations.
    ◉	An 8 bit register having 5 flags S, Z, AC, P,CY

[![8085ar.jpg](https://i.postimg.cc/L4rD50wL/8085ar.jpg)](https://postimg.cc/grH8B4Y2)
## TWO-PASS ASSEMBLER
    An assembler is a translator, that translates an assembler 
    program into a conventional machine language program. 
    Basically, the assembler goes through the program one line 
    at a time, and generates machine code for that instruction.
    Now, why do we need a 2-pass assembler?
    Consider the following code snippet
    JMP  LATER
          ...
          ...
    LATER:

    Here assembler cannot recognise the label ‘LATER’ in the 
    first pass and assign its address accordingly. This is known
    as Forward Reference problem. To address this issue, we need
    a 2-pass assembler.
[![tp.jpg](https://i.postimg.cc/nrvDCT9G/tp.jpg)](https://postimg.cc/BPn6zTgj)
## DESIGN DETAILS
    The project directory contains 4 sub-directories as follows:
      ⊛ src- Contains all the source files
      ⊛ inc-Contains all the header files including the function 
         definitions.
      ⊛ data-Contains all the data files like input file, optab, 
         symtab.
      ⊛ output-Contains the output file generated after 2 passes.
      
    The source files are as follows:
      ⊛ optable.c- Contains hashmap implementation of OPTAB.
      ⊛ symtable.c- Contains hashmap implementation of SYMTAB.
      ⊛ utils.c-  Contains utility functions used by pass 1 and 
        pass2 of assembler.
      ⊛ pass1.c- Pass1 implementation, it takes input from 
        data/input.txt, refers to data/optable.txt to update 
        the locctr and updates the SYMTAB in data/symtable.txt.
      ⊛ pass2.c- Pass2 implementation, it takes input from 
        data/input.txt, refers to data/optable.txt and 
        data/symtable.txt to produce the output file 
        output/output.txt. The output file is not produced in
        case of errors.
      ⊛ 2-pass-assembler.c- Runs the pass1 and pass2 assembler
        sequentially.

## RUNNING THE ASSEMBLER
    For compilation:
      gcc optable.c symtable.c to_hex.c utils.c pass1.c pass2.c
      2-pass-assembler.c -o assembler

    To run the assembler on Windows, go to the Assembler/src and
    type assembler.exe on command prompt.

## Output

### Console output
[![blur.png](https://i.postimg.cc/nV7hm00q/blur.png)](https://postimg.cc/3W8h5CMw)
### Object code
[![obcode.jpg](https://i.postimg.cc/W3tKLWfz/obcode.jpg)](https://postimg.cc/LYKvjTSF)





