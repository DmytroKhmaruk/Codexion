/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   coder_utils.c                                     :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/22 11:17:46 by username         #+#    #+#              */
/*   Updated: 2026/10/22 11:21:12 by username        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	start_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->state_mutex);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->state_mutex);
}

static void	finish_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->state_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->state_mutex);
}

void	one_coder_routine(t_coder *coder)
{
	if (request_dongle(coder, coder->left, &coder->left_request))
	{
		print_status(coder, "has taken a dongle");
		while (!is_stopped(coder->sim))
			usleep(500);
		release_dongle(coder->left, coder->sim->config.dongle_cooldown);
	}
}

int	coder_cycle(t_coder *coder)
{
	if (!take_two_dongles(coder))
		return (0);
	start_compile(coder);
	print_status(coder, "is compiling");
	smart_sleep(coder->sim->config.time_to_compile, coder->sim);
	release_two_dongles(coder);
	if (is_stopped(coder->sim))
		return (0);
	finish_compile(coder);
	print_status(coder, "is debugging");
	smart_sleep(coder->sim->config.time_to_debug, coder->sim);
	if (is_stopped(coder->sim))
		return (0);
	print_status(coder, "is refactoring");
	smart_sleep(coder->sim->config.time_to_refactor, coder->sim);
	return (1);
}
