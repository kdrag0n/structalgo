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

struct node *stack_push(struct node **stack, int val) {
    struct node *new_node = (struct node *) malloc(sizeof(*stack));
    new_node->val = val;
    new_node->next = *stack;

    *stack = new_node;
    return new_node;
}

static int stack_pop(struct node **stack) {
    struct node *popped_node = *stack;
    *stack = popped_node->next;

    int popped_val = popped_node->val;
    free(popped_node);

    return popped_val;
}

bool stack_check(struct node *stack) {
    if (stack == NULL || stack->next == NULL) {
        printf("Not enough values on stack to perform operation\n");
        return false;
    }

    return true;
}

int main(void) {
    struct node *stack = NULL;

    while (1) {
        // Print prompt
        fputs("RPN> ", stdout);

        // Get input
        char *line = NULL;
        size_t size;
        if (getline(&line, &size, stdin) == -1) {
            perror("Error getting input");
            return 1;
        }

        // Mask new line in input string
        line[strlen(line) - 1] = 0;

        // Parse and handle input
        if (!strcmp(line, "+")) { // op: add
            // ensure we have enough values to add
            if (!stack_check(stack))
                continue;

            int x = stack_pop(&stack);
            int y = stack_pop(&stack);

            stack_push(&stack, x + y);
        } else if (!strcmp(line, "-")) { // op: subtract
            // ensure we have enough values to subtract
            if (!stack_check(stack))
                continue;

            int x = stack_pop(&stack);
            int y = stack_pop(&stack);

            stack_push(&stack, y - x);
        } else if (!strcmp(line, "*")) { // op: multiply
            // ensure we have enough values to multiply
            if (!stack_check(stack))
                continue;

            int x = stack_pop(&stack);
            int y = stack_pop(&stack);

            stack_push(&stack, x * y);
        } else if (!strcmp(line, "/")) { // op: divide
            // ensure we have enough values to divide
            if (!stack_check(stack))
                continue;

            int x = stack_pop(&stack);
            int y = stack_pop(&stack);

            stack_push(&stack, y / x);
        } else if (strlen(line) > 0) { // parse and push number
            int num = atoi(line);
            stack_push(&stack, num);
        } else {
            // ignore empty input
        }

        // Print top stack value IF stack is not empty
        if (stack != NULL)
            printf("=%d\n", stack->val);
    }

    return 0;
}
