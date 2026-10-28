/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 21:29:30 by tisilva           #+#    #+#             */
/*   Updated: 2026/10/28 14:38:01 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_words(char const *s, char c)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] != c && (i == 0 || s[i - 1] == c))
			count++;
		i++;
	}
	return (count);
}

static void	free_words(char **words, int count)
{
	while (count > 0)
		free(words[--count]);
	free(words);
}

static int	fill_words(char **words, char const *s, char c)
{
	int	i;
	int	start;
	int	j;

	i = 0;
	j = 0;
	while (s[i])
	{
		while (s[i] == c)
			i++;
		start = i;
		while (s[i] && s[i] != c)
			i++;
		if (i > start)
		{
			words[j] = ft_substr(s, start, i - start);
			if (!words[j])
				return (free_words(words, j), 0);
			j++;
		}
	}
	words[j] = NULL;
	return (1);
}

char	**ft_split(char const *s, char c)
{
	char	**words;

	if (!s)
		return (NULL);
	words = malloc(sizeof(char *) * (count_words(s, c) + 1));
	if (!words)
		return (NULL);
	if (!fill_words(words, s, c))
		return (NULL);
	return (words);
}

/* #include <stdio.h>

int	main(void)
{Makefile
	words = ft_split("ola mundo 42 Lisboa", ' ');
	if (!words)
		return (1);
	i = 0;
	while (words[i])
	{
		printf("words[%d] = %s\n", i, words[i]);
		free(words[i]);
		i++;
	}
	free(words);
	return (0);
} */
