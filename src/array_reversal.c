#include <stdio.h>

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(*arr)

void swap(int *arr, int a, int b) {
    int temp_a = arr[a];
    arr[a] = arr[b];
    arr[b] = temp_a;
}

void reverse(int *arr, int len) {
    int left = 0;
    int right = len - 1;

    while (left < right) {
        swap(arr, left, right);

        left++;
        right--;
    }
}

void print_arr(int *arr, int len) {
    for (int i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }

    putchar('\n');
}

int main(void) {
    int arr[] = {1, 2, 3, 4, 5, 6, 7};
    int len = ARRAY_SIZE(arr);

    printf("Original: ");
    print_arr(arr, len);

    reverse(arr, len);

    printf("Reversed: ");
    print_arr(arr, len);

    return 0;
}
