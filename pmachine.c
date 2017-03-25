// COP 3402 - Systems Software
// 3-24-17 | Austin Peace & Andrew Emery
// Programming Assignment 1 -- P-Machine

#include "Compiler.h"

// STRUCTS, ENUMERATIONS, AND NATIVE PROTOTYPES.

typedef struct {

    int op;
    int r;
    int l;
    int m;

} instruction;

typedef struct {

    instruction IR;
    int SP;
    int BP;
    int PC;
    int* registerFile;

} machine;

int virtualMachine(const char* filename);
void buildMachine(machine* CPU);
int readFile(const char* filename, instruction storage[CODE_SIZE]);
void interpret(instruction storage[CODE_SIZE], int length);
void fetch(machine* CPU, instruction* storage);
void execute(machine* CPU, int* stack, int* haltflag, int line, int instructions);
int base(int l, int base, int* stack);

// GLOBALS.
const char* terms[25] = {"nul", "lit", "rtn", "lod", "sto", "cal", "inc", "jmp", "jpc", "sio", "sio", "sio",
"neg", "add", "sub", "mul", "div", "odd", "mod", "eql", "neq", "lss", "leq", "gtr", "geq"};

// MAIN PROGRAM.
int virtualMachine(const char* filename) {

    // Main variables.
    instruction storage[CODE_SIZE];
    machine CPU;
    int stack[MAX_STACK_HEIGHT] = {0};
    int endflag = false;
    int num_instructions;

    // Initialize new instance of CPU.
    buildMachine(&CPU);

    /* Debug: Print CPU Machine.
    printf("MACHINE:\nSP: %d\nBP: %d\nPC: %d\nIR: %d %d %d %d\n", CPU.SP, CPU.BP, CPU.PC, CPU.IR.op, CPU.IR.r, CPU.IR.l, CPU.IR.m);
    int n;
    for (n = 0; n < REGFILE_SIZE; n++) {
        printf("%d ", CPU.registerFile[n]);
    } */

    // Read input file.
    num_instructions = readFile(filename, storage);
    if (!num_instructions) return 0;

    // Output interpretation of code.
    interpret(storage, num_instructions);

    // Begin program execution. Track "halt" state after every cycle.
    do {
        fetch(&CPU, storage);
        CPU.PC++;
        execute(&CPU, stack, &endflag, CPU.PC-1, num_instructions);

    } while (!endflag);

    return 1;
}

// FUNCTION DEFINITIONS.
void buildMachine(machine* CPU) {

    // Allocate and initialize the CPU.
    CPU->SP = 0;
    CPU->BP = 1;
    CPU->PC = 0;
    CPU->IR.op = 0;
    CPU->IR.r = 0;
    CPU->IR.l = 0;
    CPU->IR.m = 0;

    CPU->registerFile = calloc(REGFILE_SIZE, sizeof(int));

    return;
}

int readFile(const char* filename, instruction storage[CODE_SIZE]) {
    // Local variables.
    FILE *fp = NULL;
    int line = 0;

    // Read file (if it is found).
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("\nUnable to locate file. Check local directory.");
        return 0;
    }

    while (!feof(fp)) {
        fscanf(fp, " %d %d %d %d ", &storage[line].op, &storage[line].r, &storage[line].l, &storage[line].m);
        line++;
    }

    return line;
}

void interpret(instruction storage[CODE_SIZE], int length) {
    // Local variables.
    int n;

    // Print header.
    printf("\n\n%4s %8s %8s %8s %8s\n", "LINE", "OP", "R", "L", "M");

    // Print until all code in storage has been read.
    for (n = 0; n < length; n++) {
        printf("%4d %8s %8d %8d %8d\n", n, terms[storage[n].op], storage[n].r, storage[n].l, storage[n].m);
    }

    printf("\n\nInitial Values %24s %6s %6s", "pc", "bp", "sp");

    return;
}

void fetch(machine* CPU, instruction* storage) {

    // Retrieve next instruction, according to program counter.
    CPU->IR.op = storage[CPU->PC].op;
    CPU->IR.r = storage[CPU->PC].r;
    CPU->IR.l = storage[CPU->PC].l;
    CPU->IR.m = storage[CPU->PC].m;

    return;
}

void execute(machine* CPU, int* stack, int* haltflag, int line, int instructions) {
    // Local variables.

    // Execute designated instruction in CPU.
    switch(CPU->IR.op) {

        // LIT.
        case 1:
            CPU->registerFile[CPU->IR.r] = CPU->IR.m;
            break;

        // RTN.
        case 2:
            CPU->SP = CPU->BP - 1;
            break;

        // LOD.
        case 3:
            CPU->registerFile[CPU->IR.r] = stack[base(CPU->IR.l, CPU->BP, stack) + CPU->IR.m];
            break;

        // STO.
        case 4:
            stack[base(CPU->IR.l, CPU->BP, stack) + CPU->IR.m] = CPU->registerFile[CPU->IR.l];
            break;

        // CAL.
        case 5:
            stack[CPU->SP+1] = 0;
            stack[CPU->SP+2] = base(CPU->IR.l, CPU->BP, stack);
            stack[CPU->SP+3] = CPU->BP;
            stack[CPU->SP+4] = CPU->PC;
            CPU->BP = CPU->SP+1;
            CPU->PC = CPU->IR.m;
            break;

        // INC.
        case 6:
            CPU->SP += CPU->IR.m;
            break;

        // JMP.
        case 7:
            CPU->PC = CPU->IR.m;
            break;

        // JPC.
        case 8:
            if (CPU->registerFile[CPU->IR.r] == 0) CPU->PC = CPU->IR.m;
            break;

        // SIO1.
        case 9:
            printf("\nREGISTER %d REPORTS: %d", CPU->IR.r, CPU->registerFile[CPU->IR.r]);
            break;

        // SIO2.
        case 10:
            printf("\nEnter #: ");
            scanf("%d", &CPU->registerFile[CPU->IR.r]);
            break;

        // SIO3.
        case 11:
            CPU->BP = 0;
            break;

        // NEG.
        case 12:
            CPU->registerFile[CPU->IR.r] = -CPU->registerFile[CPU->IR.l];
            break;

        // ADD.
        case 13:
            CPU->registerFile[CPU->IR.r] = CPU->registerFile[CPU->IR.l] + CPU->registerFile[CPU->IR.m];
            break;

        // SUB.
        case 14:
            CPU->registerFile[CPU->IR.r] = CPU->registerFile[CPU->IR.l] - CPU->registerFile[CPU->IR.m];
            break;

        // MUL.
        case 15:
            CPU->registerFile[CPU->IR.r] = CPU->registerFile[CPU->IR.l] * CPU->registerFile[CPU->IR.m];
            break;

        // DIV.
        case 16:
            CPU->registerFile[CPU->IR.r] = CPU->registerFile[CPU->IR.l] / CPU->registerFile[CPU->IR.m];
            break;

        // ODD.
        case 17:
            CPU->registerFile[CPU->IR.r] = CPU->registerFile[CPU->IR.r] % 2;
            break;

        // MOD.
        case 18:
            CPU->registerFile[CPU->IR.r] = CPU->registerFile[CPU->IR.l] % CPU->registerFile[CPU->IR.m];
            break;

        // EQL.
        case 19:
            CPU->registerFile[CPU->IR.r] = (CPU->registerFile[CPU->IR.l] == CPU->registerFile[CPU->IR.m]);
            break;

        // NEQ.
        case 20:
            CPU->registerFile[CPU->IR.r] = (CPU->registerFile[CPU->IR.l] != CPU->registerFile[CPU->IR.m]);
            break;

        // LSS.
        case 21:
            CPU->registerFile[CPU->IR.r] = (CPU->registerFile[CPU->IR.l] < CPU->registerFile[CPU->IR.m]);
            break;

        // LEQ.
        case 22:
            CPU->registerFile[CPU->IR.r] = (CPU->registerFile[CPU->IR.l] <= CPU->registerFile[CPU->IR.m]);
            break;

        // GTR.
        case 23:
            CPU->registerFile[CPU->IR.r] = (CPU->registerFile[CPU->IR.l] > CPU->registerFile[CPU->IR.m]);
            break;

        // GEQ.
        case 24:
            CPU->registerFile[CPU->IR.r] = (CPU->registerFile[CPU->IR.l] >= CPU->registerFile[CPU->IR.m]);
            break;

        // ERROR.
        default:
        break;

    }
    // Check state of the VM (finished after this instruction?).
    if (CPU->PC >= instructions || CPU->IR.op == sio3) {
        CPU->PC = 0;
        CPU->SP = 0;
        CPU->BP = 0;
        *haltflag = true;
    }

    // Print current state of the machine.
    printf("\n%3d %6s %6d %6d %6d %6d %6d %6d", line, terms[CPU->IR.op], CPU->IR.r, CPU->IR.l, CPU->IR.m, CPU->PC, CPU->BP, CPU->SP);

    // Print stack for current instruction.
    int i;
    for (i = 1; i <= CPU->SP; i++) {
        printf(" %6d", stack[i]);
    }
    /* DEBUG: Print register file.
    printf("\n");
    for (i = 0; i< REGFILE_SIZE; i++) {
        printf("%d ", CPU->registerFile[i]);
    }*/

    return;
}

// BORROWED FROM RUBRIC:
int base(int l, int base, int* stack) // l stand for L in the instruction format
{
  int b1; //find base L levels down
  b1 = base;
  while (l > 0)
  {
    b1 = stack[b1 + 1];
    l--;
  }

  return b1;
}

