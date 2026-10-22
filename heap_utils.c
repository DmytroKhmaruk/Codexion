/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   heap_utils.c                                      :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/22 12:04:53 by username         #+#    #+#              */
/*   Updated: 2026/10/22 12:55:44 by username        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	swap_requests(t_request **a, t_request **b)
{
	t_request	*tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

int	heap_find(t_heap *heap, t_request *request)
{
	int	i;

	i = 0;
	while (i < heap->size)
	{
		if (heap->items[i] == request)
			return (i);
		i++;
	}
	return (-1);
}

int	heap_should_sift_up(t_heap *heap, int i, t_sim *sim)
{
	int	parent;

	if (i == 0)
		return (0);
	parent = (i - 1) / 2;
	return (request_before(heap->items[i], heap->items[parent], sim));
}

void	heap_sift_up(t_heap *heap, int i, t_sim *sim)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!request_before(heap->items[i], heap->items[parent], sim))
			break ;
		swap_requests(&heap->items[i],
			&heap->items[parent]);
		i = parent;
	}
}

void	heap_sift_down(t_heap *heap, int i, t_sim *sim)
{
	int	left;
	int	right;
	int	best;

	while (1)
	{
		left = i * 2 + 1;
		right = i * 2 + 2;
		best = i;
		best = heap_best_child(heap, left, best, sim);
		best = heap_best_child(heap, right, best, sim);
		if (best == i)
			break ;
		swap_requests(&heap->items[i], &heap->items[best]);
		i = best;
	}
}
