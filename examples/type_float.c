#include "adjust.h"
#include <unistd.h>

int main(void)
{
    adjust_init();
    ADJUST_CONST_FLOAT(a, 0.000f);
    ADJUST_VAR_FLOAT(b, 0.000f);

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("%lu...\n", countdown);
        sleep(1);
    }

    printf("\nBEFORE\n");
    printf("a: %f\n", a);
    printf("b: %f\n", b);

    adjust_update();

    printf("AFTER\n");
    printf("a: %f\n", a);
    printf("b: %f\n", b);

    adjust_cleanup();
}
