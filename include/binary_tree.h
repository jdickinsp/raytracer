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

static BinaryNodeQueueItem *binary_node_query_item_create(BinaryNode *node, int lo, int hi);
static int array_partition(int *array, int lo, int hi, float pivot);
static float array_range_sum(int *array, int lo, int hi);
static BinaryNode *binary_tree_build_child(int *indexes, int lo, int hi);
BinaryNode *binary_tree_build(int *indexes, size_t size);
void binary_tree_traversal(BinaryNode *root);

#endif