#include "adjust.h"
#include <stdio.h>
#include <unistd.h>

bool get_bool(void)
{
    return ADJUST_BOOL(true);
}

int main(void)
{
    adjust_init();
    bool before = get_bool();

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("%lu...\n", countdown);
        sleep(1);
    }

    printf("Before: %i\n", before);
    adjust_update();
    printf("After: %i\n", get_bool());

    adjust_cleanup();
}
