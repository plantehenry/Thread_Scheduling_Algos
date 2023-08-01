#include "debug.h"
#include "future.h"
#include "barrier.h"

/* Called by one CPU */
void kernelMain(void) {
    auto bar = new Barrier(601);
    auto fut = new Future<int>();
    auto fut2 = new Future<int>();
    auto fut3 = new Future<int>();
    auto fut4 = new Future<int>();
    auto fut5 = new Future<int>();
    auto fut6 = new Future<int>();
    auto fut7 = new Future<int>();
    
    for (int j=0; j<200; j++) {
        for(int i = 2; i <= 4; i++){
            thread(([bar, fut, fut2, fut3, fut4, fut5, fut6, fut7, i, j] {
                int val = fut->get();
                if(j == 199 && i == 4){
                    Debug::printf("*** %d\n", val);
                    thread([fut2]{
                        fut2->set(2);
                    });
                }
                yield();
                val = fut2->get();
                if(j % 50 == 0){
                    Debug::printf("*** %d\n", val);
                }
                if(j == 199 && i == 4){
                    Debug::printf("*** %d\n", val);
                    thread([fut3]{
                        fut3->set(3);
                    });
                }
                yield();
                val = fut3->get();
                if(j % 50 == 0){
                    Debug::printf("*** %d\n", val);
                }
                if(j == 199 && i == 4){
                    Debug::printf("*** %d\n", val);
                    thread([fut4]{
                        fut4->set(4);
                    });
                }
                yield();
                if(j % 50 == 0){
                    Debug::printf("*** %d\n", val);
                }
                val = fut4->get();
                if(j == 199 && i == 4){
                    Debug::printf("*** %d\n", val);
                    thread([fut5]{
                        fut5->set(5);
                    });
                }
                yield();
                if(j % 50 == 0){
                    Debug::printf("*** %d\n", val);
                }
                val = fut5->get();
                if(j == 199 && i == 4){
                    Debug::printf("*** %d\n", val);
                    thread([fut6]{
                        fut6->set(6);
                    });
                }
                yield();
                if(j % 50 == 0){
                    Debug::printf("*** %d\n", val);
                }
                val = fut6->get();
                if(j == 199 && i == 4){
                    Debug::printf("*** %d\n", val);
                    thread([fut7]{
                        fut7->set(7);
                    });
                }
                yield();
                if(j % 50 == 0){
                    Debug::printf("*** %d\n", val);
                }
                val = fut7->get();

                bar->sync();
            }));
        }  
    }
    thread(([fut] {
        fut->set(1);
    }));

    bar->sync();
    Debug::printf("*** done\n");
}