#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_TOKENS 10000
#define MAX_SYMBOLS 100
#define CODE_SIZE 100

char* scanner(char* filename, int tokensFlag, FILE* output, int* errorFlag);
void parser(char* toks, FILE* output, int assemblyFlag, int* errorFlag);
void vm();

enum ISA {lit=1, rtn, lod, sto, cal, inc, jmp, jpc, sio1, sio2, sio3,
neg, add, sub, mul, dvd, odd, mod, eql, neq, lss, leq, gtr, geq};
