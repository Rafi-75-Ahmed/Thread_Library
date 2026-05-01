#include <stdio.h>
#include "multiply_operation.h"
#include "scheduling.h"

typedef struct {
    int numbers[3];
    int repeats;
    int burst;
} mult_arg_t;

static void mult_thread(void *arg) {
    mult_arg_t *m = (mult_arg_t *)arg;
    char symbols[3];
    for (int i = 0; i < 3; i++)
        symbols[i] = (m->numbers[i] % 2) ? '@' : '!';
    
    for (int r = 0; r < m->repeats; r++) {
        for (int i = 0; i < 3; i++) {
            // Vertical: same column (60), rows 10,12,14...
            int row = 10 + i * 2;
            printf("\033[%d;%dH%c", row, 60, symbols[i]);
            fflush(stdout);
            m->burst--;
            if (m->burst > 0) thread_yield();
        }
        if (r < m->repeats - 1) {
            printf("\n");  // small gap
            m->burst--;
            if (m->burst > 0) thread_yield();
        }
    }
    thread_exit(NULL);
}

void perform_multiply_operation(int numbers[], int repeats) {
    mult_arg_t arg = {{numbers[0], numbers[1], numbers[2]}, repeats, repeats * 4};
    int tid = thread_create(mult_thread, &arg, 2, arg.burst); // arrival=2
    thread_join(tid, NULL);
}