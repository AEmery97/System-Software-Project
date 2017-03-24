// COP 3402 - Systems Software
// 3-24-17 | Austin Peace & Andrew Emery
// Programming Assignment -- Compiler.h Header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TOKEN_BUFFER 100000
#define MAX_SYMBOLS 1000
#define MAX_SOURCE_FILE 5000
#define MAX_WORD_SIZE 12
#define MAX_NUM_SIZE 6
#define RESERVED_WORDS 14
#define RESERVED_SYMBOLS 13
#define ASCII_A 65
#define ASCII_Z 90
#define ASCII_LA 97
#define ASCII_LZ 122
#define ASCII_0 48
#define ASCII_9 57
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3
#define REGFILE_SIZE 16

enum bool {false, true};

enum ISA
{
    lit=1, rtn, lod, sto, cal, inc, jmp, jpc, sio1, sio2, sio3,
    neg, add, sub, mul, dvd, odd, mod, eql, neq, lss, leq, gtr, geq
};
typedef enum
{
	nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,  slashsym = 7,
	oddsym = 8,  eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14,
	lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18, periodsym = 19,
	becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25,
	dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
	readsym = 32, elsesym = 33

} token_type;

typedef struct
{
	int kind;	//const = 1, var = 2, proc = 3
	char name[11];	//name up to 10 chars
	int val;	//numbers(ASCII value)
	int level;	//L level
	int addr;	//M address
} symbol;

typedef struct
{
	int op;
	int r;
	int l;
	int m;
} code_struct, instruction;

typedef struct {

    instruction IR;
    int SP;
    int BP;
    int PC;
    int* registerFile;

} machine;

// SCANNER PROTOTYPES.

char* inReader();
void printBuffer(char* buffer, int length);
void clean(char* buffer, int buffersize);
void report(char* stream);
int invalid(char value);
int isLetter(char value);
int isDigit(char value);
void tokenWord(int word_id, char* buffer);
void tokenNum(char* buffer, int length);
void tokenSym(int sym_id);

// PARSER/CG PROTOTYPES.

void emit(int op, int r, int l, int m);
int program();
int block();
int getToken();
int statement();
int condition();
int expression();
int term();
int factor();
int identExists(char* ident);
int rOp();
int addSymbolToTable(char* name, int type, int value, int l, int m);
char* convert2Tokens(char* fileName);

// P-MACHINE PROTOTYPES.

int virtualMachine();
void buildMachine(machine* CPU);
int readFile(instruction storage[MAX_CODE_LENGTH]);
void interpret();
void fetch(machine* CPU, instruction* storage);
void execute(machine* CPU, int* stack, int* haltflag, int line, int instructions);
int base(int l, int base, int* stack);


