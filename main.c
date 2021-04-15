#include "argvhandler.h"
#include "secondexercise.h"
#include "noargumentbroswerhandler.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
    
    FILE *fp = NULL;

    int code_from_argvhandler = handle_argv(argc, argv);

    if (code_from_argvhandler == 111) {
        fp = broswe_for_open();

    } else if (code_from_argvhandler == 222) {
        fp = fopen(argv[1], "r");

    } else if (code_from_argvhandler != 1337) {
        fp = fopen(argv[code_from_argvhandler], "r");

    }

    if (fp != NULL) {
        char c;

        while ((c = fgetc(fp)) != EOF) {
            printf("%c", c);
        }

        fclose(fp);
    }

    return 0;
}