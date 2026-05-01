#include <stdio.h>
#include <stdlib.h>
#include "scheduling.h"

static thread_t *threads[MAX_THREADS];
static int num_threads = 0;
static int current_tid = -1;
static int global_time = 0;
static ucontext_t scheduler_context;
static int quantum_remaining = 0;   // for round‑robin preemption

// Round‑Robin: pick next ready thread in circular order
static thread_t *next_thread_rr(void) {
    if (num_threads == 0) return NULL;
    static int last_tid = -1;
    int start = (last_tid + 1) % num_threads;
    for (int i = 0; i < num_threads; i++) {
        int idx = (start + i) % num_threads;
        thread_t *t = threads[idx];
        if (t->state == READY && t->arrival_time <= global_time) {
            last_tid = idx;
            return t;
        }
    }
    return NULL;
}

void schedule_run(void) {
    while (1) {
        thread_t *next = next_thread_rr();
        if (!next) {
            // No runnable thread → jump to next arrival time
            int next_arrival = 1e9;
            for (int i = 0; i < num_threads; i++) {
                if (threads[i]->state != FINISHED &&
                    threads[i]->arrival_time > global_time &&
                    threads[i]->arrival_time < next_arrival) {
                    next_arrival = threads[i]->arrival_time;
                }
            }
            if (next_arrival == 1e9) break;  // all done
            global_time = next_arrival;
            continue;
        }
        // Preemptive round robin: give thread TIME_QUANTUM units
        quantum_remaining = TIME_QUANTUM;
        current_tid = next->tid;
        next->state = RUNNING;
        swapcontext(&scheduler_context, &next->context);
        // When thread yields or quantum expires, it comes back here
        next->state = READY;
    }
}

void thread_yield(void) {
    if (current_tid != -1) {
        threads[current_tid]->state = READY;
        current_tid = -1;
        swapcontext(&threads[current_tid]->context, &scheduler_context);
    }
}

void thread_exit(void *retval) {
    if (current_tid != -1) {
        threads[current_tid]->state = FINISHED;
        threads[current_tid]->retval = retval;
        current_tid = -1;
        swapcontext(&threads[current_tid]->context, &scheduler_context);
    }
    exit(0);
}

int schedule_init(void) {
    num_threads = 0;
    current_tid = -1;
    global_time = 0;
    quantum_remaining = 0;
    return 0;
}

int thread_create(void (*func)(void *), void *arg,
                  int arrival_time, int burst_time) {
    if (num_threads >= MAX_THREADS) return -1;
    thread_t *t = malloc(sizeof(thread_t));
    t->tid = num_threads;
    t->state = READY;
    t->arrival_time = arrival_time;
    t->burst_time = burst_time;
    t->remaining_time = burst_time;
    t->last_run = 0;
    t->retval = NULL;

    getcontext(&t->context);
    t->context.uc_stack.ss_sp = malloc(STACK_SIZE);
    t->context.uc_stack.ss_size = STACK_SIZE;
    t->context.uc_link = &scheduler_context;
    makecontext(&t->context, (void (*)(void))func, 1, arg);

    threads[num_threads++] = t;
    return t->tid;
}

int thread_join(int tid, void **retval) {
    while (threads[tid]->state != FINISHED)
        thread_yield();
    if (retval) *retval = threads[tid]->retval;
    return 0;
}