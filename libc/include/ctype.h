#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

// Is alphanumeric
int isalnum(int c);

// Is alphabetic
int isalpha(int c);

// Is blank
int isblank(int c);

// Is control character
int iscntrl(int c);

// Is digit
int isdigit(int c);

// Has graphical representation
int isgraph(int c);

// Is lowercase
int islower(int c);

// Is a printable character
int isprint(int c);

// Is a punctuation character
int ispunct(int c);

// Is space
int isspace(int c);

// Is uppercase
int isupper(int c);

// Is a hexidecimal digit
int isxdigit(int c);

// Switch an uppercase letter to a lowercase letter
int tolower(int c);

// Switch a lowercase letter to an uppercase letter
int toupper(int c);

#ifdef __cplusplus
}
#endif

#endif
