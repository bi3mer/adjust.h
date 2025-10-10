#include "adjust.h"

#include <sys/unistd.h>
#include <unistd.h>

int main(void)
{
    size_t countdown;
    adjust_init();

    ADJUST_CONST_FLOAT(gravity, -9.8f);
    ADJUST_VAR_FLOAT(a, 1.1f);
    ADJUST_VAR_FLOAT(b, 2.1f);
    ADJUST_VAR_FLOAT(c, 3.1f);
    ADJUST_VAR_FLOAT(d, 4.1f);
    ADJUST_VAR_FLOAT(e, 5.f);
    ADJUST_VAR_FLOAT(f, 100.f);

    for (countdown = 10; countdown > 0; countdown--)
    {
        printf("You have %lu seconds to adjust...\n", countdown);
        sleep(1);
    }

    adjust_update();

    f *= 10;

    printf("gravity: %f\n", gravity);
    printf("a: %f\n", a);
    printf("b: %f\n", b);
    printf("c: %f\n", c);
    printf("d: %f\n", d);
    printf("e: %f\n", e);
    printf("f: %f\n", f);

    adjust_cleanup();
}