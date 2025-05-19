#include <stdio.h>

void emit_line(const char* line) {
    FILE* out = fopen("main.c", "a");
    if (out) {
        fprintf(out, "%s\n", line);
        fclose(out);
    }
}
