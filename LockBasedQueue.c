#include "LockBasedQueue.h"
#include <stdlib.h>

void init_lock_based_queue(LockBasedQueue *queue) {
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = NULL;
    temp->next = NULL;
    queue->head = queue->tail = temp;
    pthread_mutex_init(&queue->head_mutex, NULL);
    pthread_mutex_init(&queue->tail_mutex, NULL);
    pthread_cond_init(&queue->not_empty_cond, NULL);
}

void enqueue_lock_based_queue(LockBasedQueue *queue, void *data) {
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = data;
    temp->next = NULL;

    pthread_mutex_lock(&queue->tail_mutex);
    queue->tail->next = temp;
    queue->tail = temp;
    pthread_mutex_unlock(&queue->tail_mutex);

    pthread_cond_signal(&queue->not_empty_cond);
}

void* dequeue_lock_based_queue(LockBasedQueue *queue) {
    pthread_mutex_lock(&queue->head_mutex);
    while (queue->head->next == NULL) {
        pthread_cond_wait(&queue->not_empty_cond, &queue->head_mutex);
    }

    Node *temp = queue->head;
    void *data = temp->next->data;
    queue->head = queue->head->next;
    pthread_mutex_unlock(&queue->head_mutex);

    free(temp);
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
    pthread_cond_destroy(&queue->not_empty_cond);
}