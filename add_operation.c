#include <stdio.h>
#include "add_operation.h"
#include "scheduling.h"

typedef struct {
    int numbers[3];
    int repeats;
    int burst;   // each character printed consumes 1 burst unit
} add_arg_t;

static void add_thread(void *arg) {
    add_arg_t *a = (add_arg_t *)arg;
    char symbols[3];
    for (int i = 0; i < 3; i++)
        symbols[i] = (a->numbers[i] % 2) ? '@' : '!';
    
    // Simulate burst time: print one character, then yield/check quantum
    for (int r = 0; r < a->repeats; r++) {
        for (int i = 0; i < 3; i++) {
            // Move cursor to middle row (row=15) and appropriate column
            int col = 40 + (r*4 + i)*2;
            printf("\033[%d;%dH%c ", 15, col, symbols[i]);
            fflush(stdout);
            // Each character costs 1 burst unit → round robin preempts here
            a->burst--;
            if (a->burst > 0) thread_yield();
        }
    }
    thread_exit(NULL);
}

void perform_add_operation(int numbers[], int repeats) {
    add_arg_t arg = {{numbers[0], numbers[1], numbers[2]}, repeats, repeats * 3};
    int tid = thread_create(add_thread, &arg, 0, arg.burst);
    thread_join(tid, NULL);
}