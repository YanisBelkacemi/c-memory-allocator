#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "../headers/allocator.h"

#define ITERATIONS 1000000
#define BLOCK_SIZE 64

static double elapsed_ms(clock_t start, clock_t end)
{
    return ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
}

static void benchmark_malloc_free(void)
{
    clock_t start = clock();

    for (size_t i = 0; i < ITERATIONS; i++) {
        void *ptr = my_malloc(BLOCK_SIZE);

        if (ptr == NULL) {
            printf("my_malloc failed\n");
            return;
        }

        my_free(ptr);
    }

    clock_t end = clock();

    printf("my_malloc + my_free: %.3f ms\n",
           elapsed_ms(start, end));
}

static void benchmark_system_malloc_free(void)
{
    clock_t start = clock();

    for (size_t i = 0; i < ITERATIONS; i++) {
        void *ptr = malloc(BLOCK_SIZE);

        if (ptr == NULL) {
            printf("malloc failed\n");
            return;
        }

        free(ptr);
    }

    clock_t end = clock();

    printf("malloc + free:       %.3f ms\n",
           elapsed_ms(start, end));
}


static void benchmark_realloc(void)
{
    clock_t start = clock();

    for (size_t i = 0; i < ITERATIONS; i++) {
        void *ptr = my_malloc(32);

        if (ptr == NULL) {
            printf("my_malloc failed\n");
            return;
        }

        ptr = my_realloc(ptr, 128);

        if (ptr == NULL) {
            printf("my_realloc failed\n");
            return;
        }

        my_free(ptr);
    }

    clock_t end = clock();

    printf("my_realloc:          %.3f ms\n",
           elapsed_ms(start, end));
}

int main(void)
{
    printf("=== MEMORY ALLOCATOR BENCHMARK ===\n\n");

    malloc_init();
    benchmark_malloc_free();
    malloc_destroy();

    benchmark_system_malloc_free();

    malloc_init();
    benchmark_realloc();
    malloc_destroy();

    return 0;
}