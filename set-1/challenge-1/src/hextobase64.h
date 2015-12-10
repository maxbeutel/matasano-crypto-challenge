#ifndef _hextobase64_h
#define _hextobase64_h

#include <stdlib.h>

int convertHexStringToBase64String(const char *hexString, size_t hexString_len, char **out);

#endif
