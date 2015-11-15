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

void readFile() {
    FILE *fp;
    long lSize;
    char *buffer;

    fp = fopen("lyrics.txt", "rb");
    if (!fp) perror("Unable to open key file"), exit(1);

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    buffer = calloc(1, lSize + 1);
    if (!buffer) fclose(fp), fputs("Unable to allocate memory", stderr), exit(1);

    if (1 != fread(buffer, lSize, 1, fp))
        fclose(fp), free(buffer), fputs("Unable to read key file", stderr), exit(1);

    char strStripped[strlen(buffer)];
    int i = 0, outputLength = 0;
    for (; i < strlen(buffer); i++) {
        if (isalpha(buffer[i])) {
            strStripped[outputLength++] = tolower(buffer[i]);
        }
    }
    strStripped[outputLength] = '\0';

    printf(strStripped);
    fclose(fp);
    free(buffer);

    printf("\n\n");

    fp = fopen("inputMessage.txt", "rb");
    if (!fp) perror("Unable to open message file"), exit(1);

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    buffer = calloc(1, lSize + 1);
    if (!buffer) fclose(fp), fputs("Unable to allocate memory", stderr), exit(1);

    if (1 != fread(buffer, lSize, 1, fp))
        fclose(fp), free(buffer), fputs("Unable to read message file", stderr), exit(1);

    printf(buffer);
    printf("\n\n");

    //char encodedMessage[strlen(buffer) * 5]; // worst-case
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
                    /*
                    char intStr[8];
                    sprintf(intStr, isupper(buffer[i]) ? "-%d" : "%d", k);
                    strcat(encodedMessage, "[");
                    strcat(encodedMessage, intStr);
                    strcat(encodedMessage, "]");
                    break;
                     */
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

            points[l].code[0] = buffer[i];
            points[l].isUpper = 0;
            points[l].isAlpha = 0;
            points[l++].point = -1;
            //char c[10] = {buffer[i]};
            //strcat(encodedMessage, c);
        }
    }
    //encodedMessage[l] = '\0';
    //printf(encodedMessage);

    int j = 0;
    for (; j < l; j++) {
        printf(points[j].isAlpha ? (points[j].isUpper ? "[-%s]" : "[%s]") : "%s", points[j].pointChar);
    }

    fclose(fp);
    free(buffer);
}

int main() {
    readFile();
}
