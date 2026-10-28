/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 07:29:01 by tisilva           #+#    #+#             */
/*   Updated: 2026/10/28 10:05:35 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nbr_of_elements, size_t size)
{
	void	*arr;

	if (size != 0 && nbr_of_elements > SIZE_MAX / size)
		return (NULL);
	arr = malloc(nbr_of_elements * size);
	if (arr == NULL)
		return (NULL);
	ft_bzero(arr, nbr_of_elements * size);
	return (arr);
}
