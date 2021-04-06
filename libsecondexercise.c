#include "secondexercise.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DECODEDAMOUNT 8
#define TODECODE "abc"
#define LOWER 97
#define UPPER 122
#define TESTCOUNTER 5
#define ESC 27

//Returns a randomly selected char between a-z (lowercase)
char random_char_generator()
{
    return (rand() % (UPPER - LOWER + 1)) + LOWER;
}

/*
The function does the following things (in order):
	The integer, which was passed as a parameter (previously) will
	hold the value of the decoded characters as a pointer.
	The filling starts with the most important chars,
	the required "TODECODE" char sequence,
	after that the first 5 (remaining) places will be filled
	randomly (a-z lowercase).
	In the end, the function returns the memory adress of the newly
	allocated char sequence, and the number of the decoded (hidden
	in the back of the sequence) characters as a pointer, as I was
	mentioned it above.
*/
char* test_array(int* number_of_decoded_chars)
{
    char* to_be_returned = calloc(DECODEDAMOUNT, sizeof(char));

    int j = strlen(TODECODE) - 1;

    for (int i = DECODEDAMOUNT - 1; i >= DECODEDAMOUNT - strlen(TODECODE) -1; --i)
    {
        to_be_returned[i] = TODECODE[j];
        j--;
    }

    for (int i = 0; i < DECODEDAMOUNT - strlen(TODECODE); ++i)
    {
        char get_random_char = random_char_generator();

        to_be_returned[i] = get_random_char;
    }

    int temp = strlen(TODECODE);	//Getting rid of the warning.

    *number_of_decoded_chars = temp;
    return to_be_returned;
}

//A method, which performs the error handling print.
void pretty_error_print()
{
    fprintf(stderr, "%c[1m", ESC); //changes text to bold
    fprintf(stderr, "RKP main.c: ");

    fprintf(stderr, "\033[0;31m"); //changes color to red
    fprintf(stderr, "fatal error: ");
    fprintf(stderr, "%c[0m", ESC); //changes text back to normal non-bold text
    fprintf(stderr, "\033[0m"); //changes back to normal color

    fprintf(stderr, "memory allocation failed in 'unwrap()'\n");
    fprintf(stderr, "compilation terminated.\n");
}

/*
The function does the following things:
	Unwraps the char array, which was passed as an argument,
	it also allocates memory for the unwrapped characters.
	Performs error handling (in terms of the memory allocation).
	Copies the hidden characters to the newly allocated adress,
	according to the given rule (first 5 non-important chars,
	last 3 are the decoded ones).
	It also free()-s the original decoded array.
	In the end it returns a pointer which points to the newly
	allocated adress of the unwrapped characters (abc in this case).
*/
char* unwrap(char* test_array, int number_of_decoded_chars)
{
    char* to_be_returned = calloc(number_of_decoded_chars, sizeof(char));

    if (to_be_returned == NULL)
    {
        pretty_error_print();
        exit(1);
    }

    //This could be done with for loop but I tend to overcomplicate things.
    memcpy(to_be_returned,
           test_array + (DECODEDAMOUNT - number_of_decoded_chars),
           number_of_decoded_chars * sizeof(char));	

    free(test_array);

    return to_be_returned;
}

//Same as the error print method, but it was made for the char sequences.
void pretty_char_print(char* array_to_print, int length)
{
    for (int i = 0; i < length; ++i)
    {
        if (i == length - 1)
        {
            printf("%c\n", array_to_print[i]);
        }
        else
        {
            printf("%c, ", array_to_print[i]);
        }
    }
}

/*
The main part of the second exercise:
	It basically tests the functions (5 in this case)
	Very informative for the user:
		Always tells which iteration is tested.
		Shows the original array, and the newly decoded array.
		The output is very pretty!
	In the end it free()-s every adress, which has to be free()-d.
	There are should be no leaks at all, tested with valgrind a couple
	of times.
*/
void testmode()
{
    int number_of_decoded_chars = 0;

    char* decoded_chars;

    char* original;

    for (int i = 0; i < TESTCOUNTER; ++i)
    {
        printf("Test number %d:\n", i + 1);

        decoded_chars = test_array(&number_of_decoded_chars);

        printf("The decoded char sequence:\n");
        pretty_char_print(decoded_chars, DECODEDAMOUNT);

        original = unwrap(decoded_chars, number_of_decoded_chars);

        printf("These were the hidden characters in the above sequence:\n");
        pretty_char_print(original, number_of_decoded_chars);

        if (i != TESTCOUNTER - 1)
        {
        	puts("");
        }

        free(original);
    }
}
