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

char** code;
int lineNumber;
int columnNumber;
char* tokens;
int token;
int tokenIndex;
symbol symbolTable[MAX_SYMBOLS];
int symbolTableSize;

int block();
int program();
int getToken();
int statement();
int condition();
int expression();
int term();
int factor();
int identExists();

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
	printf("HOLLA\n");
	strcpy(symbolTable[symbolTableSize].name, name);
	symbolTable[symbolTableSize].kind = type;
	symbolTable[symbolTableSize].val = value;//check to see if number is within range
	printf("HOLLA\n");
	symbolTableSize++;
	free(name);
	return 0;
}

int block()
{
	char* identName;
	//should probably check for begins statement

	if(token == 28)//constsym
	{
		do 
		{
			getToken();

			if(token != 2) // identsym
			{ error(4); return -1;}
			
			identName = getIdentName();
			
			printf("TOKEN BOYS:%d %s\n", token, identName);

			if(token != 9) //eqsym
			{ error(3); return -1;}
			
			getToken();// check if isalpha in here and return value 
			
			if(isalpha(token))
			{ error(2); return -1;}
			
			addSymbolToTable(identName, 1, token);//add a const to the table

			getToken();		

		} while(token == 17);//commasym
		
		getToken();
	}
	if(token == 29)//intsym == varsym
	{
		do
		{
			getToken();
			
			if(token != 2) //identsym
			{ error(4); return -1;}
			
			identName = getIdentName();

			addSymbolToTable(identName, 2, 0); // add variable to table
			
		} while(token == 17); // comma
		
		if(token != 18) // semicolon
		{ error(5); return -1;}
		
		getToken();		
	} 
	while(token == 30) //procsym
	{
		getToken();
		
		if(token != 2) //identsym
		{ error(4); return -1;}
		
		identName = getIdentName();
		
		addSymbolToTable(identName, 3, 0); // add procedure to table

		if(token != 18) //semicolon
		{ error(5); return -1;}
		
		getToken();
		
		if(block() == -1)
			return -1;
		
		if(token != 18) // semiColon
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
		while(token == 2)
		{
			identName = getIdentName();
		
			printf("IDENTIFIER:%s\n", identName);
		
			if(!identExists(identName))
			{ error(11); return -1;}

			printf("TOKEN:%d\n", token);

			if(token != 20) //becomessym 
			{ error(3); return -1;}
		
			printf("CHEESE!\n");
			getToken(); 
		
			if(expression() == -1)
				return -1;
		}
	}

	else if(token == 27) // callsym
	{
		getToken();
		
		if(token != 2) // identsym
		{ error(14); return -1;}
		
		identName = getIdentName(); //do something with this 

		if(!identExists(identName))//procedure does not exist (maybe change the error message or search critera to be more specific)
		{ error(11); return -1;} 
	}
	else if(token == 21) // beginsym
	{
		getToken();
		
		if(statement() == -1)
			return -1;
		
		while(token == 18) // semicolon
		{
			getToken();
			
			statement();
		}
		printf("TOKEN:%d\n", token);
		if(token != 22) // endsym
		{ error(7); return -1;}

		getToken();
	}
	else if(token == 23) // ifsym
	{
		getToken();
		
		condition();
		
		if(token != 24) // thensym
		{ error(16); return -1;}

		getToken();

		if(statement() == -1)
			return -1;
	}
	else if(token == 25) // whilesym
	{
		getToken();

		condition();

		if(token != 26) // dosym
		{ error(18); return -1;}
		
		getToken();
		
		if(statement() == -1)
			return -1;
	}
	return 0;
}

int condition()
{
	if(token == 8) // oddsym
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
	if(token == 4 || token == 5) // plussym minussym
	{
		getToken();
		term();
		
		while(token == 4 || token == 5) // plussym minussym
		{
			getToken();
			term();
		}
	}

	if(token == 2)
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
	
	while(token == 6 || token == 7) // multsym slashsym
	{
		getToken();
		factor();
	}
	return 0;
}

int factor()
{
	if(token == 2) // identsym
	{
		getToken();
	}
	else if(isdigit(token))//figure out if i need to change this
	{
		getToken();
	}
	else if(token == 15) // lparentsym '('
	{
		getToken();
		
		expression();
		
		if(token != 16) // rparentsym ')'
		{ error(22); return -1;}
		
		getToken();
	}
	else
	{
		error(1);
	}
	return 0;
}

//returns 1 if ident found 0 otherwise
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


