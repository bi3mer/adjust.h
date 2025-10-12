#include "adjust.h"
#include <unistd.h>

ADJUST_GLOBAL_CONST_CHAR(g_a, 'a');
ADJUST_GLOBAL_VAR_CHAR(g_b, 'b');

int main(void)
{
    adjust_init();

    adjust_register_global_char(g_a);
    adjust_register_global_char(g_b);

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("You have %lu seconds to adjust...\n", countdown);
        sleep(1);
    }

    printf("Before: %c\n", g_a);
    printf("Before: %c\n", g_b);
    adjust_update();
    printf("After: %c\n", g_a);
    printf("After: %c\n", g_b);

    adjust_cleanup();
    return 0;
}
