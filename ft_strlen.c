/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 07:22:57 by tisilva            #+#    #+#             */
/*   Updated: 2026/10/27 21:56:07 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

/* 
#include <stdio.h>

int	main(int argc, char *argv[])
{
	char	*input;
	int		out_std;
	int		out_ft;

	if (argc != 2)
		return (1);
	input = argv[1];
	if (!strcmp(argv[1], "[NULL]"))
		input = NULL;
	// out_std = strlen(input);
	out_ft = ft_strlen(input);
	printf("out_std: [%d]\nout_ft:  [%d]\n", out_std, out_ft);
	return (0);
}
 */
