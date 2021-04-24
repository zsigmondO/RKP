#include "bmpencoder.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <locale.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <omp.h>
#include <signal.h>

#define MY_NEPTUNID "V7DCHS"

int main(int argc, char **argv) {
    int fd = 0;
    int code_from_argvhandler = HandleArgv(argc, argv);

    if (code_from_argvhandler == 111) {
        fd = BrowseForOpen();

    } else if (code_from_argvhandler == 222) {
        fd = open(argv[1], O_RDONLY);

    } else if (code_from_argvhandler != 1337){
        fd = open(argv[code_from_argvhandler], O_RDONLY);
    }

    if (code_from_argvhandler == 1337)
    {
        return 0;
    }

    if (fd < 0) {
        error("Hiba: a fájleíró negatív értékkel tért vissza!\n"
            "(Lehet nem létezik a fájl).", 6);
    }

    signal(SIGALRM, WhatToDo);
    signal(SIGINT, WhatToDo);

    alarm(1);

    int number_of_chars = 0;
    char *raw = ReadPixels(fd, &number_of_chars);
    char *real_text = Unwrap(raw, number_of_chars);

    alarm(0);

    //printf("%s", real_text);

    Post(MY_NEPTUNID, real_text, number_of_chars);

    free(real_text);
    real_text = NULL;
    close(fd);

    return 0;
}
