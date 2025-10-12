#include "adjust.h"
#include <unistd.h>

ADJUST_GLOBAL_CONST_FLOAT(g_a, 0.1f);
ADJUST_GLOBAL_CONST_FLOAT(g, 1.2f);

int main(void)
{
    adjust_init();

    adjust_register_global_float(g_a);
    adjust_register_global_float(
        g); // TODO: temp, move below adjust_const_float

    ADJUST_CONST_FLOAT(aa, 2.2f);
    ADJUST_CONST_FLOAT(a, 2.3f);

    // TODO:
    // adjust_register_global(&G, _ADJUST_FLOAT); // ordering doesn't matter!

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("You have %lu seconds to adjust...\n", countdown);
        sleep(1);
    }

    printf("Before\n");
    printf("g_a: %f\n", g_a);
    printf("g:   %f\n", g);
    printf("aa:  %f\n", aa);
    printf("a:   %f\n", a);

    adjust_update();

    printf("\nAfter\n");
    printf("g_a: %f\n", g_a);
    printf("g:   %f\n", g);
    printf("aa:  %f\n", aa);
    printf("a:   %f\n", a);

    adjust_cleanup();
    return 0;
}
