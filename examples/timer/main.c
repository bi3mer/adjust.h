#include "adjust.h"

#include <sys/unistd.h>
#include <unistd.h>

int main(void)
{
    size_t countdown;
    adjust_init();

    ADJUST_CONST_FLOAT(gravity, -9.8f);
    ADJUST_VAR_FLOAT(a, 1.1f);

    ADJUST_CONST_INT(b, 30);
    ADJUST_VAR_INT(c, 1);

    ADJUST_CONST_BOOL(d, FALSE);
    ADJUST_VAR_BOOL(e, TRUE);

    ADJUST_CONST_STRING(f, "Hello,");
    ADJUST_VAR_STRING(g, "World!");

    for (countdown = 10; countdown > 0; countdown--)
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
