#ifndef STRINGS_H_
#define STRINGS_H_

#define STRING_SIZE 256
#define LARGE_STRING_SIZE 1024
#define SMALL_STRING_SIZE 32

void String_set(char *, const char *, int);

void String_append(char *, char *);

void String_append_int(char *, int);

void String_append_float(char *, float);

void String_clearRange(char *, int, int);

#endif
