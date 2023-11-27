// LockFreeQueue1.h
#ifndef LOCK_FREE_QUEUE_1_H
#define LOCK_FREE_QUEUE_1_H

#include <atomic>
#include <iostream>

template<typename T>
class LockFreeQueue {
private:
    struct Node {
        T data;
        std::atomic<Node*> next;

        Node() : next(nullptr) {}
        Node(T value) : data(value), next(nullptr) {}
    };

    class NodePool {
    public:
        NodePool() {}

        Node* allocateNode(T value) {
            // Implement dynamic allocation logic here
            // Return a new Node with the given value
            return new Node(value);
        }

        void deallocateNode(Node* node) {
            // Implement deallocation logic here
            // Recycle or delete the node as appropriate
            delete node;
        }
    };

    NodePool pool;
    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    LockFreeQueue() {
        Node* dummyNode = pool.allocateNode(T());
        head.store(dummyNode, std::memory_order_relaxed);
        tail.store(dummyNode, std::memory_order_relaxed);
    }

    ~LockFreeQueue() {
        // Implement cleanup logic here
        // Clean up all nodes starting from head
        Node* current = head.load(std::memory_order_relaxed);
        while (current != nullptr) {
            Node* next = current->next.load(std::memory_order_relaxed);
            pool.deallocateNode(current);
            current = next;
        }
    }

    void push(T value) {
        Node* newNode = pool.allocateNode(value);
        Node* oldTail;
        do {
            oldTail = tail.load(std::memory_order_acquire);
        } while (!tail.compare_exchange_weak(oldTail, newNode, std::memory_order_release, std::memory_order_relaxed));

        // After successfully swapping the tail, set the next pointer of the old tail to the new node
        oldTail->next.store(newNode, std::memory_order_release);
    }

    bool try_pop(T& value) {
        Node* oldHead;
        while (oldHead != nullptr) {
            Node* next = oldHead->next.load(std::memory_order_relaxed);

            // Check if head is still valid and attempt to move head to the next node
            if (head.compare_exchange_weak(oldHead, next, std::memory_order_release, std::memory_order_relaxed)) {
                if (oldHead == tail.load(std::memory_order_acquire)) {
                    // We have popped the last element, so update tail to nullptr
                    tail.store(nullptr, std::memory_order_release);
                }
                // Copy data from the node before deleting it
                value = oldHead->data;
                // Safe deletion of old_head (consider using a safer memory reclamation method here)
                pool.deallocateNode(oldHead);
                return true;
            } else {
                // CAS failed, retry with the updated head
                oldHead = head.load(std::memory_order_acquire);
            }
        }
        return false;
    }
};

#endif // LOCK_FREE_QUEUE_1_H
