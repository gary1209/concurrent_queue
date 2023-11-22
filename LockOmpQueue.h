#ifndef LOCK_BASED_QUEUE_OPENMP_H
#define LOCK_BASED_QUEUE_OPENMP_H

#include <omp.h>

typedef struct Node {
    void *data;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    omp_lock_t head_lock;
    omp_lock_t tail_lock;
} LockOmpQueue;

void init_lock_based_queue(LockOmpQueue *queue);
void enqueue_lock_based_queue(LockOmpQueue *queue, void *data);
void* dequeue_lock_based_queue(LockOmpQueue *queue);
void destroy_lock_based_queue(LockOmpQueue *queue);

#endif