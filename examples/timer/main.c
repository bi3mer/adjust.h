#include "adjust.h"

#include <sys/unistd.h>
#include <unistd.h>

int main(void)
{
    size_t countdown;
    adjust_begin();

    ADJUST_CONST_FLOAT(gravity, -9.0f);
    ADJUST_CONST_FLOAT(a, 0.1f);
    // ADJUST_CONST_FLOAT(b, 0.1f);
    // ADJUST_CONST_FLOAT(c, 0.1f);
    // ADJUST_CONST_FLOAT(d, 0.1f);
    // ADJUST_CONST_FLOAT(e, 0.f);

    for (countdown = 3; countdown > 0; countdown--)
    {
        printf("You have %lu seconds to adjust...\n", countdown);

        sleep(1);
    }

    adjust_update();
    printf("gravity: %f\n", gravity);
    printf("a: %f\n", a);
    // printf("b: %f\n", b);
    // printf("c: %f\n", c);
    // printf("d: %f\n", d);
    // printf("e: %f\n", e);

    adjust_end();
}