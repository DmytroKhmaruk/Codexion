#include "codexion.h"

int start_simulation(t_sim *sim)
{
    set_start_time(sim);
    if (!start_coders(sim))
        return (0);
    if (pthread_create(&sim->monitor, NULL, monitor_routine, sim) != 0)
    {
        stop_sim(sim);
        join_coders(sim);
        return (0);
    }
    return (1);
}

void    wait_simulation(t_sim *sim)
{
    pthread_join(sim->monitor, NULL);
    join_coders(sim);
}