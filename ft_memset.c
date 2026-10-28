/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 07:22:30 by tazake            #+#    #+#             */
/*   Updated: 2026/10/27 22:06:59 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

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
/* 
#include <stdio.h>

int	main(int argc, char *argv[])
{
    char	input;
    int     size;

    void    *addr_std;
    void    *addr_ft;
    void    *src_addr;

    if (argc != 3)
        return (1);
    
    input = *argv[1];
    size = atoi(argv[2]);
    
    // addr_std = malloc(size + 1);
    addr_std = NULL;

    // addr_ft = malloc(size + 1);
    addr_ft = NULL;


    memset(addr_std, input, size);
    // ((char *)(addr_std))[size] = 0;

    ft_memset(addr_ft, input, size);
    // ((char *)(addr_ft))[size] = 0;

    printf("addr_std: %s\n", (char *)addr_std);
    printf("addr_ft: %s\n", (char *)addr_ft);
    return (0);
}
 */