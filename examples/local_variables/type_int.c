#include <stdio.h>
#include <unistd.h>

#define ADJUST_IMPLEMENTATION
#include "adjust.h"

int get_int(void);

int main(void)
{
    adjust_init();

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("%lu...\n", countdown);
        sleep(1);
    }

    printf("Before: %i\n", get_int());
    adjust_update();
    printf("After: %i\n", get_int());

    adjust_cleanup();

    return 0;
}

int get_int(void)
{
    return ADJUST_INT(-4000);
}
