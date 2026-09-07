#include <stddef.h>

void *ft_memset(void *ptr, int value, size_t size)
{
    int    *ref;

    ref = ptr;
    while (size--)
    {
        *ref = value;
        ref++;
    }
    return (ptr);
}