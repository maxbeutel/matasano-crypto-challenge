#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <assert.h>

#define UNUSED(x) (void)(x)

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
    "0000",     "0001",     "0010",     "0011",     "0100",     "0101",     "0110",     "0111",
    "1000",     "1001",     "1010",     "1011",     "1100",     "1101",     "1110", "1111",
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


static int findIndexInToBinaryTable(char *binaryTable, int binaryTable_len, size_t binaryTableEntry_len, char *needle, size_t needle_len)
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

int main(void)
{
    char *expectedBase64String = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";
    char *hexString = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d"/*"2FA"*/;
    size_t hexString_len = strlen(hexString);

    char hexStringInBinary[1024] = { 0 }; // @FIXME size?! (watch null terminated)
    /* size_t hexStringInBinary_len = 64; */

    int bytesAdded = 0;

    for (size_t i = 0; i < hexString_len; i++) {
        /* printf("Currently at %c\n", toupper(hexString[i])); */
        char *pos = strchr(hexChars, toupper(hexString[i]));
        assert(pos != NULL && "Failed to find input char in hex chars, input failure.");

        /* printf("found %c at %ld binary is %s\n", hexString[i], pos - hexChars, hexCharsToBinaryTable[pos - hexChars]); */

        bytesAdded += sprintf(hexStringInBinary + bytesAdded, "%s", hexCharsToBinaryTable[pos - hexChars]);
    }

    assert((size_t) (bytesAdded / 4) == hexString_len && "Failed to create binary string from hexstring.");

    /* printf("hexStringInBinary => %s (strlen: %lu)\n", hexStringInBinary, strlen(hexStringInBinary)); */


    int sextetsCount = ceil(strlen(hexStringInBinary) / 6);
    char base64String[200] = { 0 }; // @FIXME whats the correct size to be allocated?

    bytesAdded = 0;

    // @TODO padding!
    for (int i = 0; i < sextetsCount; i++) {
        char currentSextet[7] = { 0 };
        sprintf(currentSextet, "%.*s", 6, hexStringInBinary + (i * 6));

        int base64CharsIndex = findIndexInToBinaryTable(base64CharsToBinaryTable[0], 64, 6, currentSextet, strlen(currentSextet));
        assert(base64CharsIndex != -1);

        /* printf( */
        /*     "current sextet: %s, index in base64 chars: %d, base 64 char: %c\n", */
        /*     currentSextet, */
        /*     base64CharsIndex, */
        /*     base64Chars[base64CharsIndex] */
        /* ) ;*/

        bytesAdded += sprintf(base64String + bytesAdded, "%c", base64Chars[base64CharsIndex]);
    }

    assert(bytesAdded == sextetsCount && "Failed to create base64 string from parsing hex binary string sextets.");

    printf("base64String => \"%s\" (strlen %lu)\n", base64String, strlen(base64String));
    assert(strcmp(expectedBase64String, base64String) == 0 && "Conversion failed, base64 string does not match expected value.");

    return 0;
}
