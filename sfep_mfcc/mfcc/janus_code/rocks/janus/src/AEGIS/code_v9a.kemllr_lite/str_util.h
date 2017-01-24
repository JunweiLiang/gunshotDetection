#ifndef STR_UTIL_H
#define STR_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include <string>

#define STRMAX 1048576

using namespace std;

int extractField(const char* input, int field_id, char delimiter[], char field[]);
int numField(char* input, char delimiter[]);
int match(char* input, char* reg_expr);
int chomp(char* input);

string int2String(int num);

#endif
