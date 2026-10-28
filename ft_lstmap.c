/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tisilva <tisilva@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 21:28:18 by tisilva           #+#    #+#             */
/*   Updated: 2026/10/28 14:39:10 by tisilva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*new_list;
	t_list	*node;
	void	*content;

	if (!f || !del)
		return (NULL);
	new_list = NULL;
	while (lst)
	{
		content = f(lst->content);
		node = ft_lstnew(content);
		if (!node)
		{
			del(content);
			ft_lstclear(&new_list, del);
			return (NULL);
		}
		ft_lstadd_back(&new_list, node);
		lst = lst->next;
	}
	return (new_list);
}

/* #include <stdio.h>
#include <stdlib.h>
#include "libft.h"

void	*ft_double(void *content)
{
	int	*new_value;

	new_value = malloc(sizeof(int));
	if (!new_value)
		return (NULL);
	*new_value = *(int *)content * 2;
	return (new_value);
}

void	ft_del(void *content)
{
	free(content);
}

void	ft_print_list(t_list *lst)
{
	while (lst)
	{
		printf("%d", *(int *)lst->content);
		if (lst->next)
			printf(" -> ");
		lst = lst->next;
	}Makefile
	printf("\n");
}

int	main(void)
{
	t_list	*lst;
	t_list	*mapped;
	int		a;
	int		b;
	int		c;

	a = 10;
	b = 20;
	c = 30;
	lst = ft_lstnew(&a);
	ft_lstadd_back(&lst, ft_lstnew(&b));
	ft_lstadd_back(&lst, ft_lstnew(&c));
	printf("Original: ");
	ft_print_list(lst);
	mapped = ft_lstmap(lst, ft_double, ft_del);
	printf("Nova:     ");
	ft_print_list(mapped);
	ft_lstclear(&mapped, ft_del);
	ft_lstclear(&lst, NULL);
	return (0);
} */