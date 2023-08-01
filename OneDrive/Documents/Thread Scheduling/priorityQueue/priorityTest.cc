#include "debug.h"
#include "future.h"
#include "barrier.h"

/* Called by one CPU */
void kernelMain(void) {
    auto bar = new Barrier(301);
    auto fut = new Future<int>();
    for (int j=0; j<100; j++) {
        for(int i = 2; i <= 4; i++){
            thread(i, ([bar, fut,i, j] {
                int val = fut->get();
                if(j == 99 && i == 4){
                    Debug::printf("*** %d\n", val);
                }
                bar->sync();
            }));
        }  
    }
    thread(1, ([fut] {
        fut->set(1);
    }));

    bar->sync();
    Debug::printf("*** done\n");
}