/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_pair_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkhmaruk <dkhmaruk@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 11:36:30 by dkhmaruk          #+#    #+#             */
/*   Updated: 2026/10/27 10:22:28 by dkhmaruk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	prepare_pair_requests(t_coder *coder)
{
	long	now;
	long	last;

	pthread_mutex_lock(&coder->state_mutex);
	last = coder->last_compile_start;
	pthread_mutex_unlock(&coder->state_mutex);
	now = get_time_ms();
	coder->left_request.coder = coder;
	coder->left_request.arrival_time = now;
	coder->left_request.deadline = last + coder->sim->config.time_to_burnout;
	coder->right_request.coder = coder;
	coder->right_request.arrival_time = now;
	coder->right_request.deadline = coder->left_request.deadline;
}

int	push_pair_requests(t_coder *coder)
{
	if (!heap_push(&coder->left->waiting, & coder->left_request, coder->sim))
		return (0);
	if (!heap_push(&coder->right->waiting, & coder->right_request, coder->sim))
	{
		heap_remove(&coder->left->waiting, & coder->left_request, coder->sim);
		return (0);
	}
	return (1);
}

void	remove_pair_requests(t_coder *coder)
{
	heap_remove(&coder->left->waiting,
		&coder->left_request, coder->sim);
	heap_remove(&coder->right->waiting,
		&coder->right_request, coder->sim);
}
