#include "string.h"

size_t strlen(const char *s)
{
    size_t len = 0;
    while (*s++)
        len++;
    return len;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    while (n-- && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return n == (size_t)-1 ? 0 : *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char *strncpy(char *dest, const char *src, size_t n)
{
    size_t i;
    for (i = 0; i < n && src[i]; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = '\0';
    return dest;
}
