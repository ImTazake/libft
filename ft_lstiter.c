/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstiter.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 21:28:10 by tisilva           #+#    #+#             */
/*   Updated: 2026/10/28 13:45:20 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstiter(t_list *lst, void (*f)(void *))
{
	if (!f)
		return ;
	while (lst)
	{
		f(lst->content);
		lst = lst->next;
	}
}

/* #include <stdio.h>
#include "libft.h"

void	ft_double(void *content)
{
	*(int *)content *= 2;
}

void	ft_print_list(t_list *lst)
{
	while (lst)
	{
		printf("%d", *(int *)lst->content);
		if (lst->next)
			printf(" -> ");
		lst = lst->next;
	}
	printf("\n");
}

int	main(void)
{
	t_list	*lst;
	int		a;
	int		b;
	int		c;

	a = 10;
	b = 20;
	c = 30;
	lst = ft_lstnew(&a);
	ft_lstadd_back(&lst, ft_lstnew(&b));
	ft_lstadd_back(&lst, ft_lstnew(&c));
	printf("Antes:  ");
	ft_print_list(lst);
	ft_lstiter(lst, ft_double);
	printf("Depois: ");
	ft_print_list(lst);
	ft_lstclear(&lst, NULL);
	return (0);
} */