#ifndef _NOARGUMENTBROSWERHANDLER_H_
#define _NOARGUMENTBROSWERHANDLER_H_
#include <stdio.h>

int is_regular_file(const char *path);
void pretty_file_error_checker(FILE *fp);
FILE *broswe_for_open();

#endif