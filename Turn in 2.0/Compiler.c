// COP 3402 - Systems Software
// 3-24-17 | Austin Peace & Andrew Emery

#include "Compiler.h"

int main(int argc, char* argv[])
{
	int tokensFlag = 0;
	int assemblyFlag = 0;
	int VMFlag = 0;
	char* filename;
	int length = 0;
	char* tokens;
	FILE* output;
	int errorFlag = 0;
	int i;

	output = fopen("output.txt", "w");

	for(i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-l") == 0)
		{
			tokensFlag = 1;
		}
		else if(strcmp(argv[i],"-a") == 0)
		{
			assemblyFlag = 1;
		}
		else if(strcmp(argv[i],"-v") == 0)
		{
			VMFlag = 1;
		}
		else
		{
			length = strlen(argv[i]) + 1;
			filename = malloc(sizeof(char)* length);
			strcpy(filename, argv[i]);
		}
	}

		tokens = scanner(filename, tokensFlag, output, &errorFlag);
		if(!errorFlag)
			parser(tokens, output, assemblyFlag, &errorFlag);
		else
			printf("Scanning ERROR\n");
		if(!errorFlag)
			virtualMachine("assembly.txt", VMFlag);
		else
			printf("PARSING ERROR\n");

		//if(errorFlag) printf("\nAn error has occurred. Please see output.txt for more details.\n");

		fclose(output);

	return 0;

}
