#include <stddef.h>

char	*ft_strnstr(const char *str, const char *to_find, size_t len)
{
	size_t i;
	size_t j;

	if (to_find == '\0')
		return ((char *)str);
	i = 0;
	while (str[i] && i < len)
	{
		j = 0;
		while (str[i + j] == to_find[j] && to_find[j] && i + j < len)
			j++;
		if(to_find[j] == '\0')
			return((char *)&str[i]);
		i++;
	}
	return (NULL);
}