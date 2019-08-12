#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Constants, types, and macros
 */

#define NS_PER_SEC 1000000000
#define true 1
#define false 0

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(*arr)

typedef _Bool bool;
typedef void (*sort_fn)(int *arr, int len);

/*
 * Helper functions
 */

unsigned long get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec * NS_PER_SEC) + ts.tv_nsec;
}

void swap(int *arr, int a, int b) {
    int temp_a = arr[a];
    arr[a] = arr[b];
    arr[b] = temp_a;
}

/*
 * Sorting implementations
 */

void bubble_sort(int *arr, int len) {
    // init swapped flag to true so it always runs at least once
    bool swapped = true;

    for (int pass = 0; swapped && pass < len - 1; pass++) {
        // unset swapped flag so we break this time if finished
        swapped = false;

        // len - 1 because we compare with + 1
        // len - pass because for k passes, the right-most k elements are guaranteed to be sorted
        for (int cur_i = 0; cur_i < len - 1 - pass; cur_i++) {
            int next_i = cur_i + 1;
            int cur = arr[cur_i];
            int next = arr[cur_i + 1];

            // ascending sort = next > cur
            if (next < cur) {
                swap(arr, cur_i, next_i);

                // set swapped flag so we keep going
                swapped = true;
            }
        }
    }
}

void insertion_sort(int *arr, int len) {
    // Iterate and sort, element by element, from the beginning
    for (int i = 1; i < len; i++) {
        int cur = arr[i];

        // Iterate backwards starting from the current position until we find an element greater than the current one
        int j = i;
        while (j > 0 && arr[j - 1] > cur) {
            // Shift element rightwards to make space for inserting cur
            arr[j] = arr[j - 1];
            // Move to the left element
            j--;
        }

        // Insert cur at the correct position
        arr[j] = cur;
    }
}

/*
 * CLI
 */

void print_arr(int *arr, int len) {
    for (int i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }

    putchar('\n');
}

void profile_and_test_algo(const char *label, sort_fn func, int *orig_data, int orig_len, int runs) {
    // Copy data for each run
    int **new_data = (int **) malloc(sizeof(*new_data) * runs);
    for (int run = 0; run < runs; run++) {
        new_data[run] = malloc(sizeof(*orig_data) * orig_len);
        memcpy(new_data[run], orig_data, sizeof(*orig_data) * orig_len);
    }

    // Run algorithm and track time
    unsigned long before = get_time_ns();
    for (int run = 0; run < runs; run++)
        func(new_data[run], orig_len);
    unsigned long after = get_time_ns();

    // Find and print conclusive time and results
    unsigned long delta = after - before;
    double d_per_run = (double) delta / (double) runs;

    printf("%s results: ", label);
    print_arr(new_data[0], orig_len);
    printf("%s time: %.2lf ns\n", label, d_per_run);

    // Free allocated memory
    for (int run = 0; run < runs; run++)
        free(new_data[run]);

    free(new_data);
}

int main(void) {
    int arr[] = {10, 12, 4, 18, 32, 3, 9};
    int len = ARRAY_SIZE(arr);

    printf("Unsorted: ");
    print_arr(arr, len);

    profile_and_test_algo("Bubble sort", bubble_sort, arr, len, 1000);
    profile_and_test_algo("Insertion sort", insertion_sort, arr, len, 1000);

    return 0;
}
