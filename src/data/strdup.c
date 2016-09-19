#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strdup.h"

char *ic_strdup(char *ch) {
    char *buffer = 0;
    size_t len = 0;

    if (!ch) {
        puts("ic_strdup: ch was null");
        return 0;
    }

    len = strlen(ch);

    buffer = calloc(len + 1, sizeof(char));
    if (!buffer) {
        puts("ic_strdup: call to calloc failed");
        return 0;
    }

    strncpy(buffer, ch, len);

    buffer[len] = '\0';

    return buffer;
}
