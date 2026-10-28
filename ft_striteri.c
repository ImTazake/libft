/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 21:30:00 by tisilva           #+#    #+#             */
/*   Updated: 2026/10/28 13:39:10 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_striteri(char *s, void (*f)(unsigned int, char *))
{
	unsigned int	i;

	if (!s || !f)
		return ;
	i = 0;
	while (s[i])
	{
		f(i, &s[i]);
		i++;
	}
}

/* #include <stdio.h>
#include <ctype.h>

void	ft_striteri(char *s, void (*f)(unsigned int, char *));

void	ft_upper(unsigned int i, char *c)
{
	(void)i;
	*c = toupper((unsigned char)*c);
}

int	main(void)
{
	char	str[] = "ola mundo";

	printf("Antes: %s\n", str);
	ft_striteri(str, ft_upper);
	printf("Depois: %s\n", str);
	return (0);
} */