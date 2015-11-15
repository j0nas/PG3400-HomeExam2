#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

int d = 2; // distance
#define ACCEPTED_ACCURACY 90 // % of words correct required when cracking

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
        if (fseek(fp, 0L, SEEK_END) == 0) {
            long bufsize = ftell(fp);
            if (bufsize == -1) { }

            source = malloc(sizeof(char) * (bufsize + 1));
            if (source == NULL) {
                perror("Unable to allocate memory");
                return source;
            }

            if (fseek(fp, 0L, SEEK_SET) != 0) {
                return NULL;
            }
            size_t newLen = fread(source, sizeof(char), bufsize, fp);
            if (newLen == 0) {
                fputs("Error reading file", stderr);
            } else {
                source[newLen++] = '\0';
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
    if (encodedMessage == NULL) {
        return;
    }

    char *strStripped = getCompressedKey("lyrics.txt");
    if (strStripped == NULL) {
        return;
    }

    char resStr[strlen(encodedMessage)];
    const char s[2] = "[";
    char *token = strtok(encodedMessage, s);
    char *ptr;
    int resStrLen = 0, strippedPos = 0, strippedLength = strlen(strStripped);
    while (token != NULL) {
        strippedPos = abs(strtol(token, &ptr, 10));
        if (strippedPos > strippedLength) {
            printf("Position of parsed char is outside bounds of compressed str: %d\n", strippedPos);
            return;
        }
        resStr[resStrLen++] = strStripped[strippedPos];
        // If char is marked as uppercase with '-' prefix..
        if ((token[0] == '-' ? 1 : 0)) {
            resStr[resStrLen - 1] = toupper(resStr[resStrLen - 1]);
        }

        if (ptr[strlen(ptr) - 1] != ']') {
            if (strlen(ptr) > 1 && ptr[strlen(ptr) - 2] != ']') {
                resStr[resStrLen++] = ptr[strlen(ptr) - 2];
            }
            resStr[resStrLen++] = ptr[strlen(ptr) - 1];
        }

        token = strtok(NULL, s);
    }
    resStr[resStrLen] = '\0';
    printf("%s\n", resStr);


    free(encodedMessage);
    free(strStripped);
}


void crack() {
    printf("Cracking, please standby..");
    char *encodedMessage = getFileContent("encodedText.txt");
    if (encodedMessage == NULL) {
        return;
    }

    int res[1000][1000]; // [word #][char in word #]
    int wordCount = 0, charCount = 0;

    const char s[2] = "[";
    char *token = strtok(encodedMessage, s);
    char *ptr;
    while (token != NULL) {
        res[wordCount][charCount++] = abs(strtol(token, &ptr, 10));
        if (res[wordCount][charCount - 1] == 0) {
            res[wordCount][charCount - 1] = -1;
        }

        if (ptr[strlen(ptr) - 1] != ']') {
            if (strlen(ptr) > 1 && !isdigit(ptr[strlen(ptr) - 2])) {
                charCount = 0;
                wordCount++;
            }
        }

        token = strtok(NULL, s);
    }

    FILE *file;
    char line[BUFSIZ];

    if ((file = fopen("/usr/share/dict/words", "r")) == NULL) {
        fprintf(stderr, "cannot open %s\n", "/usr/share/dict/words");
        return;
    }

    DIR *d = opendir("./songLibrary");
    struct dirent *dir;
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                char basePath[100];
                strcpy(basePath, "./songLibrary/");

                char fileName[100];
                strcpy(fileName, dir->d_name);
                strcat(basePath, fileName);
                char *strStripped = getCompressedKey(basePath);
                if (strStripped == NULL) {
                    return;
                }

                int wordsMatched = 0;
                int i = 0;
                for (; i < wordCount; i++) {
                    int j = 0;
                    char word[100];
                    while (res[i][j] != 0) { // for each char
                        word[j] = strStripped[res[i][j] == -1 ? 0 : res[i][j]];
                        j++;
                    }
                    word[j] = '\0';
                    rewind(file);
                    while (!feof(file)) {
                        char newStr[100];
                        fgets(line, sizeof(line), file);
                        int h = 0, c = 0;
                        for (; h < strlen(line); h++) {
                            if (isalnum(line[h])) {
                                newStr[c++] = line[h];
                            }
                        }
                        newStr[c] = '\0';

                        if (strcmp(newStr, word) == 0) {
                            wordsMatched++;
                            break;
                        }
                    }
                }

                if (((wordsMatched * 100) / wordCount) > ACCEPTED_ACCURACY) {
                    printf("FOUND MATCHING SONG: %s\n", dir->d_name);
                    break;
                }
            }
        }

        closedir(d);
    }

    fclose(file);
    free(encodedMessage);
}

int main() {
    encodeMessage();
    decodeMessage();
    crack();
}
