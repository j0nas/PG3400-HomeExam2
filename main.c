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
            if (source == NULL) {
                perror("Unable to allocate memory");
                return source;
            }

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

int appendToFile(const char *filename, Codepoint point) {
    FILE *f = fopen(filename, "a");
    if (f == NULL) {
        perror("Unable to write to file");
        return 0;
    }

    fprintf(f, point.isAlpha ? (point.isUpper ? "[-%s]" : "[%s]") : "%s", point.pointChar);
    printf(point.isAlpha ? (point.isUpper ? "[-%s]" : "[%s]") : "%s", point.pointChar);
    fclose(f);
}

char *getCompressedKey(const char *filename) {
    char *buffer = getFileContent(filename);
    if (buffer == NULL) {
        return NULL;
    }

    char *compressedStr = malloc(sizeof(char) * strlen(buffer));
    if (compressedStr == NULL) {
        perror("Unable to allocate memory");
        return NULL;
    }

    //char strStripped[strlen(buffer)];
    int i = 0, outputLength = 0;
    for (; i < strlen(buffer); i++) {
        if (isalpha(buffer[i])) {
            compressedStr[outputLength++] = tolower(buffer[i]);
        }
    }
    compressedStr[outputLength] = '\0';
    return compressedStr;
}

void encodeMessage() {
    // TODO null checks!
    // TODO free resources
    char *strStripped = getCompressedKey("lyrics.txt");
    if (strStripped == NULL) {
        return;
    }
    printf("%s\n", strStripped);

    char *buffer = getFileContent("inputMessage.txt");
    if (buffer == NULL) {
        return;
    }
    printf("%s\n", buffer);

    Codepoint points[strlen(buffer)];
    int k = 0, l = 0, i = 0, isWithinDist = 0;
    for (i = 0; i < strlen(buffer); i++) {
        if (isalpha(buffer[i])) {
            for (k = 0; k < strlen(strStripped); k++) {
                if (l > 0 && ((abs(points[l - 1].point - k) < d) || (points[l - 1].point + k) < d)) {
                    isWithinDist = 1;
                    continue;
                }

                isWithinDist = 0;
                if (tolower(buffer[i]) == strStripped[k]) {
                    points[l].code[0] = tolower(buffer[i]);
                    points[l].isUpper = isupper(buffer[i]);
                    points[l].isAlpha = 1;
                    sprintf(points[l].pointChar, "%d", k);
                    points[l++].point = k;
                    break;
                }
            }

            if (isWithinDist) {
                printf("Unable to satisfy the d condition!\n");
                return;
            }

            if (k >= strlen(strStripped)) {
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
    remove("encodedText.txt");
    for (; j < l; j++) {
        appendToFile("encodedText.txt", points[j]);
    }
    printf("\n");

    free(buffer);
}

void decodeMessage() {
    char *encodedMessage = getFileContent("encodedText.txt");

    free(encodedMessage);
}

int main() {
    encodeMessage();
    decodeMessage();
}
