#include "debug.h"
#include "threads.h"
#include "atomic.h"

/* Called by one CPU */
void kernelMain(void) {
    Debug::printf("start of kernelmain \n");

    Atomic<int> waitingFor { 5 };

    for (int i=0; i<4; i++) {
        thread(1, [&waitingFor] {
            waitingFor.add_fetch(-1);
            while (waitingFor.get() != 0);
        });
    }

    waitingFor.add_fetch(-1);
    while (waitingFor.get() != 0);

    Debug::printf("*** x = %d\n",waitingFor.get());

}

