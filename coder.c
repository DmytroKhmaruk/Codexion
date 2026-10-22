/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/21 16:06:52 by username         #+#    #+#              */
/*   Updated: 2026/10/22 11:21:47 by dkhmaruk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	start_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].tread, NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			stop_sim(sim);
			while (i > 0)
			{
				i--;
				pthread_join(sim->coders[i].tread, NULL);
			}
			return (0);
		}
		i++;
	}
	return (1);
}

void	join_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_join(sim->coders[i].tread, NULL);
		i++;
	}
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *) arg;
	if (coder->sim->config.number_of_coders == 1)
	{
		one_coder_routine(coder);
		return (NULL);
	}
	while (!is_stopped(coder->sim))
	{
		if (!coder_cycle(coder))
			break ;
	}
	return (NULL);
}
