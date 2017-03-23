#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Compiler.h"

typedef struct
{
	int kind;	//const = 1, var = 2, proc = 3 
	char name[11];	//name up to 10 chars
	int val;	//numbers(ASCII value)
	int level;	//L level
	int addr;	//M address
}symbol;

typedef enum
{
	nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,  slashsym = 7,
	oddsym = 8,  eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, 
	lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18, periodsym = 19, 
	becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25, 
	dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31, 
	readsym = 32, elsesym = 33

} token_type;

char** code;
int lineNumber;
int columnNumber;
char* tokens;
int token;
int tokenIndex;
symbol symbolTable[MAX_SYMBOLS];
int symbolTableSize;

int block();
int getToken();
int statement();
int condition();
int expression();
int term();
int factor();
int identExists();
int rOp();
void error(int code);

void error(int code)
{	
	printf("ERROR:");
	switch(code)
	{
		case 1:
			printf("Use = instead of :=. (code:%d)\n", code);
			break;
		case 2:
			printf("= must be followed by a number.(code:%d)\n", code);
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

int main(int argc, char* argv[])
{
	int tokensFlag = 0;
	int assemblyFlag = 0; 
	int printVMFlag = 0;
	int i;
	char* filename;
	symbolTableSize = 0;
	tokenIndex = 0;

	for(i = 0; i < argc; i++)
	{
		if(strcmp(argv[i], "-l") == 0)
			tokensFlag = 1;
		else if(strcmp(argv[i],"-a") == 0)
			assemblyFlag = 1;
		else if(strcmp(argv[i],"-v") == 0)
			printVMFlag = 1;
		else
			tokens = convert2Tokens(argv[i]);
	}

	int toklen = strlen(tokens);
	
	for(i = 0; i < toklen; i++)
		printf("%c", tokens[i]);
	printf("\n");	
	
	getToken();

	if(!block())
		printf("YAY NO ERRORS!\n");
	
	return 0;
}

// used to get the identifier name from tokens and return it
char* getIdentName()
{
	char* name = malloc(sizeof(char)* 11);
	int tokLen = strlen(tokens);
	int i = 0;
	char buffer[10];
	
	//no identifier!
	if(!isalpha(tokens[tokenIndex]))
	{ error(4); return NULL;}

	//shouldnt be more than 11 i think the lexi takes care of it
	while(isalpha(tokens[tokenIndex + i]))
	{
		name[i] = tokens[tokenIndex + i];

		i++;
	}
	

	tokenIndex += i + 1;

	name[i] = '\0';

	i = 0;

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

// returns -1 if at end of tokens array
int getToken()
{
	int tokLen = strlen(tokens);
	int i = 0;
	char num[10];

	while(tokens[tokenIndex + i] != ' ')
	{
		num[i] = tokens[tokenIndex + i];
		i++;
	}
	num[i] = '\0';

	token = atoi(num);
	
	i++;//move past space
	
	if(tokenIndex + i < tokLen)
		tokenIndex+= i;
	else
		return -1;
	
	return 0;
}

int addSymbolToTable(char* name, int type, int value)
{
	strcpy(symbolTable[symbolTableSize].name, name);

	symbolTable[symbolTableSize].kind = type;

	symbolTable[symbolTableSize].val = value;//check to see if number is within range

	symbolTableSize++;

	free(name);

	return 0;
}

int block()
{
	char* identName;
	//should probably check for begins statement

	if(token == constsym)//constsym
	{
		do 
		{
			getToken();

			if(token != identsym) // identsym
			{ error(4); return -1;}
			
			identName = getIdentName();
			
			printf("TOKEN BOYS:%d %s\n", token, identName);

			if(token != eqlsym) //eqsym
			{ error(3); return -1;}
			
			getToken();// check if isalpha in here and return value 
			
			if(token != numbersym) // number
			{ error(2); return -1;}
			
			getToken();
			
			addSymbolToTable(identName, 1, token);//add a const to the table

			getToken();		

		} while(token == 17);//commasym
		
		getToken();
	}
	if(token == varsym)//intsym == varsym
	{
		do
		{
			getToken();
			
			if(token != identsym) //identsym
			{ error(4); return -1;}
			
			identName = getIdentName();

			addSymbolToTable(identName, 2, 0); // add variable to table
			
		} while(token == commasym); // comma
		
		if(token != semicolonsym) // semicolon
		{ error(5); return -1;}
		
		getToken();		
	} 
	while(token == procsym) //procsym
	{
		getToken();
		
		if(token != identsym) //identsym
		{ error(4); return -1;}
		
		identName = getIdentName();
		
		addSymbolToTable(identName, 3, 0); // add procedure to table

		if(token != semicolonsym) //semicolon
		{ error(5); return -1;}
		
		getToken();
		
		if(block() == -1)
			return -1;
		
		if(token != semicolonsym) // semiColon
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

	if(token == 2) //identsym 
	{
		identName = getIdentName();
	
		if(!identExists(identName))
		{ error(11); return -1;}

		if(token != becomessym) //becomessym 
		{ error(3); return -1;}
	
		getToken(); 

		if(expression() == -1)
			return -1;
	}

	else if(token == callsym) // callsym
	{
		getToken();
		
		if(token != identsym) // identsym
		{ error(14); return -1;}
		
		identName = getIdentName();

		if(!identExists(identName))//procedure does not exist (maybe change the error message or search critera to be more specific)
		{ error(11); return -1;} 
	}
	else if(token == beginsym) // beginsym
	{
		getToken();
		
		if(statement() == -1)
			return -1;
		
		while(token == semicolonsym) // semicolon
		{
			getToken();
			
			statement();
			
		}
		printf("TOKEN:%d\n", token);
		if(token != endsym) // endsym
		{ error(7); return -1;}
		
		getToken();
	}
	else if(token == ifsym) // ifsym
	{
		getToken();
		
		condition();
		
		if(token != thensym) // thensym
		{ error(16); return -1;}

		getToken();

		if(statement() == -1)
			return -1;
	}
	else if(token == whilesym) // whilesym
	{
		getToken();

		condition();

		if(token != dosym) // dosym
		{ error(18); return -1;}
		
		getToken();
		
		if(statement() == -1)
			return -1;
	}
	return 0;
}

int condition()
{
	char* ident;
	
	if(token == oddsym) // oddsym
	{
		getToken();

		expression();
	}
	else
	{
		expression();
		
		if(!rOp())
			{error(13); return -1;}

		getToken();
		expression();
	}
	return 0;
}

int expression()
{
	int addop;
	
	if(token == 4 || token == 5)// plussym minussym
	{
		addop = token;
		getToken();
		term();
		while(token == 6 || token == 7)
		{
			getToken();
			term();
		}
	}
	else
		term();

	while(token  == plussym || token == minussym)
	{
		addop = token;
		getToken();
		term();
		//assembly stuff here
	}

	return 0;
}

int term()
{
	factor();
	
	while(token == 6 || token == 7) // multsym slashsym
	{
		getToken();
		factor();
	}
	return 0;
}

int factor()
{
	char* ident; 
	int flag = 0;

	if(token == 2) //identsym
	{
		ident = getIdentName();
	}
	else if(token == 3) // number
	{
		getToken();
		
		getToken();//get to semi colon	
	}
	else if(token == 15) // (
	{
		getToken();

		if(expression() == -1)
			return -1;

		if(token != 16) // )
			{ error(22); return -1;}
		getToken();
	}
	else 
	{
		error(1);//idk what code to put here
		return -1;
	}	

	return 0;
}

// check to see if the token is a vaild relational operator and return correct assembly operation
int rOp()
{
	switch(token)
	{
		 case eqlsym:
            		return 19;  // 19 EQL
			break;
		case neqsym:
			return 20;  // 20 NEQ
			break;
		case lessym:
			return 21;  // 21 LSS
			break;
		case leqsym:
			return 22;  // 22 LEQ
			break;
		case gtrsym:
			return 23;  // 23 GTR
			break;
		case geqsym:
			return 24;  // 24 GEQ
			break;
		default:
			return 0;	
	}
}

// check to see if identifier is in the symbol table
// returns 1 if ident found 0 otherwise
int identExists(char* ident)
{
	int i;
	for(i = 0; i < symbolTableSize; i++)
	{
		if(strcmp(ident, symbolTable[i].name))
			return 1;
	}
	return 0;
}


