/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 07:22:53 by tazake            #+#    #+#             */
/*   Updated: 2026/10/28 04:40:19 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	i;

	i = 0;
	if (size > 0)
	{
		while (src[i] && i < size - 1)
		{
			dst[i] = src[i];
			i++;
		}
		dst[i] = '\0';
	}
	return (ft_strlen(src));
}

/* 
#include <stdio.h>
#include <bsd/string.h>

int	main(void)
{
	char	*str_start = "Sphinx of black quartz, judge my vow\0abcdef";
	#ifdef LEN
	int		len = LEN;
	#else
	int		len = strlen(str_start) + 1;
	#endif
	char	*dst1 = calloc(1, len);
	char	*dst2 = calloc(1, len);
	int ret1;
	int ret2;

	ret1 = strlcpy(dst1, str_start, len);
	ret2 = ft_strlcpy(dst2, str_start, len);

	printf("std ret/dst: [%d] '%s'\n", ret1, dst1);
	printf("ft ret/dst:  [%d] '%s'\n", ret2, dst2);
	return (0);
} */
