#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "bb.h"

/* Called by one CPU */
void kernelMain(void) {
    Debug::printf("start of kernelmain \n");
    
    auto bar = new Barrier(2);
    for (int j=0; j<200; j++) {
        for(int i = 2; i <= 4; i++){
            Atomic<int> dummy {0};
            thread(j, [&dummy] {
                for(int i = 0; i < 100000; i++){
                    dummy.add_fetch(1);
                }
            });
        }
    }

    thread(1, [bar]{
        Debug::printf("*** finished\n");
        Debug::shutdown();
        bar->sync();
    });

    

    bar->sync();
    Debug::printf("*** done\n");
}

