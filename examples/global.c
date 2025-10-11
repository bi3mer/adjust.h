#include "adjust.h"
#include <unistd.h>

float PI = -3.14f;

int main(void)
{
    adjust_init();
    ADJUST_CONST_FLOAT(radius, 2.0f);
    adjust_register_global(&gravity, _ADJUST_FLOAT);

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("You have %lu seconds to adjust...\n", countdown);
        sleep(1);
    }

    adjust_update();

    printf("2 * %f * %f = %f\n", PI, radius, 2 * PI * radius);

    adjust_cleanup();
}
