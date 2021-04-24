#ifndef _BMPENCODER_H_
#define _BMPENCODER_H_
#include <stdio.h>

int HandleArgv(int argc, char** argv);

char* Unwrap(char* Pbuff, int NumCh);
char* ReadPixels(int f, int* NumCh);

void PrettyErrorPrint();
void PrettyCharPrint(char* array_to_print, int length);

int IsRegularFile(const char *path);
void PrettyFileErrorChecker(FILE *fp);
int BrowseForOpen();

void error(const char *msg, int status);
int Post(char *neptunID, char *message, int NumCh);

void WhatToDo(int sig);

#endif
