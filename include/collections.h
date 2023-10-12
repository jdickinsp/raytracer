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

extern inline Queue *queue_init();
extern inline void queue_item_free(QueueItem *q);
extern inline void queue_free(Queue *q);
extern inline void queue_add(Queue *q, void *data, size_t size);
extern inline void queue_addleft(Queue *q, void *data, size_t size);
extern inline void queue_pop(Queue *q, void *dest);
extern inline void queue_popleft(Queue *q, void *dest);

extern inline Queue *queue_init() {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->head = NULL;
    queue->current = NULL;
    queue->count = 0;
    return queue;
}

extern inline void queue_item_free(QueueItem *q) { free(q); }

extern inline void queue_free(Queue *q) {
    while (q->count > 0) {
        queue_pop(q, NULL);
    }
    free(q);
}

extern inline void queue_add(Queue *q, void *data, size_t size) {
    QueueItem *item = (QueueItem *)malloc(sizeof(QueueItem));
    item->data = data;
    item->size = size;
    item->prev = q->current;
    item->next = NULL;
    if (q->head == NULL) {
        q->head = item;
    }
    // set previous item
    if (q->current != NULL) {
        q->current->next = item;
    }
    q->current = item;
    q->count++;
}

extern inline void queue_addleft(Queue *q, void *data, size_t size) {
    QueueItem *item = (QueueItem *)malloc(sizeof(QueueItem));
    item->data = data;
    item->size = size;
    item->prev = q->current;
    item->next = NULL;

    if (q->head != NULL) {
        QueueItem *prev = q->head;
        prev->prev = item;
        item->next = prev;
    }
    if (q->current == NULL) {
        q->current = item;
    }
    q->head = item;
    q->count++;
}

extern inline void queue_pop(Queue *q, void *dest) {
    if (q->current != NULL) {
        QueueItem *current = q->current;
        if (dest != NULL) {
            memcpy(dest, current->data, current->size);
        }
        if (current->prev != NULL) {
            current->prev->next = NULL;
            q->current = current->prev;
        } else {
            q->current = NULL;
            q->head = NULL;
        }
        queue_item_free(current);
        q->count--;
    }
}

extern inline void queue_popleft(Queue *q, void *dest) {
    if (q->head != NULL) {
        QueueItem *head = q->head;
        if (dest != NULL) {
            memcpy(dest, head->data, head->size);
        }
        if (head->next != NULL) {
            head->next->prev = NULL;
            q->head = head->next;
        } else {
            q->current = NULL;
            q->head = NULL;
        }
        queue_item_free(head);
        q->count--;
    }
}

#endif