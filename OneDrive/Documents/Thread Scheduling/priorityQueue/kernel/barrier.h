#ifndef _barrier_h_
#define _barrier_h_

#include "debug.h"
#include "threads.h"

class Barrier {
    InterruptSafeLock lock;
    Atomic<int> toGo;
    Queue<TCB, InterruptSafeLock> waitingQ;
public:

    Barrier(uint32_t count): lock(),  toGo(count), waitingQ() {
    }

    Barrier(const Barrier&) = delete;
    

    // // do I need to get the lock from threads in order to add to the readyQ?

    // this seems somehwat wasteful
    // each thread will have to go to the waiting queue to see if it is empty or not
    // could put (togo != 0) in if statement but apparently that is dangerous
    void sync() {
        lock.lock();
        toGo.add_fetch(-1);
        if(toGo.get() != 0){
            block(&waitingQ, &lock);
            //lock.lock();
        } else {
            TCB* next = waitingQ.remove();
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
    }
};

class ReusableBarrier {
    InterruptSafeLock lock;
    Atomic<int> toGo;
    Queue<TCB, InterruptSafeLock> waitingQ;
    int n;
public:

    ReusableBarrier(uint32_t count) : lock(),toGo(count), waitingQ(), n(count){}

    ReusableBarrier(const ReusableBarrier&) = delete;

    void sync() {
        lock.lock();
        toGo.add_fetch(-1);
        if(toGo.get() != 0){
            // assumes block will not return if there are no other threads
            block(&waitingQ, &lock);
        } else {
            TCB* next = waitingQ.remove();
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
            // this sorta makes it reusable but I don't think thats what they meant. 
            toGo.set(n);
            lock.unlock();
        }
        
    }
};
        
        
        

#endif

