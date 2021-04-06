#include "argvhandler.h"
#include "secondexercise.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) 
{
	
	srand(time(NULL));

	if (handle_argv(argc, argv) == 1337)
	{
		testmode();
	}

	return 0;
}
