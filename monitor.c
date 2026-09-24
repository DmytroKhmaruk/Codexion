#include "codexion.h"
long get_last_compile(t_coder *coder)
{
    long    last;

    pthread_mutex_lock(&coder->state_mutex);
    last = coder->last_compile_start;
    pthread_mutex_unlock(&coder->state_mutex);
    return (last);
}

int has_burned_out(t_coder *coder)
{
    long    last;
    long    now;

    last = get_last_compile(coder);
    now = get_time_ms();
    if (now - last >= coder->sim->config.time_to_burnout)
        return (1);
    return (0);
}

void    *monitor_routine(void *arg)
{
    t_sim   *sim;
    int     i;

    sim = (t_sim *)arg;
    while (!is_stopped(sim))
    {
        i = 0;
        while (i < sim->config.number_of_coders)
        {
            if (has_burned_out(&sim->coders[i]))
            {
                report_burnout(&sim->coders[i]);
                return (NULL);
            }
            i++;
        }
        usleep(500);
    }
    return (NULL);
}