#ifndef _bb_h_
#define _bb_h_

#include "debug.h"
#include "stdint.h"
#include "threads.h"

template <typename T>
class BoundedBuffer {
    InterruptSafeLock lock;
    Queue<TCB, InterruptSafeLock> waitingToPut;
    Queue<TCB, InterruptSafeLock> waitingToGet;
    T * buffer;
    Atomic<int> startIndex;
    Atomic<int> endIndex;
    int size;
    Atomic<int> currentSize;

public:

    // remeber to delete buffer -> or maybe desructor does that for you
    BoundedBuffer(uint32_t N) : lock(), waitingToPut(), waitingToGet(), startIndex(0), endIndex(0), currentSize(0) {
        buffer = new T[N];
        startIndex.set(0);
        endIndex.set(0);
        size = N;
        currentSize.set(0);
    }

    ~BoundedBuffer() {
    }

    BoundedBuffer(const BoundedBuffer&) = delete;

    void put(T t) {
        lock.lock();
        while(currentSize.get() == size){
            block(&waitingToPut, &lock);
            lock.lock();
        }
        buffer[endIndex.get()] = t;
        endIndex.add_fetch(1);
        if(endIndex.get() == size){
            endIndex = 0;
        }
        currentSize.add_fetch(1);
        auto tcb = waitingToGet.remove();
        if(tcb != nullptr){
            readyQ.add(tcb);
        }
        lock.unlock();
    }

    T get() {
        lock.lock();
        while(currentSize.get() == 0){
            block(&waitingToGet, &lock);
            lock.lock();
        }
        T value = buffer[startIndex.get()];
        startIndex.add_fetch(1);
        if(startIndex.get() == size){
            startIndex = 0;
        }
        currentSize.add_fetch(-1);
        auto tcb = waitingToPut.remove();
        if(tcb != nullptr){
            readyQ.add(tcb);
        }
        lock.unlock();
        return value;
    }
        
};

#endif
