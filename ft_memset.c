/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tazake <tazake@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 07:22:30 by tazake            #+#    #+#             */
/*   Updated: 2026/09/09 07:22:31 by tazake           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

void *ft_memset(void *ptr, int value, size_t size)
{
    int    *ref;

    ref = ptr;
    while (size--)
    {
        *ref = value;
        ref++;
    }
    return (ptr);
}