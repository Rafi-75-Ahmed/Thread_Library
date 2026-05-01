#include "relax_operation.h"
#include "scheduling.h"

static void relax_thread(void *arg) {
    int *ticks = (int *)arg;
    for (int i = 0; i < *ticks; i++) {
        // Absolutely no printing – terminal unchanged
        thread_yield();
    }
    thread_exit(NULL);
}

void perform_relax_operation(int duration) {
    int tid = thread_create(relax_thread, &duration, 4, duration);
    thread_join(tid, NULL);
}