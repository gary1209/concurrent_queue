// LockFreeQueue2.h
#ifndef LOCK_FREE_QUEUE_2_H
#define LOCK_FREE_QUEUE_2_H

#include <atomic>
#include <iostream>
#include <vector>
#include <mutex>

template<typename T>
class LockFreeQueue {
private:
    static const int BLOCK_SIZE = 1024; // Example block size

    struct Node {
        T data;
        std::atomic<Node*> next;
        std::atomic<bool> popped{false}; // Flag to indicate if the node has been popped

        Node() : next(nullptr) {}
        Node(T value) : data(value), next(nullptr) {}
    };

    struct Block {
        Node nodes[BLOCK_SIZE];
    };

    class BlockAllocator {
    public:
        BlockAllocator() {
            // Allocate the first block
            allocateBlock();
        }

        ~BlockAllocator() {
            for (auto block : blocks) {
                delete block;
            }
        }

        Node* allocateNode(T value) {
            while (true) {
                int index = currentIndex.load(std::memory_order_relaxed);
                if (index >= BLOCK_SIZE) {
                    std::lock_guard<std::mutex> lock(mutex); // Protect block allocation
                    if (currentIndex.load(std::memory_order_acquire) >= BLOCK_SIZE) {
                        allocateBlock(); // Allocate a new block if needed
                    }
                    continue;
                }

                if (currentIndex.compare_exchange_weak(index, index + 1)) {
                    Node* newNode = new (&currentBlock->nodes[index]) Node(value);
                    return newNode;
                }
            }
        }

    private:
        std::vector<Block*> blocks;
        Block* currentBlock;
        std::atomic<int> currentIndex = 0;
        std::mutex mutex; // Mutex to protect block allocation

        void allocateBlock() {
            currentBlock = new Block();
            blocks.push_back(currentBlock);
            currentIndex.store(0, std::memory_order_release);
        }
    };

    BlockAllocator allocator;
    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    LockFreeQueue() {
        Node* dummyNode = allocator.allocateNode(T());
        head.store(dummyNode, std::memory_order_relaxed);
        tail.store(dummyNode, std::memory_order_relaxed);
    }

    ~LockFreeQueue() {
        // All nodes are cleaned up by the BlockAllocator's destructor
    }

    void push(T value) {
        Node* newNode = allocator.allocateNode(value);
        Node* oldTail;
        do {
            oldTail = tail.load(std::memory_order_acquire);
        } while (!tail.compare_exchange_weak(oldTail, newNode, std::memory_order_release, std::memory_order_relaxed));

        oldTail->next.store(newNode, std::memory_order_release);
    }

    bool try_pop(T& value) {
        Node* oldHead;
        while ((oldHead = head.load(std::memory_order_acquire)) != nullptr) {
            Node* next = oldHead->next.load(std::memory_order_relaxed);

            if (oldHead == tail.load(std::memory_order_acquire)) {
                return false; // Queue is empty
            }

            if (oldHead->popped.load(std::memory_order_relaxed)) {
                continue; // This node is already popped by another thread
            }

            if (head.compare_exchange_weak(oldHead, next, std::memory_order_release, std::memory_order_relaxed)) {
                oldHead->popped.store(true, std::memory_order_relaxed);
                value = oldHead->data;
                // Deallocate node here or mark for future deallocation
                return true;
            }
        }
        return false;
    }

};

#endif // LOCK_FREE_QUEUE_2_H
