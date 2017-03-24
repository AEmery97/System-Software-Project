// COP 3402 - Systems Software
// 3-24-17 | Austin Peace & Andrew Emery
// Programming Assignment 2 - Parser/CodeGenerator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TOKEN_BUFFER 100000
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

// PROTOYPES, STRUCTURES AND PARAMETERS.

/* NOTE: THIS CODE BORROWED FROM ASSIGNMENT SPECIFICATIONS */
typedef struct {
	int kind; 		// const = 1, var = 2, proc = 3
	char name[MAX_WORD_SIZE];	// name up to 11 chars
	int val; 		// number (ASCII value)
	int level; 		// L level
	int addr; 		// M address
    } symbol;

/* NOTE: THIS CODE BORROWED FROM ASSIGNMENT SPECIFICATIONS */
enum bool {false, true};

typedef enum {
nulsym = 1, identsym, numbersym, plussym, minussym,
multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym } token_type;

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


// GLOBALS (I.E. PROGRAMMER LAZINESS).
const char* errors[] = {"Variable does not start with letter.", "Number too long.", "Name too long.", "Invalid symbols detected."};
const char* words[RESERVED_WORDS] = {"const", "var", "procedure", "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write", "odd"};
const char symbols[RESERVED_SYMBOLS] = {'+','-','*','/','(',')','=',',','.','<','>',';', ':'};
char* tokenstream; // Storage for token output.
int token_index = 0;
int error_code[4] = {0};
FILE *fp = NULL; // File pointer for input.
char *filename; // File name.

// MAIN EXECUTION.
int main() {
    // Local variables.
    int n;
    char* sourceBuffer;
    int sourceLength = 0;
    tokenstream = malloc(TOKEN_BUFFER*sizeof(char));

    // Read input file into code store. If it fails, notify the user.
    filename = malloc(sizeof(char)*FILENAME_MAX);
    printf("\nEnter file name: ");
    scanf("%s", filename);
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("\nERROR: Unable to locate file specified! Check local directory.");
        return 0;
    }

    // Read the input source file, and populate the symbol table with the program's contents.
    sourceBuffer = inReader();
    sourceLength = strlen(sourceBuffer);

    // Print the contents of the entire input file.
    printBuffer(sourceBuffer, sourceLength);

    // Remove invalid characters/tidy up buffered input.
    clean(sourceBuffer, sourceLength);

    // Check error conditions.
    for (n = 0; n < 4; n++) {
        if (error_code[n]) {
            printf("\nERROR: %s", errors[n]);
            printf("\n%s", tokenstream);
            free(sourceBuffer);
            free(tokenstream);
            tokenstream = NULL;
            return -1;
        }
    }
    // If no errors have been discovered, read finalized input/tokens.
    report(tokenstream);

    fclose(fp);

    return 0;
}

// FUNCTIONS.
char* inReader() {
    // Local variables.
    int n = 0;
    char* stream = malloc(MAX_SOURCE_FILE*sizeof(char));
    char temp;

    // Read the entire file as a stream of input.
    while ((temp = fgetc(fp)) != EOF) {
        stream[n] = temp;
        n++;
    }
    stream[n] = '\0';

    return stream;
}

void printBuffer(char* buffer, int length) {
    // Local variables.
    int n;

    // Print header.
    printf("\nSource Program:\n");

    // Print the entire contents of the designated file.
    for (n = 0; n < length; n++) {
        printf("%c", buffer[n]);
    }

    return;
}

void clean(char* buffer, int buffersize) {
    // Local variables.
    int n, tracer = 0;
    char* tempbuf;

    // Step 1: Clean out all unnecessary components of source file.
    for (n = 0; n < buffersize; n++) {
        // Invisible characters...
        if (invalid(buffer[n])) buffer[n] = ' ';

        // Comments...
        if (buffer[n] == '/' && buffer[tracer+1] == '*') {

            // Mimic conversion of all following text to empty space.
            while (n+1 < buffersize && !(buffer[n] == '*' && buffer[n+1] == '/')) {
                buffer[n] = ' ';
                n++;
            }
            if (buffer[n] == '*' && buffer[n+1] == '/') {
                buffer[n] = ' ';
                buffer[n+1] = ' ';
            }
            else {
                buffer[n] == '\0';
            }
        }
    }

    // Step 2: Tokenize clean input.
    while (tracer < buffersize) {
        if (buffer[tracer] != ' ') {

            // Case 1: If the first character in the word is a letter.
            if (isLetter(buffer[tracer])) {
                // Allocate temporary buffer.
                tempbuf = malloc(MAX_WORD_SIZE*sizeof(char));

                // Generate the full word.
                n = 0;
                while (n < MAX_WORD_SIZE && (isLetter(buffer[tracer]) || isDigit(buffer[tracer]))) {

                    tempbuf[n] = buffer[tracer];
                    tracer++;
                    n++;
                }
                if (n == MAX_WORD_SIZE) {
                    error_code[2] = true;
                    return;
                }
                else tempbuf[n] = '\0';

                // Compare assembled word to reserved. If a match is found, diverge.
                for (n = 0; n < RESERVED_WORDS; n++) {
                    if (strcmp(tempbuf, words[n]) == 0) {
                        tokenWord(n, tempbuf);
                        break;
                    }
                }

                // If no match is found, it must be treated as an identifier.
                if (n == RESERVED_WORDS) tokenWord(-1, tempbuf);
            }

            // Case 2: If the first character in the word is a digit.
            else if (isDigit(buffer[tracer])) {
                // Allocate temporary buffer.
                tempbuf = malloc(MAX_NUM_SIZE*sizeof(char));

                // Generate the number.
                n = 0;
                while (n < MAX_NUM_SIZE && isDigit(buffer[tracer])) {

                    tempbuf[n] = buffer[tracer];
                    tracer++;
                    n++;
                }
                if (n == MAX_NUM_SIZE) {
                    error_code[1] = true;
                    return;
                }
                else tempbuf[n] = '\0';

                // Convert number string to integer (tokenize).
                tokenNum(tempbuf, n);
            }

            // Case 3: If the first character in the word is a reserved symbol.
            else if (ispunct(buffer[tracer])) {

                int errorflag = true;
                // Check the symbol against reserved. Handle special cases.
                for (n = 0; n < RESERVED_SYMBOLS; n++) {
                    if (buffer[tracer] == symbols[n]) {
                        // We know we have some manner of legitimate symbol.
                        errorflag = false;

                        // Check to see if the symbol is part of a multi-symbol operator.
                        if (tracer+1 < buffersize) {
                            switch (buffer[tracer]) {

                            // 'Becomes' case.
                            case ':':
                                // If followed by '=', tokenize the symbol. Otherwise, invalid.
                                if (buffer[tracer+1] == '=') tokenSym(becomessym);
                                else {
                                    error_code[3] = true;
                                    return;
                                }
                                break;

                            // 'GEQ' case..
                            case '>':
                                // If followed by '=' or ' ', tokenize the symbol. Otherwise, invalid.
                                if (buffer[tracer+1] == '=') tokenSym(geqsym);
                                else if (buffer[tracer+1] == ' ') tokenSym(gtrsym);
                                else {
                                    error_code[3] = true;
                                    return;
                                }
                                break;

                            // 'LEQ' case..
                            case '<':
                                // If followed by '=' or ' ', tokenize the symbol. Otherwise, invalid.
                                if (buffer[tracer+1] == '=') tokenSym(leqsym);
                                else if (buffer[tracer+1] == ' ') tokenSym(lessym);
                                else {
                                    error_code[3] = true;
                                    return;
                                }
                                break;

                            // All other cases deal with single symbols.
                            case '+':
                                tokenSym(plussym);
                                break;

                            case '-':
                                tokenSym(minussym);
                                break;

                            case '*':
                                tokenSym(multsym);
                                break;

                            case '/':
                                tokenSym(slashsym);
                                break;

                            case '(':
                                tokenSym(lparentsym);
                                break;

                            case ')':
                                tokenSym(rparentsym);
                                break;

                            case '=':
                                tokenSym(eqsym);
                                break;

                            case ',':
                                tokenSym(commasym);
                                break;

                            case '.':
                                tokenSym(periodsym);
                                break;

                            case ';':
                                tokenSym(semicolonsym);
                                break;

                            // Default is invalid.
                            default:
                                error_code[3] = true;
                                return;
                            }
                        }
                    }
                }

                // An invalid symbol should throw an error and stops execution immediately.
                if (errorflag) {
                    error_code[3] = true;
                    return;
                }
            }
        }
        tracer++;
    }
    // Add null terminator to the end of the tokenized output.
    tokenstream[token_index] = '\0';

    return;
}

void report(char* stream) {

    printf("\n%s", stream);

    return;
}

int invalid(char value) {

    // Identify invisible non-whitespace characters.
    if (value == '\n' || value == '\t') return 1;

    return 0;
}

int isLetter(char value) {

    // Identify whether the new value is an alphabetical letter.
    if ((value >= ASCII_A && value <= ASCII_Z) || (value >= ASCII_LA && value <= ASCII_LZ)) return 1;

    return 0;
}

int isDigit(char value) {

    // Identify whether the new value is a numerical digit.
    if (value >= ASCII_0 && value <= ASCII_9) return 1;

    return 0;
}

void tokenWord(int word_id, char* buffer) {
    // Local variables.
    int n, sym_id;
    int l = strlen(buffer);

    // Write the given word to the token buffer. If it is an identifier, take this procedure.
    if (word_id == -1) {

        // First, symbol token, then name.
        tokenstream[token_index++] = '2';
        tokenstream[token_index++] = ' ';
        for (n = 0; n < l; n++) {
            tokenstream[token_index++] = buffer[n];
        }
        tokenstream[token_index++] = ' ';
    }

    // If a reserved word, take it and write it to the token buffer.
    else {

        // Identify the correct symbol.
        switch(word_id) {

        case 0:
            sym_id = constsym;
            break;

        case 1:
            sym_id = varsym;
            break;

        case 2:
            sym_id = procsym;
            break;

        case 3:
            sym_id = callsym;
            break;

        case 4:
            sym_id = beginsym;
            break;

        case 5:
            sym_id = endsym;
            break;

        case 6:
            sym_id = ifsym;
            break;

        case 7:
            sym_id = thensym;
            break;

        case 8:
            sym_id = elsesym;
            break;

        case 9:
            sym_id = whilesym;
            break;

        case 10:
            sym_id = dosym;
            break;

        case 11:
            sym_id = readsym;
            break;

        case 12:
            sym_id = writesym;
            break;

        case 13:
            sym_id = oddsym;
            break;

        default:
            break;
        }
        char tempbuf[(sizeof(int)*8)+1];
        int n;

        itoa(sym_id, tempbuf, 10);
        l = strlen(tempbuf);

        for (n = 0; n < l; n++) {
            tokenstream[token_index++] = tempbuf[n];
        }
        tokenstream[token_index++] = ' ';
    }

    return;
}

void tokenNum(char* buffer, int length) {
    // Local variables.
    int n = 0;

    // Copy number.
    while (buffer[n] != '\0') {
        tokenstream[token_index++] = buffer[n++];
    }
    tokenstream[token_index++] = ' ';

    return;
}

void tokenSym(int sym_id) {
    // Local variables.
    char tempbuf[(sizeof(int)*8)+1];
    int n;

    itoa(sym_id, tempbuf, 10);
    int l = strlen(tempbuf);

    for (n = 0; n < l; n++) {
        tokenstream[token_index++] = tempbuf[n];
    }
    tokenstream[token_index++] = ' ';

    return;
}
