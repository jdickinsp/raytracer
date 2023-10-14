#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Queue *q = queue_init();
 * queue_add(q, (Vec3) { 1, 2, 3}, sizeof(Vec3) )
 * Vec3 a;
 * queue_pop(q, &a);
 * queue_free(q);
 */

typedef struct {
    void *data;
    size_t size;
    int x;
    int y;
    int z;
} Tuple3Item;

Tuple3Item *tuple3_item_create(void *data, size_t size, int x, int y, int z);

typedef struct QueueItem {
    void *data;
    size_t size;
    struct QueueItem *prev;
    struct QueueItem *next;
} QueueItem;

typedef struct Queue {
    QueueItem *head;
    QueueItem *current;
    int count;
} Queue;

Queue *queue_init();
void queue_item_free(QueueItem *q);
void queue_free(Queue *q);
void queue_add(Queue *q, void *data, size_t size);
void queue_addleft(Queue *q, void *data, size_t size);
void queue_pop(Queue *q, void *dest);
void queue_popleft(Queue *q, void *dest);

#endif