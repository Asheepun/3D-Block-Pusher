#ifndef FILES_H_
#define FILES_H_

#include "engine/strings.h"

typedef char FileLine[STRING_SIZE];

char *getFileData_mustFree(const char *, long int *);

FileLine *getFileLines_mustFree(const char *, int *);

void writeDataToFile(const char *, char *, long int);

#endif
