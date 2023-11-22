#include "LockOmpQueue.h"
#include <stdlib.h>

void init_lock_based_queue(LockOmpQueue *queue) {
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = NULL;
    temp->next = NULL;
    queue->head = queue->tail = temp;
    omp_init_lock(&queue->head_lock);
    omp_init_lock(&queue->tail_lock);
}

void enqueue_lock_based_queue(LockOmpQueue *queue, void *data) {
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = data;
    temp->next = NULL;

    omp_set_lock(&queue->tail_lock);
    queue->tail->next = temp;
    queue->tail = temp;
    omp_unset_lock(&queue->tail_lock);
}

void* dequeue_lock_based_queue(LockOmpQueue *queue) {
    void *data = NULL;

    omp_set_lock(&queue->head_lock);
    if (queue->head->next != NULL) {
        Node *temp = queue->head;
        queue->head = queue->head->next;
        data = queue->head->data;
        free(temp);
    }
    omp_unset_lock(&queue->head_lock);

    return data;
}

void destroy_lock_based_queue(LockOmpQueue *queue) {
    while (queue->head != NULL) {
        Node *temp = queue->head;
        queue->head = queue->head->next;
        free(temp);
    }

    omp_destroy_lock(&queue->head_lock);
    omp_destroy_lock(&queue->tail_lock);
}
