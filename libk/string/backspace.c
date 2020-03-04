#include <string.h>

void backspace(char s[]) {
    int len = strlen(s);
    s[len-1] = '\0';
}
