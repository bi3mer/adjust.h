#include "adjust.h"

#include <sys/unistd.h>
#include <unistd.h>

int main(void)
{
    size_t countdown;
    adjust_init();

    ADJUST_CONST_FLOAT(gravity, -9.8f);
    ADJUST_VAR_FLOAT(a, 1.1f);

    ADJUST_CONST_INT(b, 3);
    ADJUST_VAR_INT(c, 10);
    // ADJUST_VAR_FLOAT(d, 4.1f);
    // ADJUST_VAR_FLOAT(e, 5.f);
    // ADJUST_VAR_FLOAT(f, 100.f);

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
    // printf("d: %f\n", d);
    // printf("e: %f\n", e);
    // printf("f: %f\n", f);

    adjust_cleanup();
}