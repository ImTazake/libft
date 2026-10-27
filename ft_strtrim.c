#include "libft.h"
/* #include <stdlib.h> */

static int	ft_is_in_set(char c, char const *set)
{
	size_t	i;

	i = 0;
	while (set[i])
	{
		if (set[i] == c)
			return (1);
		i++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*trimmed;
	size_t	start;
	size_t	end;

	if (!s1 || !set)
		return (NULL);
	start = 0;
	end = ft_strlen(s1);
	while (s1[start] && ft_is_in_set(s1[start], set))
		start++;
	while (end > start && ft_is_in_set(s1[end - 1], set))
		end--;
	trimmed = ft_substr(s1, start, end - start);
	return (trimmed);
}