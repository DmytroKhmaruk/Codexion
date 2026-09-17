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

typedef enum e_scheduler
{
    FIFO,
    EDF
}   t_scheduler;

typedef struct s_config
{
    int         n_of_coders;
    long        t_to_bornout;
    long        t_to_compile;
    long        t_to_debug;
    long        t_to_refactor;
    long        dongle_coldown;
    int         n_of_compiles_required;
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