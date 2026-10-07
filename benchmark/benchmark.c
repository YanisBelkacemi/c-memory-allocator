
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "../headers/allocator.h"

#define ITERATIONS 1000000

/* Allocator heap size */
#define HEAP_SIZE 4096

/* Keep simultaneous allocations safely below 4096 bytes. */
#define BLOCK_COUNT 32
#define BATCHES 100000

static double elapsed_ms(clock_t start, clock_t end)
{
    return ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
}


/* ============================================================
 * Basic malloc/free
 * ============================================================ */

static void benchmark_malloc_free(void)
{
    clock_t start = clock();

    for (size_t i = 0; i < ITERATIONS; i++) {
        void *ptr = my_malloc(64);

        if (ptr == NULL) {
            printf("my_malloc failed\n");
            return;
        }

        my_free(ptr);
    }

    clock_t end = clock();

    printf("my_malloc + my_free:       %.3f ms\n",
           elapsed_ms(start, end));
}

static void benchmark_system_malloc_free(void)
{
    clock_t start = clock();

    for (size_t i = 0; i < ITERATIONS; i++) {
        void *ptr = malloc(64);

        if (ptr == NULL) {
            printf("malloc failed\n");
            return;
        }

        free(ptr);
    }

    clock_t end = clock();

    printf("malloc + free:             %.3f ms\n",
           elapsed_ms(start, end));
}


/* ============================================================
 * Multiple simultaneous allocations
 *
 * Each batch uses only 32 × 64 bytes.
 * The batch is repeated to make timing measurable.
 * ============================================================ */

static void benchmark_many_allocations(void)
{
    void *blocks[BLOCK_COUNT];

    clock_t start = clock();

    for (size_t batch = 0; batch < BATCHES; batch++) {

        for (size_t i = 0; i < BLOCK_COUNT; i++) {
            blocks[i] = my_malloc(64);

            if (blocks[i] == NULL) {
                printf("my_malloc failed at block %zu\n", i);
                return;
            }
        }

        for (size_t i = 0; i < BLOCK_COUNT; i++) {
            my_free(blocks[i]);
        }
    }

    clock_t end = clock();

    printf("my_many allocations:       %.3f ms\n",
           elapsed_ms(start, end));
}

static void benchmark_system_many_allocations(void)
{
    void *blocks[BLOCK_COUNT];

    clock_t start = clock();

    for (size_t batch = 0; batch < BATCHES; batch++) {

        for (size_t i = 0; i < BLOCK_COUNT; i++) {
            blocks[i] = malloc(64);

            if (blocks[i] == NULL) {
                printf("malloc failed at block %zu\n", i);
                return;
            }
        }

        for (size_t i = 0; i < BLOCK_COUNT; i++) {
            free(blocks[i]);
        }
    }

    clock_t end = clock();

    printf("system many allocations:   %.3f ms\n",
           elapsed_ms(start, end));
}


/* ============================================================
 * Mixed allocation sizes
 * ============================================================ */

static void benchmark_mixed_sizes(void)
{
    static const size_t sizes[] = {
        16, 32, 64, 128, 256
    };

    const size_t count = sizeof(sizes) / sizeof(sizes[0]);

    clock_t start = clock();

    for (size_t i = 0; i < ITERATIONS; i++) {
        size_t size = sizes[i % count];

        void *ptr = my_malloc(size);

        if (ptr == NULL) {
            printf("my_malloc failed\n");
            return;
        }

        my_free(ptr);
    }

    clock_t end = clock();

    printf("my_mixed sizes:            %.3f ms\n",
           elapsed_ms(start, end));
}

static void benchmark_system_mixed_sizes(void)
{
    static const size_t sizes[] = {
        16, 32, 64, 128, 256
    };

    const size_t count = sizeof(sizes) / sizeof(sizes[0]);

    clock_t start = clock();

    for (size_t i = 0; i < ITERATIONS; i++) {
        size_t size = sizes[i % count];

        void *ptr = malloc(size);

        if (ptr == NULL) {
            printf("malloc failed\n");
            return;
        }

        free(ptr);
    }

    clock_t end = clock();

    printf("system mixed sizes:        %.3f ms\n",
           elapsed_ms(start, end));
}


/* ============================================================
 * Fragmentation
 *
 * Allocate 32 blocks, free every other block,
 * then allocate into the holes.
 * ============================================================ */

static void benchmark_fragmentation(void)
{
    void *blocks[BLOCK_COUNT];

    clock_t start = clock();

    for (size_t batch = 0; batch < BATCHES; batch++) {

        for (size_t i = 0; i < BLOCK_COUNT; i++) {
            blocks[i] = my_malloc(64);

            if (blocks[i] == NULL) {
                printf("my_malloc failed at block %zu\n", i);
                return;
            }
        }

        /* Create holes. */
        for (size_t i = 0; i < BLOCK_COUNT; i += 2) {
            my_free(blocks[i]);
            blocks[i] = NULL;
        }

        /* Reuse the holes. */
        for (size_t i = 0; i < BLOCK_COUNT; i += 2) {
            blocks[i] = my_malloc(64);

            if (blocks[i] == NULL) {
                printf("my_malloc failed while filling holes\n");
                return;
            }
        }

        for (size_t i = 0; i < BLOCK_COUNT; i++) {
            my_free(blocks[i]);
        }
    }

    clock_t end = clock();

    printf("my_fragmentation:          %.3f ms\n",
           elapsed_ms(start, end));
}

static void benchmark_system_fragmentation(void)
{
    void *blocks[BLOCK_COUNT];

    clock_t start = clock();

    for (size_t batch = 0; batch < BATCHES; batch++) {

        for (size_t i = 0; i < BLOCK_COUNT; i++) {
            blocks[i] = malloc(64);

            if (blocks[i] == NULL) {
                printf("malloc failed at block %zu\n", i);
                return;
            }
        }

        for (size_t i = 0; i < BLOCK_COUNT; i += 2) {
            free(blocks[i]);
            blocks[i] = NULL;
        }

        for (size_t i = 0; i < BLOCK_COUNT; i += 2) {
            blocks[i] = malloc(64);

            if (blocks[i] == NULL) {
                printf("malloc failed while filling holes\n");
                return;
            }
        }

        for (size_t i = 0; i < BLOCK_COUNT; i++) {
            free(blocks[i]);
        }
    }

    clock_t end = clock();

    printf("system fragmentation:      %.3f ms\n",
           elapsed_ms(start, end));
}


/* ============================================================
 * Realloc
 * ============================================================ */

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

    printf("my_realloc 32 -> 128:      %.3f ms\n",
           elapsed_ms(start, end));
}

static void benchmark_system_realloc(void)
{
    clock_t start = clock();

    for (size_t i = 0; i < ITERATIONS; i++) {
        void *ptr = malloc(32);

        if (ptr == NULL) {
            printf("malloc failed\n");
            return;
        }

        ptr = realloc(ptr, 128);

        if (ptr == NULL) {
            printf("realloc failed\n");
            return;
        }

        free(ptr);
    }

    clock_t end = clock();

    printf("realloc 32 -> 128:         %.3f ms\n",
           elapsed_ms(start, end));
}


/* ============================================================
 * Realloc chain
 * ============================================================ */

static void benchmark_realloc_chain(void)
{
    clock_t start = clock();

    for (size_t i = 0; i < ITERATIONS; i++) {

        void *ptr = my_malloc(32);

        if (ptr == NULL)
            return;

        ptr = my_realloc(ptr, 64);
        if (ptr == NULL)
            return;

        ptr = my_realloc(ptr, 128);
        if (ptr == NULL)
            return;

        ptr = my_realloc(ptr, 256);
        if (ptr == NULL)
            return;

        ptr = my_realloc(ptr, 512);
        if (ptr == NULL)
            return;

        my_free(ptr);
    }

    clock_t end = clock();

    printf("my_realloc chain:          %.3f ms\n",
           elapsed_ms(start, end));
}

static void benchmark_system_realloc_chain(void)
{
    clock_t start = clock();

    for (size_t i = 0; i < ITERATIONS; i++) {

        void *ptr = malloc(32);

        if (ptr == NULL)
            return;

        ptr = realloc(ptr, 64);
        if (ptr == NULL)
            return;

        ptr = realloc(ptr, 128);
        if (ptr == NULL)
            return;

        ptr = realloc(ptr, 256);
        if (ptr == NULL)
            return;

        ptr = realloc(ptr, 512);
        if (ptr == NULL)
            return;

        free(ptr);
    }

    clock_t end = clock();

    printf("system realloc chain:      %.3f ms\n",
           elapsed_ms(start, end));
}


/* ============================================================
 * Main
 * ============================================================ */

int main(void)
{
    printf("=== MEMORY ALLOCATOR BENCHMARK ===\n");
    printf("Heap size: %d bytes\n\n", HEAP_SIZE);

    /* Basic allocation/free */
    malloc_init();
    benchmark_malloc_free();
    malloc_destroy();

    benchmark_system_malloc_free();

    printf("\n");

    /* Multiple allocations */
    malloc_init();
    benchmark_many_allocations();
    malloc_destroy();

    benchmark_system_many_allocations();

    printf("\n");

    /* Mixed sizes */
    malloc_init();
    benchmark_mixed_sizes();
    malloc_destroy();

    benchmark_system_mixed_sizes();

    printf("\n");

    /* Fragmentation */
    malloc_init();
    benchmark_fragmentation();
    malloc_destroy();

    benchmark_system_fragmentation();

    printf("\n");

    /* Realloc */
    malloc_init();
    benchmark_realloc();
    malloc_destroy();

    benchmark_system_realloc();

    printf("\n");

    /* Realloc chain */
    malloc_init();
    benchmark_realloc_chain();
    malloc_destroy();

    benchmark_system_realloc_chain();

    return 0;
}