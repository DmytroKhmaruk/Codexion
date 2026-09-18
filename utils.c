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
}