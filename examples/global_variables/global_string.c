#include <stdio.h>
#include <unistd.h>

#define ADJUST_IMPLEMENTATION
#include "adjust.h"

ADJUST_GLOBAL_CONST_STRING(g_a, "Hell, World!");
ADJUST_GLOBAL_VAR_STRING(g_b, "Goodbasdfasdfye, \"quotations \t\" friends!");

int main(void)
{
    adjust_init();

    adjust_register_global_string(g_a);
    adjust_register_global_string(g_b);

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("You have %lu seconds to adjust...\n", countdown);
        sleep(1);
    }

    printf("Before: %s\n", g_a);
    printf("Before: %s\n", g_b);
    adjust_update();
    printf("After: %s\n", g_a);
    printf("After: %s\n", g_b);

    adjust_cleanup();
    return 0;
}
