#include "codexion.h"

int request_dongle(t_coder *coder, t_dongle *dongle, t_request *request)
{
    long    last_compile;
    struct  timespec timeout;
    

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
            return (1);
        }
        if (heap_peek(&dongle->waiting) == request && dongle->available)
        {
            ms_to_timespec(dongle->available_at, &timeout);
            pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &timeout);
        }
        else
            pthread_cond_wait(&dongle->cond, &dongle->mutex);
    }
    heap_remove(&dongle->waiting, request, coder->sim);
    pthread_mutex_unlock(&dongle->mutex);
    return (0);
}

void    release_dongle(t_dongle *dongle, long cooldown)
{
    pthread_mutex_lock(&dongle->mutex);
    dongle->available = 1;
    dongle->available_at = get_time_ms() + cooldown;
    pthread_cond_broadcast(&dongle->cond);
    pthread_mutex_unlock(&dongle->mutex);
}

int take_two_dongles(t_coder *coder)
{
    t_dongle    *first;
    t_dongle    *second;
    t_request   *first_request;
    t_request   *second_request;

    if (coder->left == coder->right)
        return (0);
    if (coder->left->id < coder->right->id)
    {
        first = coder->left;
        second = coder->right;
        first_request = &coder->left_request;
        second_request = &coder->right_request;
    }
    else
    {
        first = coder->right;
        second = coder->left;
        first_request = &coder->right_request;
        second_request = &coder->left_request;
    }
    if (!request_dongle(coder, first, first_request))
        return (0);
    print_status(coder, "has taken a dongle");
    if (!request_dongle(coder, second, second_request))
    {
        release_dongle(first, coder->sim->config.dongle_cooldown);
        return (0);
    }
    print_status(coder, "has taken a dongle");
    return (1);
}

void    release_two_dongles(t_coder *coder)
{
    long cooldown;

    cooldown = coder->sim->config.dongle_cooldown;
    release_dongle(coder->left, cooldown);
    if (coder->right != coder->left)
        release_dongle(coder->right, cooldown);
}