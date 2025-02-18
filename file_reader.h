#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100

FILE* file_open(char *fname);
long long int* read_line(FILE *fp);