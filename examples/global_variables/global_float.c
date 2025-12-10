#include <stdio.h>
#include <unistd.h>

#define ADJUST_IMPLEMENTATION
#include "adjust.h"

ADJUST_GLOBAL_CONST_FLOAT(g_a, 0.2f);

int main(void)
{
    adjust_init();

    ADJUST_CONST_FLOAT(a, 2.0f);
    adjust_register_global_float(g_a); /* order doesn't matter */

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("You have %lu seconds to adjust...\n", countdown);
        sleep(1);
    }

    printf("Before\n");
    printf("g_a: %f\n", g_a);
    printf("a:   %f\n", a);

    adjust_update();

    printf("\nAfter\n");
    printf("g_a: %f\n", g_a);
    printf("a:   %f\n", a);

    adjust_cleanup();
    return 0;
}
