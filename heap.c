#include "codexion.h"

int request_before(t_request *a, t_request *b, t_sim *sim)
{
    if (sim->config.scheduler == FIFO)
    {
        if (a->arrival_time < b->arrival_time)
            return (1);
        if (a->arrival_time > b->arrival_time)
            return (0);
    }
    else
    {
        if (a->deadline < b->deadline)
            return (1);
        if (a->deadline > b->deadline)
            return (0);
    }
    return (a->coder->id < b->coder->id);
}

void    swap_requests(t_request **a, t_request **b)
{
    t_request *tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

int heap_push(t_heap *heap, t_request *request, t_sim *sim)
{
    int i;
    int parent;

    if (heap->size >= heap->capacity)
        return (0);
    i = heap->size;
    heap->items[i] = request;
    heap->size++;
    while (i > 0)
    {
        parent = (i - 1) / 2;
        if (!request_before(heap->items[i], heap->items[parent], sim))
            break;
        swap_requests(&heap->items[i], &heap->items[parent]);
        i = parent;
    }
    return (1);
}