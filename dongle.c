/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkhmaruk <dkhmaruk@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 16:07:06 by dkhmaruk          #+#    #+#             */
/*   Updated: 2026/10/27 10:22:17 by dkhmaruk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	prepare_request(t_coder *coder, t_request *request)
{
	long	last_compile;

	request->coder = coder;
	request->arrival_time = get_time_ms();
	pthread_mutex_lock(&coder->state_mutex);
	last_compile = coder->last_compile_start;
	pthread_mutex_unlock(&coder->state_mutex);
	request->deadline = last_compile + coder->sim->config.time_to_burnout;
}

static int	request_ready(t_dongle *dongle, t_request *request)
{
	if (heap_peek(&dongle->waiting) != request)
		return (0);
	if (!dongle->available)
		return (0);
	if (get_time_ms() < dongle->available_at)
		return (0);
	return (1);
}

static void	wait_request(t_dongle *dongle, t_request *request)
{
	struct timespec	timeout;

	if (heap_peek(&dongle->waiting) == request && dongle->available)
	{
		ms_to_timespec(dongle->available_at, &timeout);
		pthread_cond_timedwait(&dongle->cond,
			&dongle->mutex, &timeout);
	}
	else
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
}

int	request_dongle(t_coder *coder, t_dongle *dongle, t_request *request)
{
	prepare_request(coder, request);
	pthread_mutex_lock(&dongle->mutex);
	if (!heap_push(&dongle->waiting, request, coder->sim))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	while (!is_stopped(coder->sim))
	{
		if (request_ready(dongle, request))
		{
			heap_pop(&dongle->waiting, coder->sim);
			dongle->available = 0;
			pthread_mutex_unlock(&dongle->mutex);
			return (1);
		}
		wait_request(dongle, request);
	}
	heap_remove(&dongle->waiting, request, coder->sim);
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}
