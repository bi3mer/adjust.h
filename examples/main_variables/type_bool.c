#include "adjust.h"
#include <unistd.h>

int main(void)
{
    adjust_init();
    ADJUST_CONST_BOOL(a, false);
    ADJUST_VAR_BOOL(b, false);

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("%lu...\n", countdown);
        sleep(1);
    }

    printf("\nBEFORE\n");
    printf("a: %i\n", a);
    printf("b: %i\n", b);

    adjust_update();

    printf("AFTER\n");
    printf("a: %i\n", a);
    printf("b: %i\n", b);

    adjust_cleanup();
}
