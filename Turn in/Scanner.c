#include "Compiler.h"

#define MAX_CHARS 11
#define MAX_NUMS 5
#define CODE_CHAR_MAX 10000
char  *word [ ] = {"", "null", "ident", "number", "+", "-", "*", "/", "odd", "=", "!=", "<", "<=", ">", ">=", "(", ")", ",", ";", ".", ":=", "begin", "end", "if", "then", "while", "do", "call", "const", "var", "procedure", "write", "read", "else"};

char *symbolicRep[] = {"", "nulsym", "identsym", "numbersym", "plussym", "minussym", "multsym", "slashsym", "oddsym", "eqlsym", "neqsym", "lessym", "leqsym", "gtrsym", "geqsym", "lparentsym", "rparentsym", "commasym", "semicolonsym", "periodsym", "becomessym", "beginsym", "endsym", "ifsym", "thensym", "whilesym", "dosym", "callsym", "constsym", "varsym", "procsym", "writesym", "readsym", "elsesym"};

typedef struct
{
	int token;
	char* string;
}SYMBOL;

SYMBOL* findSymbol(char* str);
int isStrDigit(char* str)
{
	int i;
	for(i = 0; i < strlen(str); i++)
		if(!isdigit(str[i]))
			return 0;
	return 1;
}

//checks to see if temp is a punctation, or mathematical operator
int checkSymbols(char* temp)
{
	int i;
	for(i = 4; i < 20; i++)
	{
		if(strcmp(temp, word[i]) == 0)
			return 1;
	}
	return 0;
}

char* scanner(char* fileName, int tokensFlag, FILE* output, int* errorFlag)
{
	char* tokens = malloc(sizeof(char) * MAX_TOKENS);
	tokens[0] = '\0';
	tokens[MAX_TOKENS - 1] = '\0';

	FILE* file;
	int i= 0, j = 0;
	char temp[12];
	SYMBOL* symbol_found;
	int str_size;
	char buffer[100];
	file = fopen(fileName, "r");
	SYMBOL** symbols = malloc(sizeof(SYMBOL*) * 100);
	int num_symbols = 0;

	char code[CODE_CHAR_MAX];
	int codeCount = 0;
	int comments = 0;
	
	while(fgets(buffer, 100, file))
	{
		while(buffer[i] != '\0')
		{
			code[codeCount] = buffer[i];
			codeCount++;
			i++;
		}
		i = 0;
	}

	for(i = 0; i < codeCount; i++)
	{
		if(code[i] == '/' && code[i + 1] == '*')
		{
			i+=2;
			while(code[i] != '*' && code[i + 1] != '/' && i < codeCount)
			{
				i++;
			}
			i+=2;
		}

		if(isalpha(code[i]) || isdigit(code[i]) || ispunct(code[i]))
		{
			temp[j] = code[i];
			if(temp[j] == ':')
			{
				if(i + 1 < codeCount)
				{
					if(code[i + 1] == '=')
					{
						temp[j+1] = code[i+1];
						temp[j+2] = '\0';
						j+=3;
						i+=1;
					}
					else
						fprintf(output, "SCANNER: Unrecognized Symbol: %c\n", temp[j]);
				}
				else
						fprintf(output, "SCANNER: Unrecognized Symbol: %c\n", temp[j]);
			}
			else
			{
				temp[j+1] = '\0';
				j++;
			}
		}		

		
		symbol_found = findSymbol(temp);
		str_size = strlen(temp);

		if(symbol_found == NULL && str_size > 0)
		{
			if(i + 1 < codeCount)
			{
				//check to make sure variable is finished (find space or non letter)
				if(!isalpha(code[i + 1]) && !isdigit(code[i + 1]))
				{	
					if(isStrDigit(temp))
					{
						if(str_size <= MAX_NUMS)
						{
							symbol_found = malloc(sizeof(SYMBOL*));
							symbol_found->string = malloc(sizeof(temp));
							symbol_found->token = 3;
							strcpy(symbol_found->string, temp);
							symbols[num_symbols] = symbol_found;
							num_symbols++;
						}
						else
						{
							fprintf(output, "SCANNER: ERROR: Number digits exceeded! Digits max:%d [%s]\n", MAX_NUMS, temp);
							*errorFlag = 1;
						}
					}
					else if(!isalpha(temp[0]) && isdigit(temp[0]))
					{
						fprintf(output, "SCANNER: ERROR: Variable name cannot start with a number! [%s]\n", temp);
						*errorFlag = 1;
					}
					else if(str_size >= MAX_CHARS)
					{
						fprintf(output, "SCANNER: ERROR: Variable character length exceeded! Character max:%d [%s]\n", MAX_CHARS, temp);
						*errorFlag = 1;
					}
					else
					{
						
						symbol_found = malloc(sizeof(SYMBOL*));
						symbol_found->string = malloc(sizeof(temp));
						strcpy(symbol_found->string, temp);
						symbol_found->token = 2;//identsym
						symbols[num_symbols] = symbol_found;
						num_symbols++;
					}
					j = 0;
					temp[0] = '\0';
				}
			}
		}
		else
		{
			

			
			if(checkSymbols(temp))
			{
				temp[j] = code[i + 1];
				temp[j + 1] = '\0';

				if(!checkSymbols(temp))
				{
					symbols[num_symbols] = symbol_found;
					num_symbols++;
					j = 0;
					temp[0] = '\0';
				}
				
				temp[j] = '\0';

			}
			else if(!isalpha(code[i + 1]) && !isdigit(code[i + 1]) && str_size > 0)
			{
				symbols[num_symbols] = symbol_found;
				num_symbols++;
				j = 0;
				temp[0] = '\0';
			}
		}
	}
	fprintf(output, "\n SCANNER: Source Program:\n");
	for(i = 0; i < codeCount; i++)
	{
		fprintf(output, "%c", code[i]);
	}
	fprintf(output, "\n");

		
	for(i = 0; i < num_symbols; i++)
	{
		if(symbols[i]->token == 2)
		{
			strcat(tokens, "2 ");
			strcat(tokens, symbols[i]->string);
			strcat(tokens, " ");
		}
		else if(symbols[i]->token == 3)
		{
			strcat(tokens, "3 ");
			strcat(tokens, symbols[i]->string);
			strcat(tokens, " ");
		}
		else
		{
			snprintf(buffer, 10, "%d ", symbols[i]->token);
			strcat(tokens, buffer);
		}
		
	}
	if(tokensFlag)
	{
		printf("\nLexeme List:\n");
		printf("%s\n", tokens);
		printf("\n");

		for(i = 0; i < num_symbols; i++)
		{
			if(symbols[i]->token == 2 || symbols[i]->token == 3)
				printf("%s ", symbols[i]->string);
			else
				printf("%s ", symbolicRep[symbols[i]->token]);
		}
		printf("\n");
	}

	return tokens;
}


// Checks to see if str is a symbol and then returns the correct code
// returns -1 otherwise
SYMBOL* findSymbol(char* str)
{
	SYMBOL* sym = malloc(sizeof(SYMBOL));
	int k;
	for(k = 0; k < 33; k++)
	{
		if(strcmp(str, word[k]) == 0)
		{
			sym->token = k;
			sym->string =  malloc(sizeof(str));
			strcpy(sym->string, str);
			return sym;
		}
	}
	return NULL;
}
