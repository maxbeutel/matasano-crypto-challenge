#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "minunit.h"

#include <hextobase64.h>

static void doTestConversion(char *expectedBase64String, size_t expectedBase64String_len, char *hexString, size_t hexString_len)
{
    char *base64String = NULL;
    int rc = convertHexStringToBase64String(hexString, hexString_len, &base64String);
    assert(rc == 0 && "Failed to convert hex string to base64.");

    assert(expectedBase64String_len == strlen(base64String) && "Conversion failed, base64 string length do not match.");
    assert(strcmp(expectedBase64String, base64String) == 0 && "Conversion failed, base64 string does not match expected value.");

    free(base64String);
}

char *test_conversion()
{
    // f
    doTestConversion("Zg==", strlen("Zg=="), "66", strlen("66"));
    // fo
    doTestConversion("Zm8=", strlen("Zm8="), "666F", strlen("666F"));
    // foo
    doTestConversion("Zm9v", strlen("Zm9v"), "666F6F", strlen("666F6F"));
    // foob
    doTestConversion("Zm9vYg==", strlen("Zm9vYg=="), "666F6F62", strlen("666F6F62"));
    // fooba
    doTestConversion("Zm9vYmE=", strlen("Zm9vYmE="), "666F6F6261", strlen("666F6F6261"));
    // foobar
    doTestConversion("Zm9vYmFy", strlen("Zm9vYmFy"), "666F6F626172", strlen("666F6F626172"));

/*
   BASE64("") = ""
   BASE64("f") = "Zg=="
   BASE64("fo") = "Zm8="
   BASE64("foo") = "Zm9v"
   BASE64("foob") = "Zm9vYg=="
   BASE64("fooba") = "Zm9vYmE="
   BASE64("foobar") = "Zm9vYmFy"

   BASE16("") = ""
   BASE16("f") = "66"
   BASE16("fo") = "666F"
   BASE16("foo") = "666F6F"
   BASE16("foob") = "666F6F62"
   BASE16("fooba") = "666F6F6261"
   BASE16("foobar") = "666F6F626172"

   see http://tools.ietf.org/html/rfc4648#section-5
*/

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_conversion);

    return NULL;
}

RUN_TESTS(all_tests)
