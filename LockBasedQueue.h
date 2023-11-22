#ifndef LOCK_BASED_QUEUE_H
#define LOCK_BASED_QUEUE_H

#include <pthread.h>

typedef struct Node {
    void *data;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    pthread_mutex_t head_mutex;
    pthread_mutex_t tail_mutex;
} LockBasedQueue;

void init_lock_based_queue(LockBasedQueue *queue);
void enqueue_lock_based_queue(LockBasedQueue *queue, void *data);
void* dequeue_lock_based_queue(LockBasedQueue *queue);
void destroy_lock_based_queue(LockBasedQueue *queue);

#endif // LOCK_BASED_QUEUE_H