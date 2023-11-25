// LockFreeQueue.h
#ifndef LOCK_FREE_QUEUE_H
#define LOCK_FREE_QUEUE_H

#include <iostream>
#include <atomic>
#include <memory>

template<typename T>
class LockFreeQueue {
private:
    // The Node structure represents each element in the queue.
    struct Node {
        std::shared_ptr<T> data; // Smart pointer for automatic memory management.
        std::atomic<Node*> next; // Atomic pointer to the next node for thread-safe access.

        Node() : next(nullptr) { } // Constructor initializes next to nullptr.
    };

    // Head and tail pointers for the queue, using atomic for thread-safe access.
    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    // Constructor initializes the queue with a dummy node.
    LockFreeQueue() : head(new Node), tail(head.load()) { }

    // Deleting copy constructor and assignment operator to prevent copying.
    LockFreeQueue(const LockFreeQueue& other) = delete;
    LockFreeQueue& operator=(const LockFreeQueue& other) = delete;

    // Destructor to clean up the nodes.
    ~LockFreeQueue() {
        // Ensure all operations on the queue are finished before destroying.
        while (Node* const old_head = head.load(std::memory_order_relaxed)) {
            head.store(old_head->next.load(std::memory_order_relaxed), std::memory_order_relaxed);
            delete old_head; // Only delete if you're sure no other threads are accessing it.
        }
    }

    // try_pop tries to pop an element from the head of the queue.
    std::shared_ptr<T> try_pop() {
        Node* old_head = head.load(std::memory_order_acquire);
        while (old_head != nullptr) {
            Node* next = old_head->next.load(std::memory_order_acquire);

            // Check if head is still valid and attempt to move head to the next node
            if (head.compare_exchange_strong(old_head, next, std::memory_order_release, std::memory_order_relaxed)) {
                if (old_head == tail.load(std::memory_order_acquire)) {
                    // We have popped the last element, so update tail to nullptr
                    tail.store(nullptr, std::memory_order_release);
                }

                // Copy data from the node before deleting it
                std::shared_ptr<T> res = old_head->data;
                // if (res) {
                //     std::cout << "Popping: " << *res << '\n';
                // } else {
                //     std::cout << "Popping: null\n";
                // }

                // Safe deletion of old_head (consider using a safer memory reclamation method here)
                delete old_head;
                return res;
            } else {
                // CAS failed, retry with the updated head
                old_head = head.load(std::memory_order_acquire);
            }
        }
        return std::shared_ptr<T>(); // Queue is empty
    }

    void push(T new_value) {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
        Node* p = new Node; 
        p->data = new_data; // Set the data for the new node

        Node* old_tail;
        do {
            old_tail = tail.load(std::memory_order_acquire);
            p->next.store(nullptr, std::memory_order_relaxed); // Initially, the next pointer of the new node is null
            // std::cout << "Pushing: " << *new_data << '\n';
        } while (!tail.compare_exchange_weak(old_tail, p, std::memory_order_release, std::memory_order_relaxed));

        // After successfully swapping the tail, set the next pointer of the old tail to the new node
        old_tail->next.store(p, std::memory_order_release); 
    }

};

#endif // LOCK_FREE_QUEUE_H
