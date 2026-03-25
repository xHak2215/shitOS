#include <stdio.h>
#include <string.h>

int split_symbol(const char *str, char split_char, char out[][100]) {
    int part = 0;
    int pos = 0;
    int len = strlen(str);
    int max_size = sizeof(*out);

    if (max_size <= 0) return 0;

    out[part][0] = '\0';

    for (int i = 0; i < len; ++i) {
        if (str[i] == split_char) {
            out[part][pos] = '\0';
            part++;
            if (part >= max_size) return part;
            pos = 0;
            out[part][0] = '\0';
        } else {
            if (pos < 99) { // предотвратить переполнение буфера
                out[part][pos++] = str[i];
            }
        }
    }
    out[part][pos] = '\0';
    return part + 1;
}

int main(void) {
    char parts[10][100];
    int n = split_symbol("ab+cd+ef", '+', parts);
    printf("%d\n", n);
    for (int i = 0; i < n; ++i) {
        printf("part %d: %s\n", i, parts[i]);
    }

    printf(strstr("123a456", "a"));
    return 0;
}

