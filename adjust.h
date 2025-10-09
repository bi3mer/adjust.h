/******************************************************************************
                                  MIT License
                         Copyright (c) 2025 Colan Biemer

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#ifndef __ADJUST__
#define __ADJUST__
/******************************************************************************
 * Documentation
 *
 * Adjust let's you modify values from code while your application is running.
 *
 * The API is meant to be simple:
 *
 * ```
 * int main(void)
 * {
 *      adjust_begin();
 *      ADJUST_FLOAT(gravity, -9.8f);
 *      ...
 *      adjust_end(); // optional call that will clean up memory
 * }
 * ```
 *
 * If you compile in debug mode (e.g., `cmake -DCMAKE_BUILD_TYPE=Debug ..`), then
 * the gravity will be a variable that can be changed. If you compile in
 * production mode (e.g., `cmake -DCMAKE_BUILD_TYPE=Release ..`), then gravity
 * will be a constant that can't be changed.
 *
 * I hope to update this down the line such that it Adjust can work with more
 * than just floats.
 *
 * The idea is not my own, I first encountered it in a blog post, and there
 * have been several other resources that have influences how I programmed this
 * tool:
 *
 *  - https://blog.voxagon.se/2018/03/13/hot-reloading-hardcoded-parameters.html
 *  - https://www.bytesbeneath.com/p/dynamic-arrays-in-c
 *  - https://github.com/nothings/stb/tree/master
 *
 * Please feel free to make any contributions via a pull request or to submit
 * an issue if something doesn't work for you. Also, see examples in the
 * repository for example usage.
 *
 * Minimum:
 * - [ ] Support bool
 * - [ ] Support char*
 * - [ ] Support int
 ******************************************************************************/


#include <stdlib.h>
#ifdef MODE_PRODUCTION
 /* In production mode, compile everything away */
 #define ADJUST_FLOAT(name, val) const float name = val

 #define adjust_begin() ()
 #define adjust_update() ()
 #define adjust_end() ()

 #else

 #include "stdio.h"
 #include "stdlib.h"
 #include "string.h"

/******************************************************************************/
/* Generic Dynamic Array, not built for outside use */
typedef struct ___DA_Header
{
    size_t length;
    size_t capacity;
} ___DA_Header;

static void* ___da_init(const size_t item_size, const size_t capacity)
{
    void *ptr = 0;
    ___DA_Header *h = (___DA_Header*) malloc(item_size * capacity + sizeof(___DA_Header));

    if (h)
    {
        h->capacity = capacity;
        h->length = 0;
        ptr = h + 1;
    }
    else
    {
        fprintf(stderr, "Unable to initialize dynamic array with malloc.\n");
        exit(1);
    }

    return ptr;
}
#define _a_da_init(T, capacity) ___da_init(sizeof(T), capacity)

static void ___da_ensure_capacity(void** da, size_t capacity_increase, size_t item_size)
{
    ___DA_Header* h = ((___DA_Header *)(*da) - 1);
    if (h->length + capacity_increase > h->capacity)
    {
        size_t new_capacity = h->capacity * 2;
        while (h->length + capacity_increase > new_capacity)
            new_capacity *= 2;

        printf("Realloc: %lu\n", new_capacity);
        const size_t new_size = item_size * new_capacity + sizeof(___DA_Header);
        h = (___DA_Header*) realloc(h, new_size);
        if (!h) {
            fprintf(stderr, "Unable to resize dynamic array with realloc.\n");
            exit(1);
        }

        h->capacity = new_capacity;
        *da = h + 1;
    }
}

#define _a_da_append(da, val)                                      \
    do {                                                           \
        ___da_ensure_capacity((void**)&(da), 1, sizeof(val));      \
        da[((___DA_Header *)(da) - 1)->length] = (val);            \
        ((___DA_Header *)(da) - 1)->length++;                      \
    } while(0)

static void _a_da_free(void* da)
{
    if (da) free(((___DA_Header *)(da) - 1));
}
/******************************************************************************/
/* Adjust */



 float _adjust_add_float(float* val, const char* file_name, const int line_number);
 #define ADJUST_FLOAT(name, val) float name = val; _adjust_add_float(&name, __FILE__, __LINE__)

 void adjust_update(void);
 #endif

#endif