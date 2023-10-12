#include <binary_tree.h>

static BinaryNodeQueueItem *binary_node_query_item_create(BinaryNode *node, int lo, int hi) {
    BinaryNodeQueueItem *item = malloc(sizeof(BinaryNodeQueueItem));
    *item = (BinaryNodeQueueItem){node, lo, hi};
    return item;
}

static int array_partition(float *array, int lo, int hi, float pivot) {
    for (int i = lo; i < hi; i++) {
        if (array[i] <= pivot) {
            float temp = array[lo];
            array[lo] = array[i];
            array[i] = temp;
            ++lo;
        }
    }
    return lo;
}

static float array_range_sum(float *array, int lo, int hi) {
    float sum = 0;
    for (int i = lo; i < hi; i++) {
        sum += array[i];
    }
    return sum;
}

static BinaryNode *binary_tree_build_child(float *array, int lo, int hi) {
    BinaryNode *node = malloc(sizeof(BinaryNode));
    if (hi - lo == 1) {
        *node = (BinaryNode){array[lo], true, NULL, NULL};
    } else {
        float median = array_range_sum(array, lo, hi) / (hi - lo);
        if (median == array[lo]) {
            // TODO handle duplicate data
            *node = (BinaryNode){array[lo], true, NULL, NULL};
        } else {
            *node = (BinaryNode){median, false, NULL, NULL};
        }
    }
    return node;
}

BinaryNode *binary_tree_build(float *array, size_t size) {
    int lo = 0;
    int hi = size;
    Queue *queue = queue_init();
    BinaryNode *root = binary_tree_build_child(array, lo, hi);
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
        int left_pivot = array_partition(array, lo, hi, node->data);
        if (left_pivot - lo > 0) {
            node->left = binary_tree_build_child(array, lo, left_pivot);
            BinaryNodeQueueItem *item_left = binary_node_query_item_create(node->left, lo, left_pivot);
            queue_add(queue, item_left, sizeof(BinaryNodeQueueItem));
        }
        if (hi - left_pivot > 0) {
            node->right = binary_tree_build_child(array, left_pivot, hi);
            BinaryNodeQueueItem *item_right = binary_node_query_item_create(node->right, left_pivot, hi);
            queue_add(queue, item_right, sizeof(BinaryNodeQueueItem));
        }
    }
    queue_free(queue);
    return root;
}

void binary_tree_traversal(BinaryNode *root) {
    Queue *visit = queue_init();
    queue_add(visit, root, sizeof(BinaryNode));
    while (visit->count > 0) {
        BinaryNode current;
        queue_pop(visit, &current);
        if (current.is_leaf == true) {
            printf("%f\n", current.data);
        } else {
            queue_add(visit, current.right, sizeof(BinaryNode));
            queue_add(visit, current.left, sizeof(BinaryNode));
        }
    }
}
