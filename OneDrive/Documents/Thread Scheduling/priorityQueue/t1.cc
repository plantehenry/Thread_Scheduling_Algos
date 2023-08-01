#include "debug.h"
#include "threads.h"
#include "barrier.h"

/* Called by one CPU */
void kernelMain(void) {

    auto bar = new Barrier(5);
    Atomic<int> sum{0};
    // Debug::printf("*** bar %x\n", bar);
    // Debug::printf("*** sum %d\n", sum.get());
    for (int i=0; i<4; i++) {
        thread(1, ([bar,i,&sum] {
            // Debug::printf("*** in here bozo i %d\n", i);
            sum.add_fetch(i+1);
            // Debug::printf("*** after add fetch\n");
            // Debug::printf("*** bar %x\n", bar);
            // Debug::printf("*** sum %d\n", sum.get());
            bar->sync();
        }));
    }
    // Debug::printf("*** bozo\n");
    bar->sync();
    // Debug::printf("*** bozo\n");
    Debug::printf("*** sum = %d\n",sum.get());

}

