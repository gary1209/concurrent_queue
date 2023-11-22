#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "LockOmpQueue.h"

#define NUM_THREADS 16  // Adjust based on your system's capabilities
#define OPERATIONS_PER_THREAD 100000

int main() {
    LockOmpQueue queue;
    init_lock_based_queue(&queue);

    // Start timing the operations
    double start_time = omp_get_wtime();

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();

        for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
            // Enqueue and Dequeue operations
            int *data = (int *)malloc(sizeof(int));
            *data = thread_id * OPERATIONS_PER_THREAD + i;

            enqueue_lock_based_queue(&queue, data);

            int *dequeued_data = (int *)dequeue_lock_based_queue(&queue);
            if (dequeued_data != NULL) {
                free(dequeued_data);
            }
        }
    }

    // End timing the operations
    double end_time = omp_get_wtime();

    printf("Total time for high contention scenario: %f seconds using lock-omp queue\n", end_time - start_time);

    destroy_lock_based_queue(&queue);
    return 0;
}