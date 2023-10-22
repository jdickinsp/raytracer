#include <collections.h>

Queue *queue_init() {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->head = NULL;
    queue->current = NULL;
    queue->size = 0;
    return queue;
}

void queue_item_free(QueueItem *q) { free(q); }

void queue_free(Queue *q) {
    while (q->size > 0) {
        queue_pop(q, NULL);
    }
    free(q);
}

void queue_add(Queue *q, void *data, size_t size) {
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
    q->size++;
}

void queue_addleft(Queue *q, void *data, size_t size) {
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
    q->size++;
}

void queue_pop(Queue *q, void *dest) {
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
        q->size--;
    }
}

void queue_popleft(Queue *q, void *dest) {
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
        q->size--;
    }
}

static Tuple3Item *tuple3_item_create(void *data, size_t size, int x, int y, int z) {
    Tuple3Item *tuple = malloc(sizeof(Tuple3Item));
    *tuple = (Tuple3Item){data, size, x, y, z};
    return tuple;
}
