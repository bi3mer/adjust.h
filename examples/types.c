#include "adjust.h"

#include <unistd.h>

int main(void)
{
    size_t countdown;
    adjust_init();

    for (countdown = 5; countdown > 0; countdown--)
    {
        printf("You have %lu seconds to adjust...\n", countdown);
        sleep(1);
    }

    adjust_update();

    printf("gravity: %f\n", gravity);
    printf("a: %f\n", a);
    printf("b: %i\n", b);
    printf("c: %i\n", c);
    printf("d: %i\n", d);
    printf("e: %i\n", e);
    printf("f: %s\n", f);
    printf("g: %s\n", g);

    adjust_cleanup();
}
