#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "cameron-lock-free/concurrentqueue.h" // Adjust the path as necessary

#define QUEUE_NAME "Cameron-Lock-Free-Queue"
#define NUM_THREADS 8
#define OPERATIONS_PER_THREAD 200000
#define LOW_CONTENTION_PROBABILITY 5 // Probability percentage for operation execution in low contention

bool performOperationWithLowContentionProbability() {
    return (rand() % 100) < LOW_CONTENTION_PROBABILITY;
}

void performEnqueue(moodycamel::ConcurrentQueue<int*>& queue, int thread_id, bool lowContention) {
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        if (lowContention && !performOperationWithLowContentionProbability()) continue;

        int *data = new int(thread_id * OPERATIONS_PER_THREAD + i);
        queue.enqueue(data);
    }
}

void performDequeue(moodycamel::ConcurrentQueue<int*>& queue, int thread_id, bool lowContention) {
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        if (lowContention && !performOperationWithLowContentionProbability()) continue;

        int *dequeued_data;
        if (queue.try_dequeue(dequeued_data)) {
            delete dequeued_data;
        }
    }
}

void performMixedOperations(moodycamel::ConcurrentQueue<int*>& queue, int thread_id, bool lowContention) {
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        if (lowContention && !performOperationWithLowContentionProbability()) continue;

        int *data;
        if (i % 2 == 0) {
            data = new int(thread_id * OPERATIONS_PER_THREAD + i);
            queue.enqueue(data);
        } else {
            if (queue.try_dequeue(data)) {
                delete data;
            }
        }
    }
}

void runScenario(void (*operation)(moodycamel::ConcurrentQueue<int*>&, int, bool), moodycamel::ConcurrentQueue<int*>& queue, const char* scenarioName, bool lowContention) {
    double start_time = omp_get_wtime();

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        operation(queue, thread_id, lowContention);
    }

    double end_time = omp_get_wtime();
    printf("%s - %s: %f seconds\n", scenarioName, lowContention ? "Low  Contention" : "High Contention", end_time - start_time);
}

void performSPMC(moodycamel::ConcurrentQueue<int*>& queue, bool lowContention, const char* scenarioName) {
    double start_time = omp_get_wtime();

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();

        if (thread_id == 0) {
            // Single Producer
            for (int i = 0; i < OPERATIONS_PER_THREAD * (NUM_THREADS - 1); ++i) {
                if (lowContention && !performOperationWithLowContentionProbability()) continue;

                int* data = new int(i);
                queue.enqueue(data);
            }
        } else {
            // Multiple Consumers
            for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
                if (lowContention && !performOperationWithLowContentionProbability()) continue;

                int* dequeued_data;
                if (queue.try_dequeue(dequeued_data)) {
                    delete dequeued_data;
                }
            }
        }
    }

    double end_time = omp_get_wtime();
    printf("%s - %s: %f seconds\n", scenarioName, lowContention ? "Low  Contention" : "High Contention", end_time - start_time);
}

void performSCMP(moodycamel::ConcurrentQueue<int*>& queue, bool lowContention, const char* scenarioName) {
    double start_time = omp_get_wtime();

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();

        if (thread_id == 0) {
            // Single Consumer
            for (int i = 0; i < OPERATIONS_PER_THREAD * (NUM_THREADS - 1); ++i) {
                if (lowContention && !performOperationWithLowContentionProbability()) continue;

                int* dequeued_data;
                if (queue.try_dequeue(dequeued_data)) {
                    delete dequeued_data;
                }
            }
        } else {
            // Multiple Producers
            for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
                if (lowContention && !performOperationWithLowContentionProbability()) continue;

                int* data = new int(thread_id * OPERATIONS_PER_THREAD + i);
                queue.enqueue(data);
            }
        }
    }

    double end_time = omp_get_wtime();
    printf("%s - %s: %f seconds\n", scenarioName, lowContention ? "Low  Contention" : "High Contention", end_time - start_time);
}


int main() {
    printf("Benchmark Configuration for %s:\n", QUEUE_NAME);
    printf("Number of Threads: %d\n", NUM_THREADS);
    printf("Operations per Thread: %d\n", OPERATIONS_PER_THREAD);
    printf("Low Contention Probability: %d%%\n\n", LOW_CONTENTION_PROBABILITY);

    srand(time(NULL)); // Seed for low contention probability
    moodycamel::ConcurrentQueue<int*> queue;

    // High Contention Scenarios
    runScenario(performEnqueue, queue, "High Contention - Enqueue Performance (MPMC)", false);
    runScenario(performDequeue, queue, "High Contention - Dequeue Performance (MPMC)", false);
    runScenario(performMixedOperations, queue, "High Contention - Concurrent Performance (MPMC)", false);

    // Low Contention Scenarios
    runScenario(performEnqueue, queue, "Low  Contention - Enqueue Performance (MPMC)", true);
    runScenario(performDequeue, queue, "Low  Contention - Dequeue Performance (MPMC)", true);
    runScenario(performMixedOperations, queue, "Low  Contention - Concurrent Performance (MPMC)", true);

    // SPMC and SCMP Scenarios
    printf("\n--- Single Producer, Multiple Consumer Scenarios ---\n");
    performSPMC(queue, false, "SPMC High Contention"); // High Contention
    performSPMC(queue, true, "SPMC Low  Contention");  // Low Contention

    printf("\n--- Single Consumer, Multiple Producer Scenarios ---\n");
    performSCMP(queue, false, "SCMP High Contention"); // High Contention
    performSCMP(queue, true, "SCMP Low  Contention");   // Low Contention

    return 0;
}
