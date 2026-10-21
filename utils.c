/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkhmaruk <dkhmaruk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 16:08:50 by dkhmaruk          #+#    #+#             */
/*   Updated: 2026/10/21 16:42:16 by dkhmaruk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int is_stopped(t_sim *sim)
{
    int stopped;

    pthread_mutex_lock(&sim->stop_mutex);
    stopped = sim->stopped;
    pthread_mutex_unlock(&sim->stop_mutex);
    return (stopped);
}

void    stop_sim(t_sim *sim)
{
    pthread_mutex_lock(&sim->stop_mutex);
    sim->stopped = 1;
    pthread_mutex_unlock(&sim->stop_mutex);
    wake_all_dongles(sim);
}

void    wake_all_dongles(t_sim *sim)
{
    int i;

    i = 0;
    while (i < sim->config.number_of_coders)
    {
        pthread_mutex_lock(&sim->dongles[i].mutex);
        pthread_cond_broadcast(&sim->dongles[i].cond);
        pthread_mutex_unlock(&sim->dongles[i].mutex);
        i++;
    }
}
