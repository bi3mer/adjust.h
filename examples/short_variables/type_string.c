#include "adjust.h"
#include <stdio.h>
#include <unistd.h>

static inline char *get_string(void)
{
    return ADJUST_STRING("Hello, World!");
}

int main(void)
{
    adjust_init();

    printf("Before: %s\n", get_string());

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("%lu...\n", countdown);
        sleep(1);
    }

    printf("Before: %s\n", get_string());
    adjust_update();
    printf("After: %s\n", get_string());

    adjust_cleanup();

    return 0;
}
