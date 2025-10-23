#include "adjust.h"
#include <unistd.h>

ADJUST_GLOBAL_CONST_INT(g_a, 10);
ADJUST_GLOBAL_CONST_INT(g_b, 100);

int main(void)
{
    adjust_init();

    adjust_register_global_int(g_b);
    adjust_register_global_int(g_a);

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("You have %lu seconds to adjust...\n", countdown);
        sleep(1);
    }

    printf("Before: g_a=%i, g_b=%i\n", g_a, g_b);
    adjust_update();
    printf("After:  g_a=%i, g_b=%i\n", g_a, g_b);

    adjust_cleanup();
    return 0;
}
