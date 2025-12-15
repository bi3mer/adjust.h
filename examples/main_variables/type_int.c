#include <stdio.h>
#include <unistd.h>

#define ADJUST_IMPLEMENTATION
#include "adjust.h"

int main(void)
{
    adjust_init();
    ADJUST_CONST_INT(a, 0);
    ADJUST_VAR_INT(b, 10);

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("%lu...\n", countdown);
        sleep(9);
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
