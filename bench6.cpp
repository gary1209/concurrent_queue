#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "LockFreeQueue2.h" // Adjust the path as necessary

#define QUEUE_NAME "LockFreeQueue2"
#define NUM_THREADS 8
#define OPERATIONS_PER_THREAD 200000
#define LOW_CONTENTION_PROBABILITY 5

bool performOperationWithLowContentionProbability() {
    return (rand() % 100) < LOW_CONTENTION_PROBABILITY;
}

void performEnqueue(LockFreeQueue<int*>& queue, int thread_id, bool lowContention) {
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        if (lowContention && !performOperationWithLowContentionProbability()) continue;

        int *data = new int(thread_id * OPERATIONS_PER_THREAD + i);
        queue.push(data);
    }
}

void performDequeue(LockFreeQueue<int*>& queue, int thread_id, bool lowContention) {
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        if (lowContention && !performOperationWithLowContentionProbability()) continue;

        int* dequeued_data;
        if (queue.try_pop(dequeued_data)) {
            delete dequeued_data; // Manually delete the dequeued data
        }
    }
}

void performMixedOperations(LockFreeQueue<int*>& queue, int thread_id, bool lowContention) {
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        if (lowContention && !performOperationWithLowContentionProbability()) continue;

        if (i % 2 == 0) {
            int* data = new int(thread_id * OPERATIONS_PER_THREAD + i);
            queue.push(data);
        } else {
            int* dequeued_data;
            if (queue.try_pop(dequeued_data)) {
                delete dequeued_data; // Manually delete the dequeued data
            }
        }
    }
}

void runScenario(void (*operation)(LockFreeQueue<int*>&, int, bool), LockFreeQueue<int*>& queue, const char* scenarioName, bool lowContention) {
    double start_time = omp_get_wtime();

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        operation(queue, thread_id, lowContention);
    }

    double end_time = omp_get_wtime();
    printf("%s - %s: %f seconds\n", scenarioName, lowContention ? "Low Contention" : "High Contention", end_time - start_time);
}

void performSPMC(LockFreeQueue<int*>& queue, bool lowContention, const char* scenarioName) {
    double start_time = omp_get_wtime();

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();

        if (thread_id == 0) {
            // Single Producer
            for (int i = 0; i < OPERATIONS_PER_THREAD * (NUM_THREADS - 1); ++i) {
                if (lowContention && !performOperationWithLowContentionProbability()) continue;

                int* data = new int(i);
                queue.push(data);
            }
        } else {
            // Multiple Consumers
            for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
                if (lowContention && !performOperationWithLowContentionProbability()) continue;

                int* dequeued_data = nullptr; // Create a pointer to receive dequeued data
                if (queue.try_pop(dequeued_data)) {
                    delete dequeued_data; // Delete the dequeued data if try_pop is successful
                }
            }
        }
    }

    double end_time = omp_get_wtime();
    printf("%s - %s: %f seconds\n", scenarioName, lowContention ? "Low Contention" : "High Contention", end_time - start_time);
}

void performSCMP(LockFreeQueue<int*>& queue, bool lowContention, const char* scenarioName) {
    double start_time = omp_get_wtime();

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();

        if (thread_id == 0) {
            // Single Consumer
            for (int i = 0; i < OPERATIONS_PER_THREAD * (NUM_THREADS - 1); ++i) {
                if (lowContention && !performOperationWithLowContentionProbability()) continue;

                int* dequeued_data = nullptr; // Create a pointer to receive dequeued data
                if (queue.try_pop(dequeued_data)) {
                    delete dequeued_data; // Delete the dequeued data if try_pop is successful
                }
            }
        } else {
            // Multiple Producers
            for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
                if (lowContention && !performOperationWithLowContentionProbability()) continue;

                int* data = new int(thread_id * OPERATIONS_PER_THREAD + i);
                queue.push(data);
            }
        }
    }

    double end_time = omp_get_wtime();
    printf("%s - %s: %f seconds\n", scenarioName, lowContention ? "Low Contention" : "High Contention", end_time - start_time);
}

int main() {
    printf("Benchmark Configuration for %s:\n", QUEUE_NAME);
    printf("Number of Threads: %d\n", NUM_THREADS);
    printf("Operations per Thread: %d\n", OPERATIONS_PER_THREAD);
    printf("Low Contention Probability: %d%%\n\n", LOW_CONTENTION_PROBABILITY);

    srand(time(NULL)); // Seed for low contention probability
    LockFreeQueue<int*> queue;

    // High and Low Contention Scenarios
    runScenario(performEnqueue, queue, "High Contention - Enqueue Performance (MPMC)", false);
    runScenario(performDequeue, queue, "High Contention - Dequeue Performance (MPMC)", false);
    runScenario(performMixedOperations, queue, "High Contention - Concurrent Performance (MPMC)", false);

    runScenario(performEnqueue, queue, "Low Contention - Enqueue Performance (MPMC)", true);
    runScenario(performDequeue, queue, "Low Contention - Dequeue Performance (MPMC)", true);
    runScenario(performMixedOperations, queue, "Low Contention - Concurrent Performance (MPMC)", true);

    // SPMC and SCMP Scenarios
    printf("\n--- Single Producer, Multiple Consumer Scenarios ---\n");
    performSPMC(queue, false, "SPMC High Contention"); // High Contention
    performSPMC(queue, true, "SPMC Low Contention");   // Low Contention

    printf("\n--- Single Consumer, Multiple Producer Scenarios ---\n");
    performSCMP(queue, false, "SCMP High Contention"); // High Contention
    performSCMP(queue, true, "SCMP Low Contention");   // Low Contention

    return 0;
}