/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tazake <tazake@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 07:29:01 by tazake            #+#    #+#             */
/*   Updated: 2026/10/27 02:45:58 by tazake           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/* #include <stdlib.h>
#include <stdint.h> */

void	*ft_calloc(size_t nbr_of_elements, size_t size)
{
	void	*arr;
	size_t	total;
	size_t	i;

	if (size != 0 && nbr_of_elements > SIZE_MAX / size)
		return (NULL);
	total = nbr_of_elements * size;
	arr = malloc(total);
	if (arr == NULL)
		return (NULL);
	i = 0;
	while (i < total)
	{
		((unsigned char *)arr)[i] = 0;
		i++;
	}
	return (arr);
}
