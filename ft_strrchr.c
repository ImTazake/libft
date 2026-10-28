/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 07:23:09 by tisilva           #+#    #+#             */
/*   Updated: 2026/10/28 13:40:54 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	while (i >= 0)
	{
		if (s[i] == (char)c)
			return ((char *)&s[i]);
		i--;
	}
	return (NULL);
}

/* #include <stdio.h>

char	*ft_strrchr(const char *s, int c);

int	main(void)
{
	const char	*str = "42 Lisboa Lisboa";
	char		*result;

	result = ft_strrchr(str, 'L');
	if (result)
		printf("Encontrado: %s\n", result);
	else
		printf("Nao encontrado\n");
	result = ft_strrchr(str, 'x');
	if (result)
		printf("Encontrado: %s\n", result);
	else
		printf("Nao encontrado\n");
	result = ft_strrchr(str, '\0');
	if (result)
		printf("Encontrado: fim da string\n");
	else
		printf("Nao encontrado\n");
	return (0);
} */