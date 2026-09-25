#include "codexion.h"

int request_dongles(t_coder *coder, t_dongle *dongle, t_request *request)
{
    long    last_compile;

    request->coder = coder;
    request->arrival_time = get_time_ms();
    pthread_mutex_lock(&coder->state_mutex);
    last_compile = coder->last_compile_start;
    pthread_mutex_unlock(&coder->state_mutex);
    request->deadline = last_compile + coder->sim->config.time_to_burnout;
    pthread_mutex_lock(&dongle->mutex);
    if (!heap_push(&dongle->waiting, request, coder->sim))
    {
        pthread_mutex_unlock(&dongle->mutex);
        return (0);
    }

    while (!is_stopped(coder->sim))
    {
        if (heap_peek(&dongle->waiting) == request && dongle->available
                && get_time_ms() >= dongle->available_at)
        {
            heap_pop(&dongle->waiting, coder->sim);
            dongle->available = 0;
            pthread_mutex_unlock(&dongle->mutex);
        }
        pthread_cond_wait(&dongle->cond, &dongle->mutex);
    }
    pthread_mutex_unlock(&dongle->mutex);
    return (0);
}