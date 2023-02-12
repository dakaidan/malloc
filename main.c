#include <stdio.h>

#include "malloc.h"

void testMalloc() {
    int *arr = malloc(10 * sizeof(int));
    arr[0] = 10;
    arr[9] = 20;
    assert(arr[0] == 10);
    assert(arr[9] == 20);
    free(arr);
}

void testRealloc() {
    int *arr = malloc(10 * sizeof(int));
    arr[0] = 10;
    arr[9] = 20;
    assert(arr[0] == 10);
    assert(arr[9] == 20);
    arr = realloc(arr, 100 * sizeof(int));
    arr[99] = 30;
    assert(arr[99] == 30);
    free(arr);
}

void testCalloc() {
    int *arr = calloc(10, sizeof(int));
    assert(arr[0] == 0);
    assert(arr[9] == 0);
    free(arr);
}


int main() {
    printf("Hello, Malloc!\n");

    testMalloc();
    testRealloc();
    testCalloc();

    return 0;
}
