/**
 * 
 * @copyright Copyright (c) 2024, Kevin Kleiman, All Rights Reserved
 * 
 * This is the kernel for yet another hobbyOS designed and developed by Kevin Kleiman.
 * Feel free to copy, use, edit, etc. anything you see 
 *
 * This was originally designed to try writing a ring0 math library but I soon realized,
 * I should just make a full-blown kernel. It has been a great learning experience and I
 * implore anyone even remotely interested to fork, play around, contribute, whatever
 * you want. 
 *
 * For now, it's pretty barebones and shitty, but hopefully that will change with time.
 * Have fun creating kOS (pronounced "Chaos")
 */

#include <stddef.h>
#include "kernel.h"

size_t 
kstrlen(const char* str) 
{
    // Assume length at zero
    size_t length = 0;

    // Increment length when a character is present
    while(str[length])
        ++length;

    return length;
}

void 
kstrcpy(char* dest, const char* src) 
{
    // Pretty loosey goosey way of doing this but fuck it
    for (size_t i = 0; i < kstrlen(src); ++i) {
        dest[i] = src[i];
    }
}

void 
kstrncpy(char* dest, const char* src, size_t n) 
{
    // Pretty loosey goosey way of doing this but fuck it
    for (size_t i = 0; i < n; ++i) {
        if (src[i] != '\0') dest[i] = src[i];
    }
}

char* 
kstrcat(char* dest, const char* append)
{
    // appends one string to another (concatination)
    char* save = dest;
    size_t i;

	for (; *dest; ++dest);
	while ((*dest++ = *append++)); 

	return save;
}

bool 
kstrcmp(const char* a, const char* b) 
{
    // Cache length of a and b
    size_t a_len = kstrlen(a);
    size_t b_len = kstrlen(b);

    // If lengths don't match, then strings are not the same
    if (a_len != b_len) return false;

    // Loop through each character and compare
    for (size_t i = 0; i < a_len; ++i) {
        if (a[i] == b[i]) continue;
        // Fail check if any are different
        return false;
    }

    return true;
}
bool 
kstrncmp(const char* a, const char* b, const size_t length) 
{
    // Cache length of a and b
    size_t a_len = kstrlen(a);
    size_t b_len = kstrlen(b);

    //// If lengths don't match, then strings are not the same
    //if (a_len != b_len) return false;

    // Loop through each character and compare
    for (size_t i = 0; i < length; ++i) {
        if (a[i] == b[i]) continue;
        // Fail check if any are different
        return false;
    }

    return true;
}
// function like kstrcomp but sees if string a starts with string b and if so, returns everything after b seperated by spaces in a array
bool
kstrstr(const char* a, const char* b) {
    return kstrncmp(a, b, kstrlen(b));
}
// function that takes string a, int b, and c and returns a string that is a substring of a starting at b and ending at c
char*
ksubstr(const char* a, int b, int c) {
    char* res = (char*) kmalloc((c - b + 1) * sizeof(char));
    for (int i = 0; i < (c - b); ++i) {
        res[i] = a[b + i];
    }
    res[c - b] = '\0';
    return res;
}
