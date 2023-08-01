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
        Debug::printf("*** set\n");
        value = v;
        ready.set(true);
        auto next = waitingQ.remove();
        //waitingQ.remove_all(); 
        while(next != nullptr){
            if(next->priority == 1){
                readyQ1.add(next);
            } else if(next ->priority == 2) {
                readyQ2.add(next);
            } else if(next ->priority == 3) {
                readyQ3.add(next);
            } else if(next ->priority == 4) {
                readyQ4.add(next);
            }
            next = waitingQ.remove();
        }
        lock.unlock();
        
        
    }

    T get() {
        
        lock.lock();
        Debug::printf("*** get\n");
        if(!ready.get()){
            Debug::printf("*** blocking\n");
            block(&waitingQ, &lock);
            lock.lock();
        }
        lock.unlock();
        return value;
    }
};

#endif

