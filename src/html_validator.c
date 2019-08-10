#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(*arr)
typedef _Bool bool;
#define true 1
#define false 0

struct tag {
    char *name;
    int column;
};

struct node {
    struct tag val;
    struct node *next;
};

static struct tag empty_tag = {
    .name = "",
    .column = 0,
};

static struct node *stack_push(struct node **stack, struct tag val) {
    struct node *new_node = (struct node *) malloc(sizeof(*stack));
    new_node->val = val;
    new_node->next = *stack;

    *stack = new_node;
    return new_node;
}

static struct tag stack_pop(struct node **stack) {
    struct node *popped_node = *stack;
    if (!popped_node)
        return empty_tag;

    *stack = popped_node->next;

    struct tag popped_val = popped_node->val;
    free(popped_node);

    return popped_val;
}

int validate_html(struct node **stack_ptr, char *line, unsigned long len) {
    struct node *stack = *stack_ptr;

    // State machine data
    bool in_tag = false;
    char *name_ptr = NULL;
    int name_len = 0;
    bool name_ongoing = false;
    bool found_tag_char = false;
    bool current_tag_closed = false;
    bool closing_tag = false;
    int open_col = 0;

    // Iterate over each character
    for (int i = 0; i < len; i++) {
        char ch = line[i];
        int col = i + 1;

        if (name_ongoing && strchr("<>/ \t\r\n\v", ch)) {
            // Character isn't part of the tag name and we were parsing a tag name; finish up
            name_ongoing = false;

            // Terminate tag name string
            *(name_ptr + name_len) = 0;
        }

        if (ch == '<') {
            // Found tag opening bracket; reset state machine
            in_tag = true;
            found_tag_char = false;
            closing_tag = false;
            current_tag_closed = false;
            name_ptr = NULL;
            name_len = 0;
            open_col = col;
        } else if (ch == '>') {
            // Ensure we started a tag before closing it
            if (!in_tag) {
                printf("Encountered closing bracket without an opened tag on column %d\n", col);
                goto err;
            }

            // Found tag closing bracket; conclude tag
            in_tag = false;

            // Short-circuit if we closed the current tag (i.e. <tag/>)
            if (current_tag_closed)
                continue;

            // Ensure we have a tag name
            if (!found_tag_char) {
                printf("Encountered tag without element name ending on column %d\n", col);
                goto err;
            }

            if (closing_tag) {
                // We're closing the last tag

                // Ensure we have a tag to close
                if (stack == NULL) {
                    printf("Attempted to close unopened tag '%s' on column %d\n", name_ptr, col);
                    goto err;
                }

                // Ensure the name matches the tag we're trying to close
                if (strcmp(stack->val.name, name_ptr)) {
                    printf("Attempted to close tag '%s' on column %d, but last tag is '%s'\n", stack->val.name, col, name_ptr);
                    goto err;
                }

                // Finally, close the tag by popping it off the stack
                stack_pop(&stack);
            } else {
                // We're ending the current tag

                // Construct tag data struct
                struct tag tag_data = {
                    .name = name_ptr,
                    .column = col,
                };

                // Push tag onto stack
                stack_push(&stack, tag_data);
            }
        } else if (ch == '/') {
            // Found slash; check state machine context for meaning
            if (!found_tag_char) {
                // Before tag name; set flag to close tag with this name
                closing_tag = true;
            } else {
                // After tag name; close current tag
                // This allows us to short-circuit the loop in such cases
                current_tag_closed = true;
            }
        } else if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '\v') {
            // Found whitespace; ignore it
            continue;
        } else {
            // Found arbitrary character; update state
            found_tag_char = true;
            name_ongoing = true;

            if (name_len == 0)
                name_ptr = line + i;
            name_len++;
        }
    }

    // If stack isn't empty at the end, we have a dangling tag; report it
    if (stack != NULL) {
        struct tag tag = stack_pop(&stack);
        printf("Error: tag '%s' opened on column %d was never closed\n", tag.name, tag.column);
        goto err;
    }

    // If we never finished parsing a tag, report the error
    if (in_tag) {
        printf("Error: opened tag on column %d but never specified a name\n", open_col);
        goto err;
    }

    // Stack pointer has been modified; propagate the change so further checks work properly
    *stack_ptr = stack;

    return 0;

err:
    *stack_ptr = stack;
    return -1;
}

int main(void) {
    struct node *stack = NULL;

    while (1) {
        // Print prompt
        fputs("HTML> ", stdout);

        // Get input
        char *line = NULL;
        size_t size;
        if (getline(&line, &size, stdin) == -1) {
            perror("Error getting input");
            return 1;
        }

        // Mask new line in input string
        line[strlen(line) - 1] = 0;

        // Do nothing if input is empty;
        unsigned long len = strlen(line);
        if (!len)
            continue;

        // Validate input HTML
        if (!validate_html(&stack, line, len))
            printf("HTML is valid\n");
    }

    return 0;
}
