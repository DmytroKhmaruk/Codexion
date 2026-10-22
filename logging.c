/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   logging.c                                         :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/21 16:07:30 by username         #+#    #+#              */
/*   Updated: 2026/10/22 11:12:40 by username        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_status(t_coder *coder, char *message)
{
	long	time;

	pthread_mutex_lock(&coder->sim->print_mutex);
	if (!is_stopped(coder->sim))
	{
		time = get_time_ms() - coder->sim->start_time;
		printf("%ld %d %s \n", time, coder->id, message);
	}
	pthread_mutex_unlock(&coder->sim->print_mutex);
}

void	report_burnout(t_coder *coder)
{
	t_sim	*sim;
	long	time;

	sim = coder->sim;
	pthread_mutex_lock(&sim->print_mutex);
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stopped = 1;
	time = get_time_ms() - sim->start_time;
	printf("%ld %d burned out\n", time, coder->id);
	pthread_mutex_unlock(&sim->stop_mutex);
	pthread_mutex_unlock(&sim->print_mutex);
	wake_all_dongles(sim);
}
