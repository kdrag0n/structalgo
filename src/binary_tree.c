#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef _Bool bool;
#define true 1
#define false 0

struct bnode {
    int val;
    int level;
    struct bnode *left;
    struct bnode *right;
};

struct qnode {
    struct bnode *val;
    struct qnode *next;
};

struct queue {
    struct qnode *head;
    struct qnode *tail;
};

static void enqueue(struct queue *queue, struct bnode *val) {
    struct qnode *new_node = malloc(sizeof(*new_node));
    new_node->val = val;

    if (queue->head == NULL)
        queue->head = new_node;

    if (queue->tail != NULL)
        queue->tail->next = new_node;

    queue->tail = new_node;
}

static struct bnode *dequeue(struct queue *queue) {
    struct qnode *popped_node = queue->head;
    queue->head = popped_node->next;

    struct bnode *popped_val = popped_node->val;
    free(popped_node);

    return popped_val;
}

static void bt_add(struct bnode **node, int val) {
    struct bnode *new_node = malloc(sizeof(*new_node));
    new_node->val = val;
    new_node->left = NULL;
    new_node->right = NULL;

    if (*node == NULL) {
        new_node->level = 0;
        *node = new_node;
    } else {
        struct bnode *target_node = *node;
        int level = 0;

        while (target_node != NULL) {
            if (val < target_node->val) {
                if (target_node->left == NULL) {
                    new_node->level = level;
                    target_node->left = new_node;
                    break;
                }

                target_node = target_node->left;
            } else {
                if (target_node->right == NULL) {
                    new_node->level = level;
                    target_node->right = new_node;
                    break;
                }

                target_node = target_node->right;
            }

            level++;
        }
    }
}

static void _bt_print_in_order(struct bnode *node, bool *printed) {
    if (node != NULL) {
        _bt_print_in_order(node->left, printed);

        if (!*printed) {
            printf("%d", node->val);
            *printed = true;
        } else {
            printf(", %d", node->val);
        }

        _bt_print_in_order(node->right, printed);
    }
}

static void bt_print_in_order(struct bnode *node) {
    bool printed = false;
    _bt_print_in_order(node, &printed);
}

static void bt_print_bfs(struct bnode *root_node) {
    struct queue queue;
    memset(&queue, 0, sizeof(queue));
    bool printed = false;

    enqueue(&queue, root_node);

    while (queue.head != NULL) {
        struct bnode *node = dequeue(&queue);

        if (node->left != NULL)
            enqueue(&queue, node->left);

        if (node->right != NULL)
            enqueue(&queue, node->right);

        if (!printed) {
            printf("%d", node->val);
            printed = true;
        } else {
            printf(", %d", node->val);
        }
    }
}

int main(void) {
    struct queue queue;
    memset(&queue, 0, sizeof(queue));

    struct bnode *btree = NULL;

    bt_add(&btree, 50);
    bt_add(&btree, 30);
    bt_add(&btree, 10);
    bt_add(&btree, 40);
    bt_add(&btree, 80);
    bt_add(&btree, 70);
    bt_add(&btree, 90);

    puts("In-order print:");
    bt_print_in_order(btree);

    putchar('\n');

    puts("BFS print:");
    bt_print_bfs(btree);

    return 0;
}
