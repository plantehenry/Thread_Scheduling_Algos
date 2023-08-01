#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "bb.h"


/*
Checks the implementation of the boundedBuffer
first it tries to add more values than the buffere has space for
then it tries to get more values than the buffer has space for

if you are failing you are may not be implementing waitingQs for both
putting and getting correctly 
threads that are trying to be added when there isnt space should somehow wait
until there is space. Somehow these threads need to be woken up as well.

You may not be implementing the buffer correctly. The size can't exceed
the size specified, so you have to somehow keep track of the how many elements
are in the buffer

*/

/* Called by one CPU */
void kernelMain(void) {
    Debug::printf("start of kernelmain \n");


    BoundedBuffer<int> buf {(uint32_t)10};
    Atomic<int> count {0};

    // adds fout 0s to the bounded buffer
    Interrupts::disable();
    for (int i=0; i<10; i++) {
        thread([&count, &buf] {
            count.add_fetch(1);
            buf.put(0);
        });
    }
    

    // waits until all threads have started to execute
    // that way hopefully more threads will have tried to put than there
    // is available space
    while(count < 10){}

    
    for (int i=0; i<10; i++) {
        thread([&buf, &count] {
            Debug::printf("*** %d\n", buf.get() );
            count.add_fetch(-1);
        });
    }
    Interrupts::restore(false);

    while(count.get() != 0);

}