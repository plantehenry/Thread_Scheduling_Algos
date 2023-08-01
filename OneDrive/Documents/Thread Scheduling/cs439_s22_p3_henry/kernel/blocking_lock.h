#ifndef _blocking_lock_h_
#define _blocking_lock_h_

#include "debug.h"
#include "threads.h"


class BlockingLock {
    InterruptSafeLock thisLock;
    Atomic<bool> taken;
    Queue<TCB, InterruptSafeLock> waitingQ;
public:
    BlockingLock():  thisLock(), taken(false), waitingQ() {

    }


    // can you even use an interrupt safe lock here? 

    // do I need to implement something so block garuntees that you wont return if
    // there is nothing in queue
    void lock() {
        // dummy
        // while (taken.exchange(true))
	    //     yield();
        // dummy

        thisLock.lock();
        if(taken.get()){
            // need to make sure block doesnt just return and actually puts you into queue
            block(&waitingQ, &thisLock);
        } else {
            taken.set(true);
            thisLock.unlock();
        }
        // taken.set(true);
        // thisLock.unlock();
    }

    void unlock() {

        //dummy
        //taken.set(false);
        //dummy
        // //auto was = Interrupts::isDisabled();
        thisLock.lock();
        auto next = waitingQ.remove();
        if(next == nullptr){
            taken.set(false);
        } else{
            readyQ.add(next);
        }
        thisLock.unlock(); 
        
    }

    bool isMine() { return true; }
};



#endif

