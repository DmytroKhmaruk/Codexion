/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   init_utils.c                                       :+:      :+:    :+:   */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/22 12:22:42 by username         #+#    #+#              */
/*   Updated: 2026/10/22 12:41:01 by dkhmaruk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	init_dongle_data(t_sim *sim, int i)
{
	t_dongle	*dongle;

	dongle = &sim->dongles[i];
	dongle->id = i;
	dongle->available = 1;
	dongle->available_at = 0;
	dongle->waiting.size = 0;
	dongle->waiting.capacity = sim->config.number_of_coders;
}

int	init_one_dongle(t_sim *sim, int i)
{
	t_dongle	*dongle;
	int			n;

	dongle = &sim->dongles[i];
	n = sim->config.number_of_coders;
	init_dongle_data(sim, i);
	dongle->waiting.items = malloc(sizeof(t_request *) * n);
	if (!dongle->waiting.items)
		return (0);
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
	{
		free(dongle->waiting.items);
		return (0);
	}
	if (pthread_cond_init(&dongle->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&dongle->mutex);
		free(dongle->waiting.items);
		return (0);
	}
	return (1);
}

static void	init_coder_requests(t_coder *coder)
{
	coder->left_request.coder = coder;
	coder->left_request.arrival_time = 0;
	coder->left_request.deadline = 0;
	coder->right_request.coder = coder;
	coder->right_request.arrival_time = 0;
	coder->right_request.deadline = 0;
}

int	init_one_coder(t_sim *sim, int i)
{
	t_coder	*coder;
	int		n;

	n = sim->config.number_of_coders;
	coder = &sim->coders[i];
	coder->id = i + 1;
	coder->compile_count = 0;
	coder->last_compile_start = 0;
	coder->sim = sim;
	coder->left = &sim->dongles[i];
	coder->right = &sim->dongles[(i + 1) % n];
	init_coder_requests(coder);
	if (pthread_mutex_init(&coder->state_mutex, NULL) != 0)
		return (0);
	return (1);
}
