#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <ucontext.h>

#define MAX_THREADS 10
#define STACK_SIZE 8192
#define TIME_QUANTUM 2   // preemptive round robin quantum (in virtual time units)

typedef enum { READY, RUNNING, FINISHED } state_t;

typedef struct thread {
    int tid;
    state_t state;
    ucontext_t context;
    int arrival_time;    // when thread becomes ready
    int burst_time;      // total CPU time needed
    int remaining_time;  // still needs this much
    int last_run;        // last time it was scheduled
    void *retval;
} thread_t;

// Library functions
int  schedule_init(void);
int  thread_create(void (*func)(void *), void *arg,
                   int arrival_time, int burst_time);
void thread_yield(void);
void thread_exit(void *retval);
int  thread_join(int tid, void **retval);
void schedule_run(void);   // start the round‑robin scheduler

#endif