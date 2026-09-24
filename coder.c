#include "codexion.h"

void    *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    while (!is_stopped(coder->sim))
    {
        /* code */
        break;
    }
    
    return (NULL);
}

int start_coders(t_sim *sim)
{
    int i;

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

void    join_coders(t_sim *sim)
{
    int i;

    i = 0;
    while (i < sim->config.number_of_coders)
    {
        pthread_join(sim->coders[i].tread, NULL);
        i++;
    }
}