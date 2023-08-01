#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "pit.h"



/* Called by one CPU */
void kernelMain(void) {
    Debug::printf("start of kernelmain \n");
    Atomic<int> waitingFor { 601 };
    Atomic<int> counter  {0};
    Atomic<int> dummyCounter {0};

    
    for (int i=0; i<600; i++) {
        uint32_t numLoops = 500000 - (i * i);
        uint32_t initialCount = intCount;
        thread([numLoops, &counter, &dummyCounter, &waitingFor, initialCount] {
            for(uint32_t i = 0; i < numLoops; i++){
                dummyCounter.set(1);  
            }
            uint32_t curCount = intCount;
            uint32_t totalTime = curCount - initialCount;
            counter.add_fetch(totalTime);
            waitingFor.add_fetch(-1);
        });
    }


    waitingFor.add_fetch(-1);
    while (waitingFor.get() != 0);
    uint32_t average = counter.get() / 600;
    Debug::printf("*** %d\n", average);

}

