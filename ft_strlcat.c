/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 07:22:41 by tazake            #+#    #+#             */
/*   Updated: 2026/10/28 04:39:59 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (i < size && dst[i])
		i++;
	while (src[j] && i + j + 1 < size)
	{
		dst[i + j] = src[j];
		j++;
	}
	if (i < size)
		dst[i + j] = '\0';
	while (src[j])
		j++;
	return (i + j);
}

/* 
#include <stdio.h>
#include <bsd/string.h>

int	main(void)
{
	char	*str_start = "Sphinx of ";
	char	*str_end = "black quartz, judge my vow";
	int		len = strlen(str_start) + strlen(str_end) + 1;
	char	*dst1 = calloc(1, len);
	char	*dst2 = calloc(1, len);
	int ret1;	
	int ret2;	

	strcpy(dst1, str_start);
	strcpy(dst2, str_start);

	ret1 = ft_strlcat(dst1, NULL, len);
	ret2 = strlcat(dst2, NULL, len);

	printf("ft ret/dst:  [%d] '%s'\n", ret1, dst1);
	printf("std ret/dst: [%d] '%s'\n", ret2, dst2);
	return (0);
}
 */
