#include <string.h>

size_t strlen(const char *str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

int streq(const char *a, const char *b)
{
	size_t alen = strlen(a);
	size_t blen = strlen(b);
	if (alen != blen)
		return 0;

	return memcmp(a, b, alen) != 0;
}