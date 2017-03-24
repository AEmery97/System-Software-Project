typedef struct
{
	int kind;	//const = 1, var = 2, proc = 3 
	char name[11];	//name up to 10 chars
	int val;	//numbers(ASCII value)
	int level;	//L level
	int addr;	//M address
}symbol;

typedef struct 
{
	int op;
	int r;
	int l;
	int m;
}code_struct;

typedef enum
{
	nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,  slashsym = 7,
	oddsym = 8,  eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, 
	lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18, periodsym = 19, 
	becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25, 
	dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31, 
	readsym = 32, elsesym = 33

} token_type;

//char** code;
int lineNumber;
int columnNumber;
char* tokens;
int token;
int tokenIndex;
symbol symbolTable[MAX_SYMBOLS];
int symbolTableSize;
int cx; //code index
code_struct code[CODE_SIZE];
int reg;

//stack pointer
int sp;

//max lex level
int level;

//current lex level
int cLevel;

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

