#include "codexion.h"

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

void    start_compile(t_coder *coder)
{
    pthread_mutex_lock(&coder->state_mutex);
    coder->last_compile_start = get_time_ms();
    pthread_mutex_unlock(&coder->state_mutex);
}

void    finish_compile(t_coder *coder)
{
    pthread_mutex_lock(&coder->state_mutex);
    coder->compile_count++;
    pthread_mutex_unlock(&coder->state_mutex);
}

void    *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    while (!is_stopped(coder->sim))
    {
        if (!take_two_dongles(coder))
            break;
        start_compile(coder);
        print_status(coder, "is compiling");
        smart_sleep(coder->sim->config.time_to_compile, coder->sim);
        release_two_dongles(coder);
        if (is_stopped(coder->sim))
            break;
        finish_compile(coder);
        print_status(coder, "is debugging");
        smart_sleep(coder->sim->config.time_to_debug, coder->sim);
        if (is_stopped(coder->sim))
            break;
        print_status(coder, "is refactoring");
        smart_sleep(coder->sim->config.time_to_refactor, coder->sim);
    }
    return (NULL);
}