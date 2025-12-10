#include <stdio.h>
#include <unistd.h>

#define ADJUST_IMPLEMENTATION
#include "adjust.h"

float get_float(void);

int main(void)
{
    adjust_init();

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("%lu...\n", countdown);
        sleep(1);
    }

    printf("Before: %f\n", get_float());
    adjust_update();
    printf("After: %f\n", get_float());

    adjust_cleanup();

    return 0;
}

float get_float(void)
{
    return ADJUST_FLOAT(3.3f);
}
