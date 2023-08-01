#include "debug.h"
#include "smp.h"
#include "debug.h"
#include "config.h"
#include "machine.h"
#include "threads.h"

// Thread ready queue
// do I need to change spinlock to interupt lock?
// readyQ is a dummy queue
Queue<TCB, InterruptSafeLock> readyQ{};
Queue<TCB, InterruptSafeLock> readyQ1{};
Queue<TCB, InterruptSafeLock> readyQ2{};
Queue<TCB, InterruptSafeLock> readyQ3{};
Queue<TCB, InterruptSafeLock> readyQ4{};
Queue<TCB, InterruptSafeLock> zombieQ{};
Atomic<bool> reaper_enabled = false;

// Using interrupt safe lock not isl
// why would I use isl
// isl enables you to control the itterputing ability better
// inside the lock/critical area
// but doesnt that defeat the point of the interrupt safe lock?
SpinLock myLock{};

// The active thread
// TODO: turn into an array in order to support multiple cores
TCB* active[] = {nullptr, nullptr, nullptr, nullptr};
TCB* stoped[] = {nullptr, nullptr, nullptr, nullptr};
TCB* idle[] = {nullptr, nullptr, nullptr, nullptr};
TCB* dummy[] = {nullptr, nullptr, nullptr, nullptr};

// TODO: you might want to initialize some data structures here
void threadsInit() {
        for(int i = 0; i < 4; i++){
            auto idleT = fakeThread([]{
                while(true){
                    //Debug::printf("*** in idle thread\n");
                    yield();
                }
            });
            idleT->idleOrDummy = true;
            idle[i] = idleT;
            
        }
        for(int i = 0; i < 4; i++){
            auto dummyT = fakeThread([]{
                //Debug::printf("*** in fake thread\n");
            });
            
            active[i] = dummyT;
        }
    
    // thread([]{
    //     while(true){
    //         auto tcb = zombieQ.remove();
    //         while(tcb != nullptr){
    //             //delete tcb;
    //             tcb = zombieQ.remove();
    //         }
    //         yield();
    //     }
    // });
}

// TODO: deal with race conditions
void a_new_thread_starts_here() {
    
    myLock.unlock();
    // restore interrupting ability
    Interrupts::disable();
    auto me = active[SMP::me()];
    // auto deleteTCB = stoped[SMP::me()];
    // if(deleteTCB != nullptr){
    //     stoped[SMP::me()] = nullptr;
    //     Interrupts::restore(false);
    //     delete deleteTCB;
    // }
    //bool was = me->was;
    me->preemptable = true; 
    Interrupts::restore(false);

    // shouldn't all the code above go below the code below?
    // but somehow my code worked before with it like this
    me->do_what_you_need_to_do();
    // If a thread finishes its work, we force it to stop
    stop();
}

extern "C" void everyone_hates_assembly(uint32_t* from, uint32_t* to);


// using interrupt safe lock becasue of what I wrote above

// should their lock be general?
void block(Queue<TCB, InterruptSafeLock> *waitingQ, InterruptSafeLock *thierLock){
    


    // two ideas to deal with needing to not be interupted here
    // one just go ahead and say interupt::diable
    // then reenable after in context switch.
    // but issue need to unlock lock which is spinnig
    // also after yield thread could end up in a_new_thread_starts_here
    // how it know what to reinstate interrupt to?
    // possibly have a flag in tcb that says what interrupt should be reset to?
    // but what if tcb is null? is that possible?

    // other possibilty
    // have a flag in tcb that says not to mess with me while I am in yield
    // but what is tcb is null???? Can that happen? -> I think yes -> when 
    // does this happen how do I deal with it?
    // maybe you just create a quick dummy tcb whos only purpose is to hold the flag 
    // saying dont mess with me. Because apparently you dont have to add threads with null
    // tcb to queue -> why is this?


    // Get the thread to yield to
    // No threads are ready, we have to keep going


    // make sure no interrupts can happen
    auto was = Interrupts::disable();

    auto me = active[SMP::me()];

    
    // set flag so you cna redo it.
    
   // bool meWasNull = false;
    //TCB* fake = nullptr;
    // if(me == nullptr){
    //     me = dummy[SMP::me()];
    //     // is this how to create fake thread?
    //     // fake = fakeThread([]{});
    //     // me = fake;
    //     //meWasNull = true;
    // }

    if(me->cameFromPit){
        me->was = false; 
    } else {
        me->was = was;
    }
    me->cameFromPit = false;

    me->preemptable = false;
    // active[SMP::me()] = me;
    Interrupts::restore(was);

    // now no interupts can occure bevasie flag is set

    //auto next = readyQ1.remove();
    auto next = readyQ1.remove();
    if(next == nullptr){
        next = readyQ2.remove();
        if(next == nullptr){
            next = readyQ3.remove();
            if(next == nullptr){
                next = readyQ4.remove();
            }
        } 
    }
    
    
    
    
    if (next == nullptr) {
        if(me->idleOrDummy){
            // while(next == nullptr){
            //     next = readyQ1.remove();
            // }
           while(next == nullptr){
               next = readyQ1.remove();
               if(next == nullptr){
                    next = readyQ2.remove();
                    if(next == nullptr){
                        next = readyQ3.remove();
                        if(next == nullptr){
                            next = readyQ4.remove();
                        }
                    } 
                }
            } 
        } else{
            next = idle[SMP::me()];
        }
        // // how can I just get a fake thread? 
        // TCB* tcb = fakeThread([] {
        //     yield();
        // });
        // next = tcb;
    }
    ASSERT(me != next);


    // TODO: scary race conditions here

    myLock.lock();



    // before making next active need to make sure cant preempt on it
    next->preemptable = false;
    active[SMP::me()] = next;
    // reset ability to preempt on me
    
    if (!me->idleOrDummy){
        // Debug::printf("*** before here bozo\n");
        // Debug::printf("*** waitingQ %x\n", waitingQ);
        // Debug::printf("*** readyQ1 %x\n", &readyQ1);
        if(waitingQ == &readyQ){
            // Debug::printf("*** here bozo\n");
            //readyQ1.add(me);
            if(me->priority == 1){
                readyQ1.add(me);
            } else if (me->priority == 2){
                readyQ2.add(me);
            } else if (me->priority == 3){
                readyQ3.add(me);
            } else if (me->priority == 3){
                readyQ3.add(me);
            }
        } else {
            waitingQ->add(me);
        }
    } 
    // else {
    //     zombieQ.add(me);
    // }

    // uhhh -> if this is an intterupt safe lock do I really need to be worred about intterupts before this at all?
    // maybe only thing I need to do is to 
    // oh dont know, lock could be null
    if(thierLock != nullptr){
        thierLock->unlock();
    }
    
    //if(me != nullptr && me->dead != true) waitingQ->add(me);

    // Perform the context switch
    
    // I think you just need to wait to get inside the lock to unlock -> if you 
    // unlock outside of mylock being locked then you run risk of what is stated below

    // if you unlock lock here is there not the chance that you could
    // run into senario where you unlock it so you are in ready queue
    // and set is called and you are then put into readyQ and someone stars running on you
    //while you are still running here -> no because only you can be in here beccause of mylock
    // so someone could pick you up but not be able to start running you until you leave this zone

    // maybe it is possible mid way into everyonehates assembly to get an innterupt?
    // do i need to disabele interrupts here and reenable them elsewhere? 
    // no shouldnt be possible active[smp::me()] shoudl have next which is not preemptable
    everyone_hates_assembly(
        (me == nullptr) ? nullptr : &me->saved_esp,
        &next->saved_esp);

    Interrupts::disable();
    

    // restore interupting ability
    me->preemptable = true;
    myLock.unlock();
    Interrupts::restore(me->was);
    
    
    // Interrupts::restore(false);
    // delete fake;
    // Interrupts::restore(was);

    // // does this implementation look ok?
    // // before this would need to make sure inteerupting could happen

    // // DO I NEED TO MAKE SURE THAT INTERRUPTS IS POSSIBLE BEFORE DELETING???    
    // // not just resetting it back to was? 
    // was = Interrupts::disable();
    // auto deleteTCB = stoped[SMP::me()];
    // if(deleteTCB != nullptr){
    //     stoped[SMP::me()] = nullptr;
    //     // need to set to false right? need to make sure 
    //     // Interrupts can happen in delete?
    //     Interrupts::restore(false);
    //     delete deleteTCB;
    // }
    // Interrupts::restore(was);
    
    
}

void yield() {
    auto was = Interrupts::disable();

    auto me = active[SMP::me()];

    Interrupts::restore(was);
    if(me->priority == 1){
        block(&readyQ1, nullptr);
    } else if(me->priority == 2){
        block(&readyQ2, nullptr);
    } else if(me->priority == 3){
        block(&readyQ3, nullptr);  
    } else if(me->priority == 4){
        block(&readyQ4, nullptr);  
    }
    //block(&readyQ1, nullptr);
    return;
    // disable interrupts here?
    // block(&readyQ, nullptr);
    // return;

    // auto was = Interrupts::disable();
    // auto next = readyQ.remove();
    // auto me = active[SMP::me()];
    // if (next == nullptr) {
    //     if(me != nullptr){
    //         if(me->cameFromPit){
    //             me->was = false;
    //         } else {
    //             me->was = was;
    //         }
    //         Interrupts::restore(me->was);
    //         return;
    //     }
    //     Interrupts::restore(was);
    //     return;
    // }
    
    // bool meWasNull = false;
    // TCB* fake = nullptr;
    // if(me == nullptr){
    //     // is this how to create fake thread?
    //     fake = fakeThread([]{});
    //     me = fake;
    //     meWasNull = true;
    // }

    // ASSERT(me != next);
    // if(me->cameFromPit){
    //     me->was = false;
    // } else {
    //     me->was = was;
    // }
    // me->cameFromPit = false;
    // me->preemptable = false;

    // myLock.lock();
    // next->preemptable = false;
    // active[SMP::me()] = next;
    // if(!meWasNull && !me->nullInPit && !me->dead){ 
    //     readyQ.add(me);
    // } else {
    //     zombieQ.add(me);
    // }

    // // Perform the context switch
    // everyone_hates_assembly(
    //     (me == nullptr) ? nullptr : &me->saved_esp,
    //     &next->saved_esp);

    // Interrupts::disable();
    // me->preemptable = true;
    // myLock.unlock();
    // Interrupts::restore(me->was);




    // // // Get the thread to yield to
    // // // No threads are ready, we have to keep going
    // // auto next = readyQ.remove();
    // // if (next == nullptr) return;

    // // // if(next->dead == true){
    // // //     return;
    // // // }
    
    

    // // auto me = active[SMP::me()];
    // // ASSERT(me != next);

    // // // TODO: scary race conditions here


    // // myLock.lock();
    // // active[SMP::me()] = next;
    // // if(me != nullptr && me->dead != true) readyQ.add(me);

    // // // Perform the context switch
    // // everyone_hates_assembly(
    // //     (me == nullptr) ? nullptr : &me->saved_esp,
    // //     &next->saved_esp);

    // // myLock.unlock();


    // // if(stoped[SMP::me()] != nullptr){
    // //     delete stoped[SMP::me()];
    // //     stoped[SMP::me()] = nullptr;
    // // }

    // // // could be wrong but 

    
}

// TODO: this is silly, we don't really stop but we do stop making
// forward progress. A waste of resources: CPU time and memory but
// it does appear to work correctly.
void stop() {
    block(&zombieQ, nullptr);
    return;
    Interrupts::disable();
    auto me = active[SMP::me()];
    me->dead = true;
    while(true) {yield();}

    // am I tthinking about this right?
    // only threads on this core will ever try to chang or 
    // delete from index smp in deleting array
    // it is possible that you get to here and there is still a tcb
    // waiting to be deleted
    // need to remove that one from array before adding yourself to 
    // prevent memeory leak

    // is it ok to have most of stop unable to be interrupted? 

    // auto was = Interrupts::disable();
    // auto me = active[SMP::me()];
    // // I don't think you need to set dead to true in non
    // // interuptable zone
    // // I think the important part is
    // // HMMM big error potential here
    // // dead prevents you from being added to readyQ
    // // what is the point of dead anyway
    // // how would someting dead -> only can be set here
    // // end up trying to be added to readyQ
    // // oh i see
    // // dead makes sure you don't get added to readyQ after you have been added 
    // // to the delete list
    // // maybe no error here.
    // // if there is an innterupt it shouldnt really matter -> if there is another thread to run
    // // you wonr get added back into the readyQ which is fint because after restoring the innterupt 
    // // all you would be doing is looping which the thread could do
    // // if there is not another thread you should return to right before the loop
    // me->dead = true;
    // auto deleteTCB = stoped[SMP::me()];
    // stoped[SMP::me()] = me;
    // Interrupts::restore(was);

    // all I added was 
    // if(deleteTCB != nullptr && deleteTCB != me){
    //     Interrupts::restore(false);
    //     delete deleteTCB;
    //     Interrupts::restore(was);
    // }

    //while(true) {yield();}
}
