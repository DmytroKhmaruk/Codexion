/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   heap_runnable.c                                   :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/22 12:04:33 by username         #+#    #+#              */
/*   Updated: 2026/10/22 12:04:33 by username        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	coder_is_runnable(t_coder *coder, long now)
{
	if (!coder->left->available)
		return (0);
	if (!coder->right->available)
		return (0);
	if (now < coder->left->available_at)
		return (0);
	if (now < coder->right->available_at)
		return (0);
	return (1);
}

t_request	*heap_best_runnable(t_heap *heap, t_sim *sim, long now)
{
	t_request	*best;
	int			i;

	best = NULL;
	i = 0;
	while (i < heap->size)
	{
		if (coder_is_runnable(heap->items[i]->coder, now)
				&& (!best || request_before(heap->items[i],
				best, sim)))
		best = heap->items[i];
		i++;
	}
	return (best);
}
