/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   cleanup.c                                         :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/21 16:06:38 by username         #+#    #+#              */
/*   Updated: 2026/10/22 11:07:02 by username        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_dongles(t_sim *sim)
{
	int	i;

	if (!sim->dongles)
		return ;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_cond_destroy(&sim->dongles[i].cond);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		free(sim->dongles[i].waiting.items);
		i++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
}

void	destroy_coders(t_sim *sim)
{
	int	i;

	if (!sim->coders)
		return ;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_destroy(&sim->coders[i].state_mutex);
		i++;
	}
	free(sim->coders);
	sim->coders = NULL;
}

void	destroy_sim(t_sim *sim)
{
	destroy_coders(sim);
	destroy_dongles(sim);
	pthread_mutex_destroy(&sim->print_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
}
