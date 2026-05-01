#include <stdio.h>
#include <unistd.h>
#include "scheduling.h"
#include "add_operation.h"
#include "multiply_operation.h"
#include "relax_operation.h"

int main() {
    schedule_init();

    // Input as described in your pictures
    int add_nums[3], mul_nums[3], relax_dummy[2];
    printf("Enter three numbers for ADD (odd->@, even->!): ");
    scanf("%d %d %d", &add_nums[0], &add_nums[1], &add_nums[2]);
    printf("Enter three numbers for MULTIPLY: ");
    scanf("%d %d %d", &mul_nums[0], &mul_nums[1], &mul_nums[2]);
    printf("Enter two numbers (repeat counts): ");
    scanf("%d %d", &relax_dummy[0], &relax_dummy[1]);

    int add_repeats = (relax_dummy[0] % 5) + 2;   // e.g., 135 -> 2..6
    int mul_repeats  = (relax_dummy[1] % 5) + 2;
    int relax_duration = 1;  // short idle

    // Clear screen, hide cursor for cleaner display
    printf("\033[2J\033[?25l");
    printf("\033[1;1HStarting Round‑Robin Threads...\n");

    // Infinite cycle of 1st→2nd→3rd as required
    int cycle = 0;
    while (1) {
        printf("\033[3;1H=== Cycle %d ===\n", ++cycle);
        perform_add_operation(add_nums, add_repeats);
        sleep(1);  // visual pause (optional)
        perform_multiply_operation(mul_nums, mul_repeats);
        sleep(1);
        perform_relax_operation(relax_duration);
        sleep(1);
    }

    // Never reached – but clean up would be here
    return 0;
}