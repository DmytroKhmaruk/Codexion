#include "codexion.h"

long    get_time_ms(void)
{
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

void    set_start_time(t_sim *sim)
{
    int i;

    sim->start_time = get_time_ms();
    i = 0;
    while (i < sim->config.number_of_coders)
    {
        sim->coders[i].last_compile_start = sim->start_time;
        i++;
    }
}