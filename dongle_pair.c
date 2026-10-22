/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   dongle_pair.c                                     :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/22 11:38:53 by username         #+#    #+#              */
/*   Updated: 2026/10/22 11:38:53 by username        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	lock_all_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		i++;
	}
}

static void	unlock_all_dongles(t_sim *sim)
{
	int	i;

	i = sim->config.number_of_coders;
	while (i > 0)
	{
		i--;
		pthread_mutex_unlock(&sim->dongles[i].mutex);
	}
}

static int	pair_is_best(t_coder *coder)
{
	t_request	*best_left;
	t_request	*best_right;
	long		now;

	now = get_time_ms();
	best_left = heap_best_runnable(&coder->left->waiting,
		coder->sim, now);
	best_right = heap_best_runnable(&coder->right->waiting,
		coder->sim, now);
	if (best_left != &coder->left_request)
		return (0);
	if (best_right != &coder->right_request)
		return (0);
	return (1);
}

static int	try_take_pair(t_coder *coder)
{
	if (!pair_is_best(coder))
		return (0);
	remove_pair_requests(coder);
	coder->left->available = 0;
	coder->right->available = 0;
	return (1);
}

int	take_two_dongles(t_coder *coder)
{
	if (coder->left == coder->right)
		return (0);
	prepare_pair_requests(coder);
	lock_all_dongles(coder->sim);
	if (!push_pair_requests(coder))
	{
		unlock_all_dongles(coder->sim);
		return (0);
	}
	unlock_all_dongles(coder->sim);
	while (!is_stopped(coder->sim))
	{
		lock_all_dongles(coder->sim);
		if (try_take_pair(coder))
		{
			unlock_all_dongles(coder->sim);
			print_status(coder, "has taken a dongle");
			print_status(coder, "has taken a dongle");
			return (1);
		}
		unlock_all_dongles(coder->sim);
		usleep(500);
	}
	lock_all_dongles(coder->sim);
	remove_pair_requests(coder);
	unlock_all_dongles(coder->sim);
	return (0);
}
