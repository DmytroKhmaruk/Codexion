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

int all_compiles_done(t_sim *sim)
{
    int i;
    int count;

    i = 0;
    while (i < sim->config.number_of_coders)
    {
        pthread_mutex_lock(&sim->coders[i].state_mutex);
        count = sim->coders[i].compile_count;
        pthread_mutex_unlock(&sim->coders[i].state_mutex);
        if(count < sim->config.number_of_compiles_required)
            return (0);
        i++;
    }
    return (1);
}

void *monitor_routine(void *arg)
{
    t_sim *sim;
    int i;

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
        if (all_compiles_done(sim))
        {
            stop_sim(sim);
            return (NULL);
        }
        usleep(500);
    }
    return (NULL);
}