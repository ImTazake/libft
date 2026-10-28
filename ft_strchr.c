/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 07:22:35 by tisilva           #+#    #+#             */
/*   Updated: 2026/10/28 13:41:29 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	while (*s)
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if (*s == (char)c)
		return ((char *)s);
	return (NULL);
}

/* #include <stdio.h>

char	*ft_strchr(const char *s, int c);

 int	main(void)
{
	char	*str;
	char	*result;

	str = "42 Lisboa";
	result = ft_strchr(str, 'L');
	if (result)
		printf("Encontrado: %s\n", result);
	else
		printf("Nao encontrado\n");
	result = ft_strchr(str, 'x');
	if (result)
		printf("Encontrado: %s\n", result);
	else
		printf("Nao encontrado\n");
	result = ft_strchr(str, '\0');
	if (result)
		printf("Encontrado: %s\n", result);
	else
		printf("Nao encontrado\n");
	return (0);
} */ 