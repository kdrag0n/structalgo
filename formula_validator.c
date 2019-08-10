#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef _Bool bool;
#define true 1
#define false 0

struct node {
    int val;
    struct node *next;
};

static struct node *stack_push(struct node **stack, int val) {
    struct node *new_node = (struct node *) malloc(sizeof(*stack));
    new_node->val = val;
    new_node->next = *stack;

    *stack = new_node;
    return new_node;
}

static int stack_pop(struct node **stack) {
    struct node *popped_node = *stack;
    if (!popped_node)
        return -1;

    *stack = popped_node->next;

    int popped_val = popped_node->val;
    free(popped_node);

    return popped_val;
}

int main(void) {
    struct node *stack = NULL;

    while (1) {
repl_start:
        // Print prompt
        fputs("Formula> ", stdout);

        // Get input
        char *line = NULL;
        size_t size;
        if (getline(&line, &size, stdin) == -1) {
            perror("Error getting input");
            return 1;
        }

        // Mask new line in input string
        line[strlen(line) - 1] = 0;

        // Parse and handle input, if any
        unsigned long len = strlen(line);
        if (len > 0) {
            for (int i = 0; i < len; i++) {
                char ch = line[i];

                if (ch == '(') {
                    // Push column so we can pinpoint the source of the ( later
                    stack_push(&stack, i + 1);
                } else if (ch == ')') {
                    if (stack_pop(&stack) == -1) {
                        printf("Error: spurious closing parenthesis encountered on column %d\n", i + 1);
                        goto repl_start;
                    }
                }
            }

            if (stack != NULL)
                printf("Error: opening parenthesis on column %d was never closed\n", stack_pop(&stack));
        }

        printf("Formula is valid\n");
    }

    return 0;
}
