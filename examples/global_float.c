#include "adjust.h"
#include <unistd.h>

ADJUST_GLOBAL_CONST_FLOAT(G1, 0.8f);
ADJUST_GLOBAL_CONST_FLOAT(G2, 2.8f);

int main(void)
{
    adjust_init();

    adjust_register_global_float(G1); // TODO: temp
    adjust_register_global_float(G2); // TODO: temp

    ADJUST_CONST_FLOAT(m1, 2.0f);
    ADJUST_CONST_FLOAT(m2, 2.0f);
    ADJUST_CONST_FLOAT(d, 10.5f);

    // TODO:
    // adjust_register_global(&G, _ADJUST_FLOAT); // ordering doesn't matter!

    for (size_t countdown = 5; countdown > 0; countdown--)
    {
        printf("You have %lu seconds to adjust...\n", countdown);
        sleep(1);
    }

    printf("Before G1: F = %f *%f*%f / %f = %f\n", G1, m1, m2, d,
           G1 * m1 * m2 / d);
    printf("Before G2: F = %f *%f*%f / %f = %f\n", G2, m1, m2, d,
           G2 * m1 * m2 / d);
    adjust_update();
    printf("After G1: F = %f *%f*%f / %f = %f\n", G1, m1, m2, d,
           G1 * m1 * m2 / d);
    printf("After G2: F = %f *%f*%f / %f = %f\n", G2, m1, m2, d,
           G2 * m1 * m2 / d);

    adjust_cleanup();
    return 0;
}
