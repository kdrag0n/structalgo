#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Constants, types, and macros
 */

#define true 1
#define false 0

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(*arr)

typedef _Bool bool;
typedef void (*sort_fn)(int *arr, int len);

static void print_arr(int *arr, int len) {
    for (int i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }

    putchar('\n');
}

static int merge_arrays(int *dest_arr, int dest_cap, int *arr1, int len1, int *arr2, int len2) {
    int dest_pos = 0;
    int i1 = 0;
    int i2 = 0;

    // Loop through common indices
    while (i1 < len1 && i2 < len2) {
        int val1 = arr1[i1];
        int val2 = arr2[i2];

        // Use the lower value and increment the index of the array we draw from
        if (val1 < val2) {
            // val1 is lower; use it
            dest_arr[dest_pos++] = val1;
            i1++;
        } else if (val1 > val2) {
            // val2 is lower; use it
            dest_arr[dest_pos++] = val2;
            i2++;
        } else {
            // val1 and val2 are equal; use val1 and increment both since they both had the value drawn
            dest_arr[dest_pos++] = val1;
            i1++;
            i2++;
        }
    }

    // Draw the remaining elements from arr1 if necessary
    while (i1 < len1) {
        dest_arr[dest_pos++] = arr1[i1];
        i1++;
    }

    // Draw the remaining elements from arr2 if necessary
    while (i2 < len2) {
        dest_arr[dest_pos++] = arr2[i2];
        i2++;
    }

    // Return the number of elements we added to the final array
    return dest_pos;
}

int main(void) {
    int arr1[] = {1, 12, 15, 22, 30, 38, 42, 50, 60, 70, 80, 90, 100};
    int arr2[] = {2, 4, 8, 10, 12, 14, 16};
    int len1 = ARRAY_SIZE(arr1);
    int len2 = ARRAY_SIZE(arr2);

    printf("A: ");
    print_arr(arr1, len1);

    printf("B: ");
    print_arr(arr2, len2);

    putchar('\n');

    int *arr_merged = malloc(sizeof(*arr_merged) * (len1 + len2));
    int len_merged = merge_arrays(arr_merged, len1 + len2, arr1, len1, arr2, len2);

    printf("Merged: ");
    print_arr(arr_merged, len_merged);

    return 0;
}
