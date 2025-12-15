#include <stdio.h>
#include <unistd.h>

#define ADJUST_IMPLEMENTATION
#include "adjust.h"

char get_char(void);

int main(void)
{
    adjust_init();

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("%lu...\n", countdown);
        sleep(1);
    }

    printf("Before: %c\n", get_char());
    adjust_update();
    printf("After: %c\n", get_char());

    adjust_cleanup();

    return 0;
}

char get_char(void)
{
    return ADJUST_CHAR('c');
}
