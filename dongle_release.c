/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   dongle_release.c                                  :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/22 11:39:00 by username         #+#    #+#              */
/*   Updated: 2026/10/22 11:39:00 by username        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	release_dongle(t_dongle *dongle, long cooldown)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->available = 1;
	dongle->available_at = get_time_ms() + cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_two_dongles(t_coder *coder)
{
	long	cooldown;

	cooldown = coder->sim->config.dongle_cooldown;
	release_dongle(coder->left, cooldown);
	if (coder->right != coder->left)
		release_dongle(coder->right, cooldown);
}
