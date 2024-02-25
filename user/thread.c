#include "kernel/types.h"
#include "user/user.h"    // for uint and malloc
#include "kernel/param.h"
#include "kernel/riscv.h" // for PGSIZE

#include "user/thread.h"

//interrupt disable?
// for parent: return 0 on success or -1 on fail
int thread_create(void *(start_routine)(void*), void *arg) {
    void *new_stack = malloc((uint)PGSIZE);
    int return_value; 

    // clone can return three things: 0 (new thread), threadpid (parent), or -1 (failure)
    return_value = clone(new_stack);

    if (return_value == 0) { // child thread
        start_routine(arg);
        exit(0);
    }
    // child thread should not reach past this point
    if (return_value > 0) { //success
        return_value = 0;
    }

    return return_value;
}

void lock_init(lock_t* lock) {
    lock->locked = 0;
    return;
}

void lock_acquire(lock_t* lock) {
    while(__sync_lock_test_and_set(&lock->locked, 1) != 0);
    __sync_synchronize();
    return;
}

void lock_release(lock_t* lock) {
    __sync_synchronize();
    __sync_lock_release(&lock->locked);
    return;
}