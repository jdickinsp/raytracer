#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <collections.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct BinaryNode {
    float data;
    bool is_leaf;
    struct BinaryNode *left;
    struct BinaryNode *right;
} BinaryNode;

typedef struct {
    BinaryNode *node;
    int lo;
    int hi;
} BinaryNodeQueueItem;

static inline int array_partition(int *array, int lo, int hi, float pivot);
static inline float array_range_sum(int *array, int lo, int hi);
static inline BinaryNode *binary_tree_build_child(int *indexes, int lo, int hi);
extern inline BinaryNode *binary_tree_build(int *indexes, size_t size);
extern inline void binary_tree_traversal(BinaryNode *root);

static inline BinaryNodeQueueItem *binary_node_query_item_create(BinaryNode *node, int lo, int hi) {
    BinaryNodeQueueItem *item = malloc(sizeof(BinaryNodeQueueItem));
    *item = (BinaryNodeQueueItem){node, lo, hi};
    return item;
}

static inline int array_partition(int *array, int lo, int hi, float pivot) {
    for (int i = lo; i < hi; i++) {
        if ((float)array[i] <= pivot) {
            int temp = array[lo];
            array[lo] = array[i];
            array[i] = temp;
            ++lo;
        }
    }
    return lo;
}

static inline float array_range_sum(int *array, int lo, int hi) {
    float sum = 0;
    for (int i = lo; i < hi; i++) {
        sum += array[i];
    }
    return sum;
}

static inline BinaryNode *binary_tree_build_child(int *indexes, int lo, int hi) {
    BinaryNode *node = malloc(sizeof(BinaryNode));
    if (hi - lo == 1) {
        *node = (BinaryNode){(float)indexes[lo], true, NULL, NULL};
    } else {
        float median = array_range_sum(indexes, lo, hi) / (float)(hi - lo);
        *node = (BinaryNode){median, false, NULL, NULL};
    }
    return node;
}

extern inline BinaryNode *binary_tree_build(int *indexes, size_t size) {
    int lo = 0;
    int hi = size;
    Queue *queue = queue_init();
    BinaryNode *root = binary_tree_build_child(indexes, lo, hi);
    BinaryNodeQueueItem item = {root, lo, hi};
    BinaryNode *node;
    queue_add(queue, &item, sizeof(BinaryNodeQueueItem));
    while (queue->count > 0) {
        queue_pop(queue, &item);
        node = item.node;
        lo = item.lo;
        hi = item.hi;
        if (node->is_leaf == true || node == NULL) {
            continue;
        }
        int left_pivot = array_partition(indexes, lo, hi, node->data);
        if (left_pivot - lo > 0) {
            node->left = binary_tree_build_child(indexes, lo, left_pivot);
            BinaryNodeQueueItem *item_left = binary_node_query_item_create(node->left, lo, left_pivot);
            queue_add(queue, item_left, sizeof(BinaryNodeQueueItem));
        }
        if (hi - left_pivot > 0) {
            node->right = binary_tree_build_child(indexes, left_pivot, hi);
            BinaryNodeQueueItem *item_right = binary_node_query_item_create(node->right, left_pivot, hi);
            queue_add(queue, item_right, sizeof(BinaryNodeQueueItem));
        }
    }
    queue_free(queue);
    return root;
}

extern inline void binary_tree_traversal(BinaryNode *root) {
    Queue *visit = queue_init();
    queue_add(visit, root, sizeof(BinaryNode));
    while (visit->count > 0) {
        BinaryNode current;
        queue_pop(visit, &current);
        // printf("current.data: %f, %i \n", current.data, current.is_leaf);
        if (current.is_leaf == true) {
            printf("is_leaf %f\n", current.data);
        } else {
            queue_add(visit, current.right, sizeof(BinaryNode));
            queue_add(visit, current.left, sizeof(BinaryNode));
        }
    }
}

#endif