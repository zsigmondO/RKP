#include "argvhandler.h"
#include <string.h>
#include <stdlib.h>

#define ENVFILENAME "sample.env"

void handle_argv(const int argc, const char** argv) {
	if (argc == 1) //the program was called without any option
	{
		printf("%c[1m", ESC); //changes text to bold
		printf("RKP main.c: ");

		printf("\033[0;31m"); //changes color to red
		printf("fatal error: ");
		printf("%c[0m", ESC); //changes text back to normal non-bold text
		printf("\033[0m"); //changes back to normal color
		
		printf("no input file\n");
		printf("compilation terminated.\n");

		printf("%c[1m", ESC); //changes text to bold
		printf("RKP tip: ");
		printf("%c[0m", ESC); //changes text back to normal non-bold text
		printf("use the --help command line option for further information.\n");

		exit(1);

	}
	else //the program was called with options, but...
	{
		for (int i = 1; i <= argc; ++i)
		{
			if (argv[i][0] == '-')
			{
				if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-version") == 0)
				{
					//TODO: open the sample.env file then print the required stuff
					FILE* fp = fopen(ENVFILENAME, "r");

					if (fp)
					{
						printf("%c[1m", ESC); //changes text to bold
						printf("RKP main.c: ");

						printf("\033[0;31m"); //changes color to red
						printf("fatal error: ");
						printf("%c[0m", ESC); //changes text back to normal non-bold text
						printf("\033[0m"); //changes back to normal color
						
						printf("failed opening the .env file\n");
						printf("compilation terminated.\n");

						fclose(fp);
						exit(1);
					}

					


					fclose(fp);
				} 
				else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-help") == 0) 
				{
					//TODO: tell the user what to do with the program
				} 
				else 
				{
					printf("%c[1m", ESC); //changes text to bold
					printf("RKP main.c: ");

					printf("\033[0;31m"); //changes color to red
					printf("error: ");
					printf("%c[0m", ESC); //changes text back to normal non-bold text
					printf("\033[0m"); //changes back to normal color
					
					printf("unrecognized command line option '%s'\n", argv[i]);
					printf("compilation terminated.\n");

					exit(1);

				}
			}
			else 
			{
				//TODO: check if file exists
			}
		}
	}

}