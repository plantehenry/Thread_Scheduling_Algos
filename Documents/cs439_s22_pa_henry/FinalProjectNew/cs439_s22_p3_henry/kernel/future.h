#ifndef _future_h_
#define _future_h_

#include "debug.h"
#include "threads.h"

template <typename T>
class Future {
        Atomic<bool> ready;
        T value;
        InterruptSafeLock lock;
        Queue<TCB, InterruptSafeLock> waitingQ;
public:

    // is this right initlization?
    // specifically default constructors 
    // and value(nullptr) -> can I just not initilie it?
    Future(): ready(false), lock(), waitingQ() {
    }

    // Can't copy a future
    Future(const Future&) = delete;

    // do I need to import readyQ somehow?
    // prolly
    void set(T v) {
        lock.lock();
        value = v;
        ready.set(true);
        auto next = waitingQ.remove();
        //waitingQ.remove_all(); 
        while(next != nullptr){
            readyQ.add(next);
            next = waitingQ.remove();
        }
        lock.unlock();
        
        
    }

    T get() {
        lock.lock();
        if(!ready.get()){
            block(&waitingQ, &lock);
            lock.lock();
        }
        lock.unlock();
        return value;
    }
};

#endif

