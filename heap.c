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

t_request   *heap_pop(t_heap *heap, t_sim *sim)
{
    t_request   *result;
    int         i;
    int         left;
    int         right;
    int         best;

    if (heap->size == 0)
        return (NULL);
    result = heap->items[0];
    heap->size--;
    if (heap->size == 0)
        return (result);
    heap->items[0] = heap->items[heap->size];
    i = 0;
    while (1)
    {
        left = i * 2 + 1;
        right = i * 2 + 2;
        best = i;
        if (left < heap->size && request_before(heap->items[left],
                heap->items[best], sim))
            best = left;
        if (right < heap->size && request_before(heap->items[right],
                heap->items[best], sim))
            best = right;
        if (best == i)
            break;
        swap_requests(&heap->items[i], &heap->items[best]);
        i = best;
    }
    return (result);
}