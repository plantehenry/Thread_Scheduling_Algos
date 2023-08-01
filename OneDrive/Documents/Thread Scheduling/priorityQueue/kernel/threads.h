#ifndef _threads_h_
#define _threads_h_

#include "atomic.h"
#include "queue.h"
#include "heap.h"
#include "debug.h"
#include "smp.h"


extern Atomic<bool> reaper_enabled;

// Base class. Every thread will have a TCB that extends this
struct TCB {
    // Next, allows us to be inserted in linked lists
    TCB* next = nullptr;
    // flag so you can delete stack and stuff
    bool dead = false;

    bool preemptable = true;
    int priority = 1;

    bool was = false;

    bool idleOrDummy = false;

    bool cameFromPit = false;
    bool nullInPit = false;
    // A place to save the stack pointer. The rest of the state is saved on the stack
    uint32_t saved_esp;


    // abstract method that represents the initial work that needs to be done by the thread
    // called once when the thread is dispatched
    virtual void do_what_you_need_to_do() = 0;
    virtual ~TCB(){

    }

};

// A special function that gets called once when a thread is first dispatched
// Why not just call 'do_what_you_need_to_do'? Because it is tricky to convince
// the context switching code implemented in assembly to enter an overloaded method.
extern void a_new_thread_starts_here();

// Each call to 'thread' will get its own specialization of this template
template <typename Work>
struct TCBWithWork : public TCB {
    // baaaaad programmer
    // the stack
    uint32_t stack[2048];

    // The initial work that needs to be done
    Work work;



    TCBWithWork(Work w): work(w) {
        // context switching pops 4 callee-saved registers
        // from the stack then returns.
        // We maniuplate the stack to make the return address
        // be the entry point for 'a_new_thread_starts_here'
        stack[2047] = (uint32_t) a_new_thread_starts_here;
        saved_esp = (uint32_t) &stack[2043];
    }

    // This method invoked the first time a thread runs, it just does the
    // work.
    void do_what_you_need_to_do() override {
        work();
    }

    virtual ~TCBWithWork(){
        //delete[] &stack;
    }

    
};

extern void threadsInit();

extern void stop();
extern void yield();
extern void block(Queue<TCB, InterruptSafeLock> *waitingQ, InterruptSafeLock *lock);

extern Queue<TCB, InterruptSafeLock> readyQ;
extern Queue<TCB, InterruptSafeLock> readyQ1;
extern Queue<TCB, InterruptSafeLock> readyQ2;
extern Queue<TCB, InterruptSafeLock> readyQ3;
extern Queue<TCB, InterruptSafeLock> readyQ4;
extern Queue<TCB, InterruptSafeLock> zombieQ;
extern TCB* active[];
extern TCB* idle[];
extern TCB* dummy[];

template <typename T>
void thread(int priority, T work ) {
    auto was = Interrupts::disable();
    if(!reaper_enabled.exchange(true)){
        reaper_enabled = true;
        auto reaper = TCBWithWork([]{
            Debug::printf("*** in reaper\n");
            while(true){
            auto tcb = zombieQ.remove();
            while(tcb != nullptr){
                delete tcb;
                tcb = zombieQ.remove();
            }
            yield();
        }
        });
        reaper.priority = 4;
    }
    Interrupts::restore(was);
    if(priority < 1){
        priority = 1;
    } else if(priority > 4){
        priority = 4;
    }
    auto tcb = new TCBWithWork(work);
    tcb->priority = priority;
    
    // gloabl variable
    // reperthreadEabled = false;
    readyQ1.add(tcb);
    //MISSING();
}

template <typename T>
TCB* fakeThread(T work){
    auto tcb = new TCBWithWork(work);
    tcb->priority = 4;
    return tcb;
}

#endif
