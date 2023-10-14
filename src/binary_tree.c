#include <binary_tree.h>

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
        float median = array_range_sum(array, lo, hi) / (float)(hi - lo);
        if (median == array[lo]) {
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
    Tuple3Item item = {root, sizeof(BinaryNode), lo, hi, 0};
    BinaryNode *node;
    queue_add(queue, &item, sizeof(Tuple3Item));
    while (queue->count > 0) {
        queue_pop(queue, &item);
        node = item.data;
        lo = item.x;
        hi = item.y;
        if (node->is_leaf == true || node == NULL) {
            continue;
        }
        int left_pivot = array_partition(array, lo, hi, node->data);
        if (left_pivot == lo || left_pivot == hi) {
            printf("warn: %f, %i, %i, %i\n", node->data, lo, hi, left_pivot);
            left_pivot = hi - 1;
        }
        if (left_pivot - lo > 0) {
            node->left = binary_tree_build_child(array, lo, left_pivot);
            Tuple3Item *item_left = tuple3_item_create(node->left, sizeof(BinaryNode), lo, left_pivot, 0);
            queue_add(queue, item_left, sizeof(Tuple3Item));
        }
        if (hi - left_pivot > 0) {
            node->right = binary_tree_build_child(array, left_pivot, hi);
            Tuple3Item *item_right = tuple3_item_create(node->right, sizeof(BinaryNode), left_pivot, hi, 0);
            queue_add(queue, item_right, sizeof(Tuple3Item));
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
    queue_free(visit);
}

void binary_tree_pprint(BinaryNode *root) {
    int indent = 0;
    int parens = 0;
    bool is_left = false;
    Queue *visit = queue_init();
    Tuple3Item *item = tuple3_item_create(root, sizeof(BinaryNode), indent, parens, 0);
    Tuple3Item current;
    BinaryNode *node;
    queue_add(visit, item, sizeof(Tuple3Item));
    while (visit->count > 0) {
        queue_pop(visit, &current);
        node = current.data;
        indent = current.x;
        parens = current.y;
        is_left = parens == 1;
        int space = indent * 2;
        printf("%*s", space, "");
        if (is_left) {
            printf("l:");
        } else if (!is_left && indent != 0) {
            printf("r:");
        }
        if (node->is_leaf == true) {
            printf("=>(d=%f", node->data);
            printf("%.*s,\n", is_left ? 1 : parens, ")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))");
        } else {
            printf("->(,\n");
            Tuple3Item *item_r = tuple3_item_create(node->right, sizeof(BinaryNode), indent + 1, parens + 1, 0);
            queue_add(visit, item_r, sizeof(Tuple3Item));
            Tuple3Item *item_l = tuple3_item_create(node->left, sizeof(BinaryNode), indent + 1, 1, 0);
            queue_add(visit, item_l, sizeof(Tuple3Item));
        }
    }
    queue_free(visit);
}
