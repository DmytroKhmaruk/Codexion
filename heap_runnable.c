/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_runnable.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkhmaruk <dkhmaruk@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 12:04:33 by dkhmaruk          #+#    #+#             */
/*   Updated: 2026/10/27 10:22:13 by dkhmaruk         ###   ########.fr       */
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
			&& (!best || request_before(heap->items[i], best, sim)))
			best = heap->items[i];
		i++;
	}
	return (best);
}

int	heap_best_child(t_heap *heap, int child,
	int best, t_sim *sim)
{
	if (child < heap->size && request_before(heap->items[child],
			heap->items[best], sim))
		return (child);
	return (best);
}
