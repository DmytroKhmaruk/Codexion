/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkhmaruk <dkhmaruk@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 16:07:06 by dkhmaruk          #+#    #+#             */
/*   Updated: 2026/10/21 16:07:08 by dkhmaruk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int request_dongle(t_coder *coder, t_dongle *dongle, t_request *request)
{
    long    last_compile;
    struct  timespec timeout;
    

    request->coder = coder;
    request->arrival_time = get_time_ms();
    pthread_mutex_lock(&coder->state_mutex);
    last_compile = coder->last_compile_start;
    pthread_mutex_unlock(&coder->state_mutex);
    request->deadline = last_compile + coder->sim->config.time_to_burnout;
    pthread_mutex_lock(&dongle->mutex);
    if (!heap_push(&dongle->waiting, request, coder->sim))
    {
        pthread_mutex_unlock(&dongle->mutex);
        return (0);
    }

    while (!is_stopped(coder->sim))
    {
        if (heap_peek(&dongle->waiting) == request && dongle->available
                && get_time_ms() >= dongle->available_at)
        {
            heap_pop(&dongle->waiting, coder->sim);
            dongle->available = 0;
            pthread_mutex_unlock(&dongle->mutex);
            return (1);
        }
        if (heap_peek(&dongle->waiting) == request && dongle->available)
        {
            ms_to_timespec(dongle->available_at, &timeout);
            pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &timeout);
        }
        else
            pthread_cond_wait(&dongle->cond, &dongle->mutex);
    }
    heap_remove(&dongle->waiting, request, coder->sim);
    pthread_mutex_unlock(&dongle->mutex);
    return (0);
}

void    release_dongle(t_dongle *dongle, long cooldown)
{
    pthread_mutex_lock(&dongle->mutex);
    dongle->available = 1;
    dongle->available_at = get_time_ms() + cooldown;
    pthread_cond_broadcast(&dongle->cond);
    pthread_mutex_unlock(&dongle->mutex);
}

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

static void	remove_pair_requests(t_coder *coder)
{
	heap_remove(&coder->left->waiting,
		&coder->left_request, coder->sim);
	heap_remove(&coder->right->waiting,
		&coder->right_request, coder->sim);
}

static void	prepare_pair_requests(t_coder *coder)
{
	long	now;
	long	last;

	pthread_mutex_lock(&coder->state_mutex);
	last = coder->last_compile_start;
	pthread_mutex_unlock(&coder->state_mutex);
	now = get_time_ms();
	coder->left_request.coder = coder;
	coder->left_request.arrival_time = now;
	coder->left_request.deadline = last
		+ coder->sim->config.time_to_burnout;
	coder->right_request.coder = coder;
	coder->right_request.arrival_time = now;
	coder->right_request.deadline
		= coder->left_request.deadline;
}

static int	push_pair_requests(t_coder *coder)
{
	if (!heap_push(&coder->left->waiting,
			&coder->left_request, coder->sim))
		return (0);
	if (!heap_push(&coder->right->waiting,
			&coder->right_request, coder->sim))
	{
		heap_remove(&coder->left->waiting,
			&coder->left_request, coder->sim);
		return (0);
	}
	return (1);
}

int	take_two_dongles(t_coder *coder)
{
	t_request	*best_left;
	t_request	*best_right;
	long		now;

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
		now = get_time_ms();
		best_left = heap_best_runnable(&coder->left->waiting,
				coder->sim, now);
		best_right = heap_best_runnable(&coder->right->waiting,
				coder->sim, now);
		if (best_left == &coder->left_request
			&& best_right == &coder->right_request)
		{
			remove_pair_requests(coder);
			coder->left->available = 0;
			coder->right->available = 0;
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

void    release_two_dongles(t_coder *coder)
{
    long cooldown;

    cooldown = coder->sim->config.dongle_cooldown;
    release_dongle(coder->left, cooldown);
    if (coder->right != coder->left)
        release_dongle(coder->right, cooldown);
}
