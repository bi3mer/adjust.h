#include <stdio.h>
#include <unistd.h>

#define ADJUST_IMPLEMENTATION
#include "adjust.h"

int main(void)
{
    adjust_init();
    ADJUST_CONST_CHAR(a, 'a');
    ADJUST_VAR_CHAR(b, 'b');

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("%lu...\n", countdown);
        sleep(1);
    }

    printf("\nBEFORE\n");
    printf("a: %c\n", a);
    printf("b: %c\n", b);

    adjust_update();

    printf("AFTER\n");
    printf("a: %c\n", a);
    printf("b: %c\n", b);

    adjust_cleanup();
}
