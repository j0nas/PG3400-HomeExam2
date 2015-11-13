#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int main() {
    FILE *fp;
    long lSize;
    char *buffer;

    fp = fopen("lyrics.txt", "rb");
    if (!fp) perror("lyrics.txt"), exit(1);

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    buffer = calloc(1, lSize + 1);
    if (!buffer) fclose(fp), fputs("memory alloc fails", stderr), exit(1);

    if (1 != fread(buffer, lSize, 1, fp))
        fclose(fp), free(buffer), fputs("entire read fails", stderr), exit(1);

    char strStripped[strlen(buffer)];
    int i = 0, c = 0;
    for (; i < strlen(buffer); i++) {
        if (isalpha(buffer[i])) {
            strStripped[c++] = tolower(buffer[i]);
        }
    }

    strStripped[c] = '\0';

    printf(strStripped);
    fclose(fp);
    free(buffer);
}