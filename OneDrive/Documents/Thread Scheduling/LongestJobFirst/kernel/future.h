#ifndef _future_h_
#define _future_h_

#include "atomic.h"
#include "semaphore.h"
#include "threads.h"


template <typename T>
class Future {
    Semaphore go;
    bool volatile isReady;
    T volatile    t;

    // Needed by Shared<>
    Atomic<uint32_t> ref_count;
public:

    Future() : go(0), isReady(false), t(), ref_count(0) {}

    // Can't copy a future
    Future(const Future&) = delete;
    
    void set(T v) {
        ASSERT(!isReady);
        t = v;
        isReady = true;
        go.up();
    }
    T get() {
        if (!isReady) {
            go.down();
            go.up();
        }
        return t;
    }

};

#endif

