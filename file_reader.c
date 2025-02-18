#include "file_reader.h"

FILE* file_open(char *fname){

    FILE *fp = fopen(fname, "r");

    return fp;
}


long long int* read_line(FILE *fp){

    // read & split & convert line
    long long int lba, size, dt;
    if (fscanf(fp, "%lld %lld %lld", &lba, &size, &dt) != EOF){

        unsigned long long int *line = malloc(sizeof(long long int) * 3);
        line[0] = lba;
        line[1] = size;
        line[2] = dt;

        return line;
    }
    else
        return NULL;
}
