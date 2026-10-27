/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkhmaruk <dkhmaruk@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 11:24:18 by dkhmaruk          #+#    #+#             */
/*   Updated: 2026/10/27 10:21:54 by dkhmaruk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_sim(t_sim *sim, t_config *config)
{
	sim->config = *config;
	sim->coders = NULL;
	sim->dongles = NULL;
	sim->stopped = 0;
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->print_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->stop_mutex);
		return (0);
	}
	return (1);
}

int	init_dongles(t_sim *sim)
{
	int	n;
	int	i;

	n = sim->config.number_of_coders;
	sim->dongles = malloc(sizeof(t_dongle) * n);
	if (!sim->dongles)
		return (0);
	i = 0;
	while (i < n)
	{
		if (!init_one_dongle(sim, i))
		{
			cleanup_dongles(sim, i);
			return (0);
		}
		i++;
	}
	return (1);
}

int	init_coders(t_sim *sim)
{
	int	n;
	int	i;

	n = sim->config.number_of_coders;
	sim->coders = malloc(sizeof(t_coder) * n);
	if (!sim->coders)
		return (0);
	i = 0;
	while (i < n)
	{
		if (!init_one_coder(sim, i))
		{
			cleanup_coders(sim, i);
			return (0);
		}
		i++;
	}
	return (1);
}

int	init_all(t_sim *sim, t_config *config)
{
	if (!init_sim(sim, config))
		return (0);
	if (!init_dongles(sim))
		return (cleanup_sim_mutexes(sim), 0);
	if (!init_coders(sim))
	{
		destroy_dongles(sim);
		cleanup_sim_mutexes(sim);
		return (0);
	}
	return (1);
}
