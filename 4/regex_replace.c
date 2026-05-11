#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <regex> <text> <replacement>\n", argv[0]);
        return 1;
    }

    const char *pattern     = argv[1];
    const char *text        = argv[2];
    const char *replacement = argv[3];

    regex_t regex;
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        fprintf(stderr, "Failed to compile regex: %s\n", pattern);
        return 1;
    }

    size_t buf_size = strlen(text) * 2 + strlen(replacement) + 1;
    char *buf = malloc(buf_size);
    if (!buf) {
        fprintf(stderr, "Memory allocation failed\n");
        regfree(&regex);
        return 1;
    }
    buf[0] = '\0';

    const char *cursor = text;
    size_t buf_used = 0;
    regmatch_t match;

    while (regexec(&regex, cursor, 1, &match, 0) == 0) {
        size_t prefix_len  = match.rm_so;
        size_t replace_len = strlen(replacement);
        size_t needed      = buf_used + prefix_len + replace_len + 1;

        if (needed > buf_size) {
            buf_size = needed * 2;
            char *new_buf = realloc(buf, buf_size);
            if (!new_buf) {
                fprintf(stderr, "Memory reallocation failed\n");
                free(buf);
                regfree(&regex);
                return 1;
            }
            buf = new_buf;
        }

        memcpy(buf + buf_used, cursor, prefix_len);
        buf_used += prefix_len;

        memcpy(buf + buf_used, replacement, replace_len);
        buf_used += replace_len;

        cursor += match.rm_eo;

        if (match.rm_so == match.rm_eo) {
            if (*cursor == '\0') break;
            buf[buf_used++] = *cursor++;
        }
    }


    size_t tail_len = strlen(cursor);
    size_t needed   = buf_used + tail_len + 1;
    if (needed > buf_size) {
        buf_size = needed;
        char *new_buf = realloc(buf, buf_size);
        if (!new_buf) {
            fprintf(stderr, "Memory reallocation failed\n");
            free(buf);
            regfree(&regex);
            return 1;
        }
        buf = new_buf;
    }
    memcpy(buf + buf_used, cursor, tail_len);
    buf_used += tail_len;
    buf[buf_used] = '\0';

    printf("%s\n", buf);

    free(buf);
    regfree(&regex);
    return 0;
}
