#include <string.h>
#include <ctype.h>

#include <assert.h>

#include "hextobase64.h"

// debug
#include <stdio.h>

char hexChars[16] = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'A',
    'B',
    'C',
    'D',
    'E',
    'F',
};

char hexCharsToBinaryTable[16][5] = {
    "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
    "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111",
};

char base64Chars[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/',
};

char base64CharsToBinaryTable[64][7] = {
    "000000", "000001", "000010", "000011", "000100", "000101", "000110", "000111",
    "001000", "001001", "001010", "001011", "001100", "001101", "001110", "001111",
    "010000", "010001", "010010", "010011", "010100", "010101", "010110", "010111",
    "011000", "011001", "011010", "011011", "011100", "011101", "011110", "011111",
    "100000", "100001", "100010", "100011", "100100", "100101", "100110", "100111",
    "101000", "101001", "101010", "101011", "101100", "101101", "101110", "101111",
    "110000", "110001", "110010", "110011", "110100", "110101", "110110", "110111",
    "111000", "111001", "111010", "111011", "111100", "111101", "111110", "111111",
};


static int findIndexInToBinaryTable(
    const char *binaryTable,
    int binaryTable_len,
    size_t binaryTableEntry_len,
    const char *needle,
    size_t needle_len
)
{
    assert(binaryTableEntry_len == needle_len);

    for (int i = 0; i < binaryTable_len; i++) {
        if (strcmp(binaryTable, needle) == 0) {
            return i;
        }

        // @TODO break here if last?! see http://stackoverflow.com/questions/25303647/accesing-a-2d-array-using-a-single-pointer

        // assuming table consists of \0 terminated strings
        binaryTable += (binaryTableEntry_len + 1);
    }

    return -1;
}

static int convertHexStringToBinaryString(const char *hexString, size_t hexString_len, char **out)
{
    assert(hexString != NULL);

    if (hexString_len == 0) {
        return -1;
    }

    char *hexStringInBinary = calloc((4 * hexString_len) + 1, sizeof(char));

    if (hexStringInBinary == NULL) {
        return -1;
    }

    int bytesAdded = 0;

    for (size_t i = 0; i < hexString_len; i++) {
        char *pos = strchr(hexChars, toupper(hexString[i]));
        assert(pos != NULL && "Failed to find input char in hex chars, input failure.");

        /* printf("hexStringInBinary '%s', len %lu\n", hexStringInBinary, strlen(hexStringInBinary)); */

        bytesAdded += sprintf(hexStringInBinary + bytesAdded, "%s", hexCharsToBinaryTable[pos - hexChars]);
    }

    /* printf("hexStringInBinary '%s', len %lu\n", hexStringInBinary, strlen(hexStringInBinary)); */

    assert((size_t) (bytesAdded / 4) == hexString_len && "Failed to create binary string from hexstring.");
    *out = hexStringInBinary;

    return 0;
}

static int convertBinaryStringToBase64String(const char *binaryString, size_t binaryString_len, char **out)
{
    assert(binaryString != NULL);

    if (binaryString_len == 0) {
        return -1;
    }

    int sextetsCount = (binaryString_len + (6 - 1)) / 6; // round up
    /* printf("sextets count %d of string len %lu\n", sextetsCount, binaryString_len); */
    char *base64String = calloc(sextetsCount + 1, sizeof(char));

    if (base64String == NULL) {
        return -1;
    }

    int bytesAdded = 0;
    int paddingCharsAdded = 0;

    for (int i = 0; i < sextetsCount; i++) {
        size_t remainingBinaryStringLength = binaryString_len - (i * 6);

        if (remainingBinaryStringLength < 6) {
            /* printf("less then one sextet left, needs padding\n"); */

            char currentSextet[7] = { 0 };
            sprintf(currentSextet, "%.*s", (int) remainingBinaryStringLength, binaryString + (i * 6));

            /* printf("current sextet before appending 0s => %s\n", currentSextet); */

            for (int j = remainingBinaryStringLength; j < 6; j++) {
                if (currentSextet[j] == '\0') {
                    currentSextet[j] = '0';
                }
            }

            /* printf("current sextet after appending 0s => %s\n", currentSextet); */


            // binary string is now padded, we look up this one in the table
            int base64CharsIndex = findIndexInToBinaryTable(base64CharsToBinaryTable[0], 64, 6, currentSextet, strlen(currentSextet));
            assert(base64CharsIndex != -1 && "base64 char not found in binary table.");

            /* printf("found char with padding: %c\n", base64Chars[base64CharsIndex]); */

            // add this one char found based on 0-padded binary string
            bytesAdded += sprintf(base64String + bytesAdded, "%c", base64Chars[base64CharsIndex]);

            /* printf("need to fill up %lu times\n", (6 - remainingBinaryStringLength) / 2); */

            // now fill up all remaining bytes with base64 padding char =
            for (size_t j = 0; j < (6 - remainingBinaryStringLength) / 2; j++) {
                /* printf("filling up\n"); */
                paddingCharsAdded += sprintf(base64String + bytesAdded + paddingCharsAdded, "%c", '=');
            }
        } else {
            /* printf("current sextet: %.*s, bytes added so far %d, binary chars left %lu\n", 6, binaryString + (i * 6), bytesAdded, remainingBinaryStringLength); */

            char currentSextet[7] = { 0 };
            sprintf(currentSextet, "%.*s", 6, binaryString + (i * 6));

            int base64CharsIndex = findIndexInToBinaryTable(base64CharsToBinaryTable[0], 64, 6, currentSextet, strlen(currentSextet));
            assert(base64CharsIndex != -1 && "base64 char not found in binary table.");

            /* printf("found char NO padding: %c\n", base64Chars[base64CharsIndex]); */

            bytesAdded += sprintf(base64String + bytesAdded, "%c", base64Chars[base64CharsIndex]);
        }
    }

    assert(bytesAdded == sextetsCount && "Failed to create base64 string from parsing binary string sextets.");
    *out = base64String;

    return 0;
}

static int isValidHexString(const char *hexString)
{
    return hexString[strspn(hexString, "0123456789abcdefABCDEF")] == 0;
}

int convertHexStringToBase64String(const char *hexString, size_t hexString_len, char **out)
{
    assert(hexString != NULL);

    if (hexString_len == 0) {
        return -1;
    }

    if (!isValidHexString(hexString)) {
        return -1;
    }

    char *hexStringInBinary = NULL;
    char *base64String = NULL;

    int rc = convertHexStringToBinaryString(hexString, hexString_len, &hexStringInBinary);

    if (rc != 0) {
        goto error;
    }

    rc = convertBinaryStringToBase64String(hexStringInBinary, strlen(hexStringInBinary), &base64String);

    if (rc != 0) {
        goto error;
    }

    assert(base64String != NULL);
    *out = base64String;

    free(hexStringInBinary);

    return 0;

error:
    free(hexStringInBinary);
    free(base64String);

    return -1;
}
