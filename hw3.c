// COP 3402 - Systems Software
// 3-24-17 | Austin Peace & Andrew Emery
// Programming Assignment 3 - Parser/Code Generator

#include "Compiler.h"

//stack pointer
int sp;

//max lex level
int level;

//current lex level
int cLevel;

int lineNumber;
int columnNumber;
char* tokens;
int token;
int tokenIndex;
symbol symbolTable[MAX_SYMBOLS];
int symbolTableSize;
int cx; //code index
code_struct code[MAX_CODE_LENGTH];
int reg;

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
			printf("Identifier must be followed by =. (code:%d)\n", code);
			break;
		case 4:
			printf("const, var, procedure must be followed by identifier. (code:%d)\n", code);
			break;
		case 5:
			printf("Semicolon or comma missing. (code:%d)\n", code);
			break;
		case 6:
			printf("6. Incorrect symbol after procedure declaration. (code:%d)\n", code);
			break;
		case 7:
			printf("7. Statement expected.(code:%d)\n", code);
			break;
		case 8:
			printf("Incorrect symbol after statement part in block. (code:%d)\n", code);
			break;
		case 9:
			printf("Period expected. (code:%d)\n", code);
			break;
		case 10:
			printf("Semicolon between statements missing. (code:%d)\n", code);
			break;
		case 11:
			printf("Undeclared identifier. (code:%d)\n", code);
			break;
		case 12:
			printf("Assignment to constant or procedure is not allowed. (code:%d)\n", code);
			break;
		case 13:
			printf("Assignment operator expected. (code:%d)\n", code);
			break;
		case 14:
			printf("Call must be followed by an identifier. (code:%d)\n", code);
			break;
		case 15:
			printf("Call of a constant or variable is meaningless. (code:%d)\n", code);
			break;
		case 16:
			printf("Then expected.(code:%d)\n", code);
			break;
		case 17:
			printf("Semicolon or } expected. (code:%d)\n", code);
			break;
		case 18:
			printf("Do expected. (code:%d)\n", code);
			break;
		case 19:
			printf("Incorrect symbol following statement. (code:%d)\n", code);
			break;
		case 20:
			printf("Relational operator expected. (code:%d)\n", code);
			break;
		case 21:
			printf("Expression must not contain a procedure identifier. (code:%d)\n", code);
			break;
		case 22:
			printf("Right parenthesis missing. (code:%d)\n", code);
			break;
		case 23:
			printf("The preceding factor cannot begin with this symbol. (code:%d)\n", code);
			break;
		case 24:
			printf("An expression cannot begin with this symbol. (code:%d)\n", code);
			break;
		case 25:
			printf("This number is too large. (code:%d)\n", code);
			break;
		default:
			printf("Unrecognized Error (code:%d)\n", code);
	}
}

void emit(int op, int r, int l, int m)
{
	if(cx > MAX_CODE_LENGTH)
	{
		error(25);
	}
	else
	{
		code[cx].op = op;
		code[cx].r = r;
		code[cx].l = l;
		code[cx].m = m;
		cx++;
	}
}

int main(int argc, char* argv[])
{
	int tokensFlag = 0;
	int assemblyFlag = 0;
	int printVMFlag = 0;
	int i;
	FILE* codeOutput;
	symbolTableSize = 0;
	tokenIndex = 0;
	cx = 0;
	sp = 1;
	level = cLevel = 1;

	for(i = 1; i < argc; i++)
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

	//if no error
	if(!program())
	{
		printf("YAY no error!\n");
	}

	codeOutput = fopen("assembly.txt", "w");

	for(i = 0; i < cx; i++)
	{
		fprintf(codeOutput, "%d %d %d %d\n", code[i].op, code[i].r, code[i].m, code[i].l);
	}
	fclose(codeOutput);
	return 0;
}

int program()
{
	getToken();

	if(block())
		return -1;

	getToken();

	if(token != periodsym)
		{ error(9); return -1;}

	emit(rtn, 0, 0, 0);// im pretty sure this should be return
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

int addSymbolToTable(char* name, int type, int value, int l, int m)
{
	strcpy(symbolTable[symbolTableSize].name, name);

	symbolTable[symbolTableSize].kind = type;

	symbolTable[symbolTableSize].val = value;//check to see if number is within range

	symbolTable[symbolTableSize].level = l;

	symbolTable[symbolTableSize].addr = m;

	symbolTableSize++;

	free(name);

	return 0;
}

int block()
{
	char* identName;

	sp = 3;

	int space = 4;
	int procI; // procedure index

	int jumpAddress = cx;

	emit(jmp, 0, 0, 0);

	if(token == constsym)//constsym
	{
		do
		{
			getToken();

			if(token != identsym) // identsym
			{ error(4); return -1;}

			identName = getIdentName();

			if(token != eqlsym) //eqsym
			{ error(3); return -1;}

			getToken();// check if isalpha in here and return value

			if(token != numbersym) // number
			{ error(2); return -1;}

			getToken();

			addSymbolToTable(identName, 1, token, 0, 0);//add a const to the table

			getToken();

		} while(token == 17);//commasym

		//check for ;<------------------------------------------------------------------------------------

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

			addSymbolToTable(identName, 2, 0, level, sp); // add variable to table

			space++;

			sp++;

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

//CHECK TO SEE IF THIS IS RIGHT!!!
		addSymbolToTable(identName, 3, 0, level, jumpAddress + 1); // add procedure to table

		if(token != semicolonsym) //semicolon
		{ error(5); return -1;}

		getToken();

		if(block() == -1)
			return -1;

		if(token != semicolonsym) // semiColon
		{ error(5); return -1;}

		getToken();
	}

	code[jumpAddress].m = cx;

	if(statement() == -1)
		return -1;

	cLevel--;

	return 0;
}

int statement()
{
	char* identName;
	int index;
	int cTemp;

	if(token == 2) //identsym
	{
		identName = getIdentName();

		index = identExists(identName);

		if(index == -1)
			{ error(11); return -1;}
		else if(symbolTable[index].kind == 1 || symbolTable[index].kind == 3) 	// if const or procedure
			{ error(12); return -1;}

		if(token != becomessym) //becomessym
			{ error(3); return -1;}

		getToken();

		if(expression() == -1)
			return -1;

		emit(sto, reg - 1, cLevel - symbolTable[index].level, symbolTable[index].addr - 1);

		reg--;
	}

	else if(token == callsym) // callsym
	{
		getToken();

		if(token != identsym) // identsym
		{ error(14); return -1;}

		identName = getIdentName();

		index = identExists(identName);

		if(index == -1)
			{ error(11); return -1;}
		else if(symbolTable[index].kind == 1 || symbolTable[index].kind == 2) 	// if const or var
			{ error(15); return -1;}

		emit(cal, 0, level, symbolTable[index].addr);
		cLevel++;

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

		cTemp = cx;

		emit(jpc, reg - 1, 0, 0);

		if(statement() == -1)
			return -1;

		//ADD IN ELSE?

		code[cTemp].m = cx;
	}
	else if(token == whilesym) // whilesym
	{
		int cx1 = cx;
		int cx2;
		getToken();

		condition();

		cx2 = cx;

		emit(jpc, reg-1, 0, 0);

		if(token != dosym) // dosym
		{ error(18); return -1;}

		getToken();

		if(statement() == -1)
			return -1;

		emit(jmp, 0, 0, cx1);

		reg--;

	}
	return 0;
}

int condition()
{
	char* ident;
	int operator;

	if(token == oddsym) // oddsym
	{
		getToken();

		expression();

		emit(odd, reg - 1, reg - 1, 0);
	}
	else
	{
		expression();
		operator = rOp();
		if(!operator)
			{error(13); return -1;}

		getToken();
		expression();

		emit(operator, reg - 2, reg - 2, reg - 1);
		reg--;
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
		if(addop == minussym)
			emit(neg, reg - 1, reg - 1, 0);
	}
	else
		term();

	while(token  == plussym || token == minussym)
	{
		addop = token;
		getToken();
		term();
		if(addop == plussym)
			emit(add, reg - 2, reg - 2, reg - 1);
		else
			emit(sub, reg - 2, reg - 2, reg - 1);

		reg--;
	}

	return 0;
}

int term()
{
	int mulop;

	factor();

	while(token == 6 || token == 7) // multsym slashsym
	{
		mulop = token;
		getToken();
		factor();
		if(mulop == multsym)
			emit(mul, reg - 2, reg - 2, reg - 1);//multiplication
		else
			emit(dvd, reg - 2, reg - 2, reg - 1);//division
		reg--;
	}
	return 0;
}

int factor()
{
	char* ident;
	int flag = 0;
	int index;

	if(token == 2) //identsym
	{
		ident = getIdentName();

		index = identExists(ident);

		if(index == -1)
			{ error(11); return -1;}
		else if(symbolTable[index].kind == 3) 	// if procedure
			{ error(21); return -1;}

		if(symbolTable[index].kind == 2)
			emit(lod, reg, 0, symbolTable[index].val);
		else if(symbolTable[index].kind == 1)
			emit(lit, reg, 0, symbolTable[index].val);

		reg++;

	}
	else if(token == 3) // number
	{
		getToken();

		emit(lit, reg, 0, token);

		reg++;

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
            		return eql;  // 19 EQL
			break;
		case neqsym:
			return neq;  // 20 NEQ
			break;
		case lessym:
			return lss;  // 21 LSS
			break;
		case leqsym:
			return leq;  // 22 LEQ
			break;
		case gtrsym:
			return gtr;  // 23 GTR
			break;
		case geqsym:
			return geq;  // 24 GEQ
			break;
		default:
			return 0;
	}
}

// check to see if identifier is in the symbol table
// returns index if ident found -1 otherwise
int identExists(char* ident)
{
	int i;
	printf("TOKEN INDEX:%d TOKEN:%d\n", tokenIndex, token);
	for(i = 0; i < symbolTableSize; i++)
	{
		if(strcmp(ident, symbolTable[i].name) == 0)
			return i;
	}
	return -1;
}


