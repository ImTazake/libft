/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 07:23:05 by tazake            #+#    #+#             */
/*   Updated: 2026/10/28 04:41:09 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *str, const char *to_find, size_t len)
{
	size_t	i;
	size_t	j;

	if (to_find[0] == '\0')
		return ((char *) str);
	i = 0;
	while (str[i] && i < len)
	{
		j = 0;
		while (i + j < len && to_find[j] && str[i + j] == to_find[j])
			j++;
		if (to_find[j] == '\0')
			return ((char *) &str[i]);
		i++;
	}
	return (NULL);
}
