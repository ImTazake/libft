/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tazake <tazake@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 07:22:30 by tazake            #+#    #+#             */
/*   Updated: 2026/10/26 22:15:02 by tazake           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/* #include <stddef.h> */

void    *ft_memset(void *ptr, int value, size_t size)
{
    unsigned char    *ref;

    ref = ptr;
    while (size--)
    {
        *ref = (unsigned char)value;
        ref++;
    }
    return (ptr);
}