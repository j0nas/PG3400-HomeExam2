#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int d = 2;

struct Codepoint {
    char code[10];
    char pointChar[10];
    int point;
    int isUpper;
    int isAlpha;
} typedef Codepoint;

char *getFileContent(const char *filename) {
    char *source = NULL;
    FILE *fp = fopen(filename, "r");
    if (fp != NULL) {
        /* Go to the end of the file. */
        if (fseek(fp, 0L, SEEK_END) == 0) {
            /* Get the size of the file. */
            long bufsize = ftell(fp);
            if (bufsize == -1) { /* Error */ }

            /* Allocate our buffer to that size. */
            source = malloc(sizeof(char) * (bufsize + 1));

            /* Go back to the start of the file. */
            if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

            /* Read the entire file into memory. */
            size_t newLen = fread(source, sizeof(char), bufsize, fp);
            if (newLen == 0) {
                fputs("Error reading file", stderr);
            } else {
                source[newLen++] = '\0'; /* Just to be safe. */
            }
        }
        fclose(fp);
    } else {
        perror("Unable to read file");
    }

    return source;
}

void readFile() {
    // TODO null checks!
    // TODO free resources
    char *buffer = getFileContent("lyrics.txt");
    if (buffer == NULL) {
        return;
    }

    char strStripped[strlen(buffer)];
    int i = 0, outputLength = 0;
    for (; i < strlen(buffer); i++) {
        if (isalpha(buffer[i])) {
            strStripped[outputLength++] = tolower(buffer[i]);
        }
    }
    strStripped[outputLength] = '\0';
    printf("%s\n", strStripped);

    buffer = getFileContent("inputMessage.txt");
    printf("%s\n", buffer);

    Codepoint points[strlen(buffer)];

    int k = 0, l = 0;
    for (i = 0; i < strlen(buffer); i++) {
        if (isalpha(buffer[i])) {
            for (k = 0; k < outputLength; k++) {
                if (l > 0 && ((abs(points[l - 1].point - k) < d) || (points[l - 1].point + k) < d)) {
                    continue;
                }

                if (tolower(buffer[i]) == strStripped[k]) {
                    points[l].code[0] = tolower(buffer[i]);
                    points[l].isUpper = isupper(buffer[i]);
                    points[l].isAlpha = 1;
                    sprintf(points[l].pointChar, "%d", k);
                    points[l++].point = k;
                    break;
                }
            }

            if (k >= outputLength) {
                printf("Unable to encode using the current key!\n"
                               "No matching position found for char '%c'\n", buffer[i]);
                return;
            }
        } else {
            char c[10] = {buffer[i]};
            strcpy(points[l].pointChar, c);
            points[l].isUpper = 0;
            points[l].isAlpha = 0;
            points[l++].point = -1;
        }
    }

    int j = 0;
    for (; j < l; j++) {
        printf(points[j].isAlpha ? (points[j].isUpper ? "[-%s]" : "[%s]") : "%s", points[j].pointChar);
    }
    printf("\n");

    free(buffer);
}

int main() {
    readFile();
}
