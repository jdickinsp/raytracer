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
    int x;
    int y;
} BinaryNodeQueueItem;

static BinaryNodeQueueItem *binary_node_query_item_create(BinaryNode *node, int x, int y);
static int array_partition(float *array, int lo, int hi, float pivot);
static float array_range_sum(float *array, int lo, int hi);
static BinaryNode *binary_tree_build_child(float *array, int lo, int hi);
BinaryNode *binary_tree_build(float *array, size_t size);
void binary_tree_traversal(BinaryNode *root);
void binary_tree_pprint(BinaryNode *root);

#endif