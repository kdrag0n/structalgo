#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __linux__
#include <bsd/stdlib.h>
#endif

/*
 * Constants, types, and macros
 */

#define SORT_ARRAY_ELEMENTS 1000
#define SORT_RUNS_PER_IMPL 2000

#define NS_PER_SEC 1000000000
#define true 1
#define false 0

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(*arr)

typedef _Bool bool;
typedef void (*sort_fn)(int *arr, int len);

/*
 * Helper functions
 */

static inline unsigned long get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec * NS_PER_SEC) + ts.tv_nsec;
}

static void swap(int *arr, int a, int b) {
    int temp_a = arr[a];
    arr[a] = arr[b];
    arr[b] = temp_a;
}

/*
 * Sorting implementations
 */

static void bubble_sort(int *arr, int len) {
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

static void insertion_sort(int *arr, int len) {
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

static int _qs_partition(int *arr, int lo, int hi) {
    // Use separate left and right variables to preserve original lo and hi values for final swap
    int left = lo;
    int right = hi;

    // Assume the first element will work as a decent pivot value
    int pivot_elem = arr[lo];

    // Move lo and hi inwards until they overlap
    while (left < right) {
        // Move left as close to the pivot as possible while staying on the left (i.e. inwards, -> right)
        while (left < right && arr[left] <= pivot_elem)
            left++;
        
        // Move right as close to the pivot as possible while staying on the right (i.e. inwards, -> left)
        while (right >= left && arr[right] > pivot_elem)
            right--;

        // Swap left and right values if their target indices didn't overlap
        if (left < right)
            swap(arr, left, right);
    }

    // Since we swapped, the right index should now point to the lowest element so assign it
    arr[lo] = arr[right];

    // Because we now have a (left, right) pair within the array, the right element must be in the center
    arr[right] = pivot_elem;

    // Return the index of our newly swapped center element
    return right;
}

static void _qs_sort(int *arr, int lo, int hi) {
    // Terminate when there's overlap
    if (hi < lo)
        return;

    // Partition the current region
    // Note that this guarantees that pivot_i will be the middle value, so we don't need to include it when sorting
    int pivot_i = _qs_partition(arr, lo, hi);

    // Sort the lower partition
    _qs_sort(arr, lo, pivot_i - 1);

    // Sort the higher partition
    _qs_sort(arr, pivot_i + 1, hi);
}

static void quick_sort(int *arr, int len) {
    _qs_sort(arr, 0, len - 1);
}

/*
 * stdlib implementation wrappers
 */

static int int_comparator(const void *a, const void *b) {
    return *(int *)a - *(int *)b;
}

static void stdlib_qsort(int *arr, int len) {
    qsort(arr, len, sizeof(*arr), int_comparator);
}

static void stdlib_mergesort(int *arr, int len) {
    mergesort(arr, len, sizeof(*arr), int_comparator);
}

static void stdlib_heapsort(int *arr, int len) {
    heapsort(arr, len, sizeof(*arr), int_comparator);
}

/*
 * CLI
 */

static void print_arr(int *arr, int len) {
    if (len > 0) {
        printf("%d", arr[0]);

        for (int i = 1; i < len; i++) {
            printf(", %d", arr[i]);
        }
    }

    putchar('\n');
}

static void profile_and_test_algo(const char *label, sort_fn func, int *orig_data, int orig_len) {
    printf("%s: ", label);
    fflush(stdout);

    int runs = SORT_RUNS_PER_IMPL;
    size_t orig_bytes = sizeof(*orig_data) * orig_len;

    // Sort a reference copy using stdlib functions
    int *sorted_data = malloc(orig_bytes);
    memcpy(sorted_data, orig_data, orig_bytes);
    stdlib_qsort(sorted_data, orig_len);

    // Copy data for each run
    int **new_data = (int **) malloc(sizeof(*new_data) * runs);
    for (int run = 0; run < runs; run++) {
        new_data[run] = malloc(orig_bytes);
        memcpy(new_data[run], orig_data, orig_bytes);
    }

    // Run algorithm and track time
    unsigned long before = get_time_ns();
    for (int run = 0; run < runs; run++)
        func(new_data[run], orig_len);
    unsigned long after = get_time_ns();

    // Find conclusive time
    unsigned long delta = after - before;
    double d_per_run = (double) delta / (double) runs;

    // Verify results and print error if they don't match the reference
    if (memcmp(sorted_data, new_data[0], orig_bytes)) {
        fprintf(stderr, "%s returned incorrect results.\nOriginal: ", label);
        print_arr(orig_data, orig_len);
        fprintf(stderr, "Expected: ");
        print_arr(sorted_data, orig_len);
        fprintf(stderr, "Found: ");
        print_arr(new_data[0], orig_len);
    }

    printf("%.1lf μs\n", d_per_run / 1000);

    // Free allocated memory
    for (int run = 0; run < runs; run++)
        free(new_data[run]);

    free(new_data);
}

void fill_random(int *arr, int len) {
    for (int i = 0; i < len; i++)
        arr[i] = rand();
}

int main(void) {
    int arr[SORT_ARRAY_ELEMENTS];
    int len = ARRAY_SIZE(arr);

    printf("Filling array with %d random numbers... ", SORT_ARRAY_ELEMENTS);
    fflush(stdout);
    srand(time(NULL));
    fill_random(arr, len);
    printf("done.\n\n");

    profile_and_test_algo("Bubble sort", bubble_sort, arr, len);
    profile_and_test_algo("Insertion sort", insertion_sort, arr, len);
    profile_and_test_algo("Quick sort", quick_sort, arr, len);

    putchar('\n');

    profile_and_test_algo("stdlib quick sort", stdlib_qsort, arr, len);
    profile_and_test_algo("stdlib merge sort", stdlib_mergesort, arr, len);
    profile_and_test_algo("stdlib heap sort", stdlib_heapsort, arr, len);

    return 0;
}
