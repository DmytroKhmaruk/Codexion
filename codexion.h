/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkhmaruk <dkhmaruk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 11:24:09 by dkhmaruk          #+#    #+#             */
/*   Updated: 2026/09/18 16:56:25 by dkhmaruk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
#   define CODEXION_H

#   include <pthread.h>
#   include <stdlib.h>
#   include <unistd.h>
#   include <sys/time.h>
#   include <stdio.h>
#   include <string.h>

typedef struct  s_sim       t_sim;
typedef struct  s_coder     t_coder;
typedef struct  s_dongle    t_dongle;
typedef struct  s_request   t_request;
typedef struct  s_heap      t_heap;
typedef struct  s_config    t_config;

int     parse_args(int argc, char **argv, t_config *config);
int     init_all(t_sim *sim, t_config *config);
void    destroy_dongles(t_sim *sim);
void    destroy_sim(t_sim *sim);
long    get_time_ms(void);
void    set_start_time(t_sim *sim);
int     is_stopped(t_sim *sim);
void    stop_sim(t_sim *sim)

typedef enum e_scheduler
{
    FIFO,
    EDF
}   t_scheduler;

typedef struct s_config
{
    int         number_of_coders;
    long        time_to_burnout;
    long        time_to_compile;
    long        time_to_debug;
    long        time_to_refactor;
    int         number_of_compiles_required;
    long        dongle_cooldown;
    t_scheduler scheduler;
}   t_config;

typedef struct s_request
{
    t_coder *coder;
    long    arrival_time;
    long    deadline;
}   t_request;

typedef struct s_heap
{
    t_request   **items;
    int         size;
    int         capacity;
}   t_heap;

typedef struct s_dongle
{
    int             id;
    int             available;
    long            available_at;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    t_heap          waiting;
}   t_dongle;

typedef struct s_coder
{
    int             id;
    pthread_t       tread;
    t_dongle        *left;
    t_dongle        *right;
    long            last_compile_start;
    int             compile_count;
    pthread_mutex_t state_mutex;
    struct s_sim    *sim;
}   t_coder;

typedef struct s_sim
{
    t_config        config;
    t_coder         *coders;
    t_dongle        *dongles;
    long            start_time;
    int             stopped;
    pthread_mutex_t stop_mutex;
    pthread_mutex_t print_mutex;
    pthread_t       monitor;
}   t_sim;

#endif
