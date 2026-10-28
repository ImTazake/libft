/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 07:23:01 by tisilva           #+#    #+#             */
/*   Updated: 2026/10/28 10:07:34 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && s1[i] && s1[i] == s2[i])
		i++;
	if (i == n)
		return (0);
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

/* 
#include <stdio.h>
#include <bsd/string.h>

int	main(void)
{
	char	*s1 = "Sphinx of";
	char	*s2 = "Sphinx of black quartz";
	int 	ret1;
	int 	ret2;

	ret2 = strncmp(s1, s2, strlen(s1));
	ret1 = ft_strncmp(s1, s2, strlen(s1));

	printf("ft ret:  %d\n", ret1);
	printf("std ret: %d\n", ret2);
	return (0);
}
 */
