#include "adjust.h"
#include <unistd.h>

ADJUST_GLOBAL_CONST_BOOL(toggle1, FALSE);
ADJUST_GLOBAL_VAR_BOOL(toggle2, FALSE);

int main(void)
{
    adjust_init();

    adjust_register_global_bool(toggle1);
    adjust_register_global_bool(toggle2);

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("You have %lu seconds to adjust...\n", countdown);
        sleep(1);
    }

    printf("Before: %i\n", toggle1);
    printf("Before: %i\n", toggle2);
    adjust_update();
    printf("After: %i\n", toggle1);
    printf("After: %i\n", toggle2);

    adjust_cleanup();
    return 0;
}
