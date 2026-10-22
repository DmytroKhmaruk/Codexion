/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   heap.c                                            :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/21 16:07:15 by username         #+#    #+#              */
/*   Updated: 2026/10/22 12:05:49 by username        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	request_before(t_request *a, t_request *b, t_sim *sim)
{
	if (sim->config.scheduler == FIFO)
	{
		if (a->arrival_time < b->arrival_time)
			return (1);
		if (a->arrival_time > b->arrival_time)
			return (0);
	}
	else
	{
		if (a->deadline < b->deadline)
			return (1);
		if (a->deadline > b->deadline)
			return (0);
	}
	return (a->coder->id < b->coder->id);
}

int	heap_push(t_heap *heap, t_request *request, t_sim *sim)
{
	int	i;
	int	parent;

	if (heap->size >= heap->capacity)
		return (0);
	i = heap->size;
	heap->items[i] = request;
	heap->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!request_before(heap->items[i], heap->items[parent], sim))
			break ;
		swap_requests(&heap->items[i], &heap->items[parent]);
		i = parent;
	}
	return (1);
}

t_request	*heap_pop(t_heap *heap, t_sim *sim)
{
	t_request	*result;

	if (heap->size == 0)
		return (NULL);
	result = heap->items[0];
	heap->size--;
	if (heap->size == 0)
		return (result);
	heap->items[0] = heap->items[heap->size];
	heap_sift_down(heap, 0, sim);
	return (result);
}

t_request	*heap_peek(t_heap *heap)
{
	if (heap->size == 0)
		return (NULL);
	return (heap->items[0]);
}

int	heap_remove(t_heap *heap, t_request *request, t_sim *sim)
{
	int	i;

	i = heap_find(heap, request);
	if (i == -1)
		return (0);
	heap->size--;
	if (i == heap->size)
		return (1);
	heap->items[i] = heap->items[heap->size];
	if (heap_should_sift_up(heap, i, sim))
		heap_sift_up(heap, i, sim);
	else
		heap_sift_down(heap, i, sim);
	return (1);
}
