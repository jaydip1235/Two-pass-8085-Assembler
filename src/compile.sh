gcc -g to_hex.c pass1.c pass2.c hash_map_1.c hash_map_2.c utils.c 2-pass-assembler.c -o assembler -lm
gcc -g run_program.c simulator.c -o simulator -lm