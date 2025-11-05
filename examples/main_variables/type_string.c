#include "adjust.h"
#include <unistd.h>

int main(void)
{
    adjust_init();
    ADJUST_CONST_STRING(a, "\"adjust.h\" can ignore annoying '\"' \t marks!\n");
    ADJUST_VAR_STRING(b, "To infinity... and beyond!");

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("%lu...\n", countdown);
        sleep(1);
    }

    printf("\nBEFORE\n");
    printf("a: %s\n", a);
    printf("b: %s\n", b);

    adjust_update();

    printf("AFTER\n");
    printf("a: %s\n", a);
    printf("b: %s\n", b);

    adjust_cleanup();
}
