#include "LockBasedQueue.h"
#include <stdlib.h>
#include <pthread.h>

void init_lock_based_queue(LockBasedQueue *queue) {
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = NULL;
    temp->next = NULL;
    queue->head = queue->tail = temp;
    pthread_mutex_init(&queue->head_mutex, NULL);
    pthread_mutex_init(&queue->tail_mutex, NULL);
}

void enqueue_lock_based_queue(LockBasedQueue *queue, void *data) {
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = data;
    temp->next = NULL;

    pthread_mutex_lock(&queue->tail_mutex);
    queue->tail->next = temp;
    queue->tail = temp;
    pthread_mutex_unlock(&queue->tail_mutex);
}

void* dequeue_lock_based_queue(LockBasedQueue *queue) {
    void *data = NULL;

    pthread_mutex_lock(&queue->head_mutex);
    if (queue->head->next != NULL) {
        Node *temp = queue->head;
        queue->head = queue->head->next;
        data = queue->head->data;
        free(temp);
    }
    pthread_mutex_unlock(&queue->head_mutex);

    return data;
}

void destroy_lock_based_queue(LockBasedQueue *queue) {
    while (queue->head != NULL) {
        Node *temp = queue->head;
        queue->head = queue->head->next;
        free(temp);
    }

    pthread_mutex_destroy(&queue->head_mutex);
    pthread_mutex_destroy(&queue->tail_mutex);
}