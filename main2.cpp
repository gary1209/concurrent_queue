#include <iostream>
#include <omp.h>
#include "cameron-lock-free/concurrentqueue.h" // Adjust the path as necessary

#define NUM_THREADS 16
#define OPERATIONS_PER_THREAD 100000

int main() {
    moodycamel::ConcurrentQueue<int*> queue;

    // Start timing the operations
    double start_time = omp_get_wtime();

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();

        for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
            // Enqueue and Dequeue operations
            int *data = new int(thread_id * OPERATIONS_PER_THREAD + i);

            queue.enqueue(data);

            int* dequeued_data;
            if (queue.try_dequeue(dequeued_data)) {
                delete dequeued_data;
            }
        }
    }

    // End timing the operations
    double end_time = omp_get_wtime();

    std::cout << "Total time for high contention scenario: " << (end_time - start_time) << " seconds using lock-free queue\n";

    return 0;
}
