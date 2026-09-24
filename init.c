/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkhmaruk <dkhmaruk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 11:24:18 by dkhmaruk          #+#    #+#             */
/*   Updated: 2026/09/18 16:30:37 by dkhmaruk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int init_sim(t_sim *sim, t_config *config)
{
    sim->config = *config;
    sim->coders = NULL;
    sim->dongles = NULL;
    sim->stopped = 0;
    if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
        return (0);
    if(pthread_mutex_init(&sim->print_mutex, NULL) != 0)
    {
        pthread_mutex_destroy(&sim->stop_mutex);
        return (0);
    }
    return (1);
}

void cleanup_dongles(t_sim *sim, int count)
{
    int i;

    i = 0;
    while(i < count)
    {    
        pthread_cond_destroy(&sim->dongles[i].cond);
        pthread_mutex_destroy(&sim->dongles[i].mutex);
        free(sim->dongles[i].waiting.items);
        i++;
    }
    free(sim->dongles);
    sim->dongles = NULL;
}

int init_dongles(t_sim *sim)
{
    int n;
    int i;
    
    n = sim->config.number_of_coders;
    sim->dongles = malloc(sizeof(t_dongle) * n);
    if (!sim->dongles)
        return (0);
    i = 0;
    while (i < n)
    {
        sim->dongles[i].id = i;
        sim->dongles[i].available = 1;
        sim->dongles[i].available_at = 0;
        sim->dongles[i].waiting.items = malloc(sizeof(t_request *) * sim->config.number_of_coders);
        if(!sim->dongles[i].waiting.items)
        {
            cleanup_dongles(sim, i);
            return (0);
        }
        sim->dongles[i].waiting.size = 0;
        sim->dongles[i].waiting.capacity = sim->config.number_of_coders;
        if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
        {
            free(sim->dongles[i].waiting.items);
            cleanup_dongles(sim, i);
            return (0);
        }
        if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
        {
            pthread_mutex_destroy(&sim->dongles[i].mutex);
            free(sim->dongles[i].waiting.items);
            cleanup_dongles(sim, i);
            return (0);
        }
        i++;
    }
    return (1);
}

void cleanup_coders(t_sim *sim, int count)
{
    int i;

    i = 0;
    while(i < count)
    {    
        pthread_mutex_destroy(&sim->coders[i].state_mutex);
        i++;
    }
    free(sim->coders);
    sim->coders = NULL;
}

int init_coders(t_sim *sim)
{
    int n;
    int i;
    
    n = sim->config.number_of_coders;
    sim->coders = malloc(sizeof(t_coder) * n);
    if (!sim->coders)
        return (0);
    i = 0;
    while (i < n)
    {
        sim->coders[i].id = i + 1;
        sim->coders[i].compile_count = 0;
        sim->coders[i].last_compile_start = 0;
        sim->coders[i].sim = sim;
        sim->coders[i].left = &sim->dongles[i];
        sim->coders[i].right = &sim->dongles[(i + 1) % n];
        sim->coders[i].left_request.coder = &sim->coders[i];
        sim->coders[i].right_request.arrival_time = 0;
        sim->coders[i].right_request.deadline = 0;
        if (pthread_mutex_init(&sim->coders[i].state_mutex, NULL) != 0)
        {
            cleanup_coders(sim, i);
            return (0);
        }
        i++;
    }
    return (1);
}

int init_all(t_sim *sim, t_config *config)
{
    if (!init_sim(sim, config))
        return (0);
    if (!init_dongles(sim))
    {
        pthread_mutex_destroy(&sim->print_mutex);
        pthread_mutex_destroy(&sim->stop_mutex);
        return (0);
    }
    if (!init_coders(sim))
    {
        destroy_dongles(sim);
        pthread_mutex_destroy(&sim->print_mutex);
        pthread_mutex_destroy(&sim->stop_mutex);
        return (0);
    }
    return (1);
}