// COP 3402 - Systems Software
// 3-24-17 | Austin Peace & Andrew Emery
// Programming Assignment 3 - Parser/CodeGenerator

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Compiler.h"

// STRUCTURES, PROTOTYPES, & ENUMERATIONS.
typedef struct
{
	int kind;	//const = 1, var = 2, proc = 3
	char name[11];	//name up to 10 chars
	int val;	//numbers(ASCII value)
	int level;	//L level
	int addr;	//M address
} symbol;

char** code;
int lineNumber;
int columnNumber;
char* tokens;
int token;
int tokenIndex;
symbol symbolTable[MAX_SYMBOLS];
int symbolTableSize;

char* getIdentName();
int getToken();
int addSymbolToTable(char* name, int type, int value);
int block();
int statement();
int condition();
int expression();
int term();
int factor();
int identExists();

typedef enum {
nulsym = 1, identsym, numbersym, plussym, minussym,
multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym } token_type;

typedef enum {
wrongdef = 1,

void error(int code);

void error(int code)
{
	printf("ERROR: ");
	switch(code)
	{
		case 1:
			printf("Use = instead of :=. (code:%d)\n", code);
			break;
		case 2:
			printf("'=' must be followed by a number.(code:%d)\n", code);
			break;
		case 3:
			printf("Identifier must be followed by =.(code:%d)\n", code);
			break;
		case 4:
			printf("const, var, procedure must be followed by identifier.(code:%d)\n", code);
			break;
		case 5:
			printf("Semicolon or comma missing.(code:%d)\n", code);
			break;
		case 6:
			printf("6. Incorrect symbol after procedure declaration.(code:%d)\n", code);
			break;
		case 7:
			printf("7. Statement expected.(code:%d)\n", code);
			break;
		default:
			printf("Unrecognized Error (code:%d)\n", code);
	}
}

// MAIN PROGRAM.
int main(int argc, char* argv[])
{
	// Local variables.
	int tokensFlag = 0;
	int assemblyFlag = 0;
	int printVMFlag = 0;
	int i;
	char* filename;
	symbolTableSize = 0;
	tokenIndex = 0;

	// Get program arguments.
	for(i = 0; i < argc; i++)
	{
		// Output tokens to terminal.
		if (strcmp(argv[i], "-l") == 0)
			tokensFlag = 1;

		// Output assembly code to terminal.
		else if (strcmp(argv[i],"-a") == 0)
			assemblyFlag = 1;

		// Output VM execution trace to terminal.
		else if (strcmp(argv[i],"-v") == 0)
			printVMFlag = 1;
		else
			tokens = convert2Tokens(argv[i]); /** ADD ARGUMENT TO DISABLE/ENABLE OUTPUTS **/
	}

	// Get length of token string.
	int toklen = strlen(tokens);

	// Print each token.
	for(i = 0; i < toklen; i++)
		printf("%c", tokens[i]);
	printf("\n");

	// Get next token.
	getToken();

	// Run block. Detect any errors in syntax during procedure.
	if(!block())
		printf("YAY NO ERRORS!\n");

	return 0;
}

// Gets identifier name from tokens and returns it.
char* getIdentName()
{
	char* name = malloc(sizeof(char)* 11);
	int tokLen = strlen(tokens);
	int i = 0;
	char buffer[10];

	// Error: No identifier!
	if(!isalpha(tokens[tokenIndex]))
	{ error(4); return NULL;}

	// Error: Name shouldn't be longer than 11.
	while(isalpha(tokens[tokenIndex + i]))
	{
		name[i] = tokens[tokenIndex + i];

		i++;
	}

	tokenIndex += i + 1;

	name[i] = '\0';

	i = 0;

    // Read string into buffer.
	while(tokens[tokenIndex + i] != ' ')
	{
		buffer[i] = tokens[tokenIndex + i];
		i++;
	}

	buffer[i] = '\0';

	tokenIndex += i + 1;

	printf("I: [%c]\n", tokens[tokenIndex]);

	token = atoi(buffer);

	return name;
}

// Returns -1 if at end of tokens array.
int getToken()
{
    // Local variables.
	int tokLen = strlen(tokens);
	int i = 0;
	char num[10];

    // Read tokenized string.
	while(tokens[tokenIndex + i] != ' ')
	{
		num[i] = tokens[tokenIndex + i];
		i++;
	}
	num[i] = '\0';

	token = atoi(num);

	i++; //move past space

	if(tokenIndex + i < tokLen)
		tokenIndex+= i;
	else
		return -1;

	return 0;
}

// Incorporates next symbol into the symbol table.
int addSymbolToTable(char* name, int type, int value)
{
	printf("HOLLA\n");
	strcpy(symbolTable[symbolTableSize].name, name);
	symbolTable[symbolTableSize].kind = type;
	symbolTable[symbolTableSize].val = value;//check to see if number is within range
	printf("HOLLA\n");
	symbolTableSize++;
	free(name);
	return 0;
}

// Block procedure.
int block()
{
    // Character name.
	char* identName;

	if(token == constsym)
	{
		do
		{
			getToken();

			if(token != identsym)
			{ error(4); return -1;}

			identName = getIdentName();

			printf("TOKEN BOYS:%d %s\n", token, identName);

			if(token != eqsym)
			{ error(3); return -1;}

			getToken();

			// check if token is letter and return value.
			if(isalpha(token))
			{ error(2); return -1;}

			addSymbolToTable(identName, 1, token); // Add const to the table.

			getToken();

		} while(token == commasym);

		getToken();
	}
	if(token == varsym)
	{
		do
		{
			getToken();

			if(token != identsym)
			{ error(4); return -1;}

			identName = getIdentName();

			addSymbolToTable(identName, 2, 0); // Add variable to table.

		} while(token == commasym);

		if(token != semicolonsym)
		{ error(5); return -1;}

		getToken();
	}
	while(token == procsym)
	{
		getToken();

		if(token != identsym)
		{ error(4); return -1;}

		identName = getIdentName();

		addSymbolToTable(identName, 3, 0); // Add procedure to table.

		if(token != 18) //semicolon
		{ error(5); return -1;}

		getToken();

		if(block() == -1)
			return -1;

		if(token != semicolonsym)
		{ error(5); return -1;}

		getToken();
	}

	if(statement() == -1)
		return -1;

	return 0;
}

int statement()
{
	char* identName;

	if(token == identsym)
	{
		while(token == identsym)
		{
			identName = getIdentName();

			printf("IDENTIFIER:%s\n", identName);

			if(!identExists(identName))
			{ error(11); return -1;}

			printf("TOKEN:%d\n", token);

			if(token != becomessym)
			{ error(3); return -1;}

			printf("CHEESE!\n");
			getToken();

			if(expression() == -1)
				return -1;
		}
	}

	else if(token == callsym)
	{
		getToken();

		if(token != identsym)
		{ error(14); return -1;}

		identName = getIdentName(); // Get identifier name.

		if(!identExists(identName)) // Check to make sure that identifier has been defined.
		{ error(11); return -1;}
	}
	else if(token == beginsym)
	{
		getToken();

		if(statement() == -1)
			return -1;

		while(token == semicolonsym)
		{
			getToken();

			statement();
		}
		printf("TOKEN:%d\n", token);
		if(token != endsym)
		{ error(7); return -1;}

		getToken();
	}
	else if(token == ifsym)
	{
		getToken();

		condition();

		if(token != thensym)
		{ error(16); return -1;}

		getToken();

		if(statement() == -1)
			return -1;
	}
	else if(token == whilesym)
	{
		getToken();

		condition();

		if(token != dosym)
		{ error(18); return -1;}

		getToken();

		if(statement() == -1)
			return -1;
	}
	return 0;
}

int condition()
{
	if(token == oddsym)
	{
		getToken();
		expression();
	}
	else
	{
		expression();
		//if TOKEN != RELATION then ERROR;
		if(token == '<' || token == '>')
		{
		}
		else if(token == '=')
		{
			getToken();
			if(token != '=')
			{ error(1); return -1;}
		}
		else if(token == '&')
		{
			getToken();
			if(token != '&')
			{ error(1); return -1;}
		}
		else if(token == '|')
		{
			getToken();
			if(token != '|')
			{ error(1); return -1;}
		}
		else
		{
			error(1);
			return -1;
		}
		getToken();
		expression();
	}
	return 0;
}

int expression()
{
	char* ident;
	if(token == plussym || token == minussym)
	{
		getToken();
		term();

		while(token == plussym || token == minussym)
		{
			getToken();
			term();
		}
	}

	if(token == identsym)
	{
		ident = getIdentName();

		if(!identExists(identName))
			{ error(11); return -1;}
	}
	return 0;
}

int term()
{
	factor();

	while(token == multsym || token == slashsym)
	{
		getToken();
		factor();
	}
	return 0;
}

int factor()
{
	if(token == identsym)
	{
		getToken();
	}
	else if(isdigit(token))
	{
		getToken();
	}
	else if(token == lparentsym)
	{
		getToken();

		expression();

		if(token != rparentsym)
		{ error(22); return -1;}

		getToken();
	}
	else
	{
		error(1);
	}
	return 0;
}

// Returns whether the identifier exists in the symbol table.
int identExists(char* ident)
{
	int i;
	for(i = 0; i < symbolTableSize; i++)
	{
		//printf("IDEN:%s SI:%s\n", ident, s)
		if(strcmp(ident, symbolTable[i].name))
			return 1;
	}
	return 0;
}

