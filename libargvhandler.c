#include "argvhandler.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define ENVFILENAME "sample.env"
#define MAXLINESIZE 512
#define ESC 27

int handle_argv(int argc, char **argv) {

    int index_of_picture = 1337;

    if (argc == 1) {
        printf("%c[1m", ESC); //changes text to bold
        printf("RKP main.c: ");
        printf("%c[0m", ESC); //changes text back to normal non-bold text

        printf("program starting.\n");
        printf("Broswer was called.\n");

        return (111);

    } else if (argc == 2 && argv[1][0] != '-') {
        return (222); //file as input. but can be NULL!!

    } else {

        int version_was_printed_before = 0;

        int help_was_printed_before = 0;

        int only_one_picture_is_allowed = 0;


        for (int i = 1; i < argc; ++i) {

            if (argv[i][0] == '-') {
                if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-version") == 0) {
                    if (!version_was_printed_before) {

                        version_was_printed_before = 1;

                        int saved_errno = 0;

                        FILE *fp = fopen(ENVFILENAME, "r");

                        saved_errno = errno;

                        if (!fp) {

                            printf("%c[1m", ESC); //changes text to bold
                            printf("RKP main.c: ");

                            printf("\033[0;31m"); //changes color to red
                            printf("fatal error: ");
                            printf("%c[0m", ESC); //changes text back to normal non-bold text
                            printf("\033[0m"); //changes back to normal color

                            printf("failed opening the .env file, error code: %d\n", saved_errno);
                            printf("compilation terminated.\n");

                            exit(1);

                        }

                        char line_buffer[MAXLINESIZE] = {0};

                        line_buffer[strlen(line_buffer) - 3] = '\0';

                        while (fgets(line_buffer, MAXLINESIZE, fp)) {
                            char *token = strtok(line_buffer, "=");
                            token = strtok(NULL, "=");

                            printf("%s", token);
                            token = strtok(NULL, "=");
                        }

                        puts("");

                        fclose(fp);

                    }
                } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-help") == 0) {
                    if (!help_was_printed_before) {

                        help_was_printed_before = 1;

                        printf("Program manual:\n");
                        printf("%c[1m", ESC); //changes text to bold
                        printf("RKP tips:\n");
                        printf("%c[0m", ESC); //changes text back to normal non-bold text

                        printf("Use the --version command line option "
                               "for getting detailed information about the program.\n\n");

                        printf("Give a TrueColor bmp image file as an argument for decoding.\n");
                        
                    }
                } else {
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
            } else {
                if (!only_one_picture_is_allowed) {

                    only_one_picture_is_allowed = 1;

                    if (access(argv[i], F_OK) != 0 || access(argv[i], R_OK) != 0) {
                        printf("%c[1m", ESC); //changes text to bold
                        printf("RKP main.c: ");

                        printf("\033[0;31m"); //changes color to red
                        printf("fatal error: ");
                        printf("%c[0m", ESC); //changes text back to normal non-bold text
                        printf("\033[0m"); //changes back to normal color

                        printf("input file '%s' does not exists,\n"
                               "or no execute/read permission is given\n", argv[i]);
                        printf("compilation terminated.\n");

                        exit(1);

                    } else {
                        printf("%c[1m", ESC); //changes text to bold
                        printf("RKP main.c: ");

                        printf("\033[0;32m"); //changes color to green
                        printf("%c[0m", ESC); //changes text back to normal non-bold text
                        printf("\033[0m"); //changes back to normal color

                        printf("file '%s' was found successfully!\n", argv[i]);

                        index_of_picture = i;

                    }
                }
            }
        }
    }

    return index_of_picture;
}
