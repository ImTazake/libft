/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 07:22:25 by tazake            #+#    #+#             */
/*   Updated: 2026/10/27 22:21:25 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t n)
{
	unsigned char		*d;
	const unsigned char	*s;
	size_t				i;

	d = (unsigned char *)dst;
	s = (const unsigned char *)src;
	if (d < s)
	{
		i = 0;
		while (i < n)
		{
			d[i] = s[i];
			i++;
		}
	}
	else if (d > s)
	{
		i = n;
		while (i > 0)
		{
			i--;
			d[i] = s[i];
		}
	}
	return (dst);
}

/* #include <stdio.h>

int	main(int argc, char *argv[])
{
	int		len;
	char	*src;
	char	*dest_ft;
	char	*dest_std;

	if (argc != 2)
		return (1);

	len = strlen(argv[1]) + 1;
	dest_ft = malloc(len);
	// dest_ft = NULL;
	dest_std = malloc(len);
	// dest_std = NULL;
	src = strdup(argv[1]);
	// printf("dest_ft: %p\n", dest_ft);
	// printf("dest_std: %p\n", dest_std);
	// printf("src: %p\n", src);

	memmove(dest_std, src, 0);
	ft_memmove(dest_ft, src, 0);
	printf("dest_ft:  [%s]\n", dest_ft);
	printf("dest_std: [%s]\n", dest_std);
	return (0);
}
 */