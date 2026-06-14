/*
 * Reads N, allocates an array of N random integers on the heap,
 * splits it into 4 approximately-equal parts, and uses 4 worker
 * threads to compute the total sum. Each thread adds its partial
 * sum to a global `sum` variable protected by a mutex.
 *
 * Build:  gcc -Wall -O2 -pthread sum_threads.c -o sum_threads
 * Run:    ./sum_threads
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 4

long long sum = 0;                                  /* global result */
pthread_mutex_t sum_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Each worker gets the slice [start, end) of the shared array. */
typedef struct {
    int *array;
    int  start;
    int  end;
} part_t;

void *worker(void *arg)
{
    part_t *p = (part_t *)arg;
    long long partial = 0;

    for (int i = p->start; i < p->end; i++)
        partial += p->array[i];

    /* Protect the shared variable from concurrent modification. */
    pthread_mutex_lock(&sum_mutex);
    sum += partial;
    pthread_mutex_unlock(&sum_mutex);

    return NULL;
}

int main(void)
{
    int N;

    printf("Enter N: ");
    if (scanf("%d", &N) != 1 || N < NUM_THREADS) {
        fprintf(stderr, "Please enter an integer N >= %d.\n", NUM_THREADS);
        return 1;
    }

    /* Allocate the array on the heap. */
    int *array = malloc((size_t)N * sizeof(int));
    if (array == NULL) {
        perror("malloc");
        return 1;
    }

    /* Fill it with random integers (0..99 to keep printing readable). */
    srand((unsigned)time(NULL));
    for (int i = 0; i < N; i++)
        array[i] = rand() % 100;

    /* Split into 4 approximately-equal parts. */
    pthread_t threads[NUM_THREADS];
    part_t    parts[NUM_THREADS];

    int base = N / NUM_THREADS;     /* minimum size of each part */
    int rem  = N % NUM_THREADS;     /* leftover spread over first parts */
    int start = 0;

    for (int t = 0; t < NUM_THREADS; t++) {
        int count = base + (t < rem ? 1 : 0);   /* balance the remainder */
        parts[t].array = array;
        parts[t].start = start;
        parts[t].end   = start + count;
        start += count;
    }

    /* Main thread prints the content of the array. */
    printf("Array contents (%d elements):\n", N);
    for (int i = 0; i < N; i++)
        printf("%d%s", array[i], (i + 1 < N) ? " " : "\n");

    /* Create the worker threads. */
    for (int t = 0; t < NUM_THREADS; t++) {
        if (pthread_create(&threads[t], NULL, worker, &parts[t]) != 0) {
            perror("pthread_create");
            free(array);
            return 1;
        }
    }

    /* Join all workers. */
    for (int t = 0; t < NUM_THREADS; t++)
        pthread_join(threads[t], NULL);

    /* All threads finished: print the result. */
    printf("Sum = %lld\n", sum);

    free(array);
    pthread_mutex_destroy(&sum_mutex);
    return 0;
}
