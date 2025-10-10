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
 * If you compile in debug mode (e.g., `cmake -DCMAKE_BUILD_TYPE=Debug ..`),
 *then the gravity will be a variable that can be changed. If you compile in
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
 * - [ ] Support float
 * - [ ] Support bool
 * - [ ] Support char*
 * - [ ] Support int
 *
 * Ideal:
 * - [ ] store file modification times, and only re-read when necessary
 * - [ ] threaded option, one thread per file. Will need to be lightweight,
 *       though
 ******************************************************************************/

#ifdef MODE_PRODUCTION
/* In production mode, compile everything away */
#define ADJUST_VAR(T, name, val) T name = val
#define ADJUST_CONST(T, name, val) const T name = val

#define adjust_begin() ()
#define adjust_update() ()
#define adjust_end() ()

#else
/* In debug mode the user can adjust everything */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef bool
typedef int bool;
#define TRUE 1
#define FALSE 0
#endif

/******************************************************************************/
/* Generic Dynamic Array, not built for outside use */
typedef struct ___DA_Header
{
    size_t length;
    size_t capacity;
} ___DA_Header;

static void *___da_init(const size_t item_size, const size_t capacity)
{
    void *ptr = 0;
    ___DA_Header *h =
        (___DA_Header *)malloc(item_size * capacity + sizeof(___DA_Header));

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

static void ___da_ensure_capacity(void **da, size_t capacity_increase,
                                  size_t item_size)
{
    ___DA_Header *h = ((___DA_Header *)(*da) - 1);
    if (h->length + capacity_increase > h->capacity)
    {
        size_t new_capacity = h->capacity * 2;
        while (h->length + capacity_increase > new_capacity)
        {
            new_capacity *= 2;
        }

        const size_t new_size = item_size * new_capacity + sizeof(___DA_Header);
        h = (___DA_Header *)realloc(h, new_size);
        if (!h)
        {
            fprintf(stderr, "Unable to resize dynamic array with realloc.\n");
            exit(1);
        }

        h->capacity = new_capacity;
        *da = h + 1;
    }
}

#define _a_da_append(da, val)                                                  \
    do                                                                         \
    {                                                                          \
        ___da_ensure_capacity((void **)&(da), 1, sizeof(*da));                 \
        da[((___DA_Header *)(da) - 1)->length] = (val);                        \
        ((___DA_Header *)(da) - 1)->length++;                                  \
    } while (0)

static size_t _a_da_length(void *da)
{
    if (!da)
        return 0;
    return ((___DA_Header *)da - 1)->length;
}

static size_t _a_da_capacity(void *da)
{
    if (!da)
        return 0;
    return ((___DA_Header *)da - 1)->capacity;
}

static void _a_da_free(void *da)
{
    if (da)
    {
        free(((___DA_Header *)(da)-1));
    }
}

/******************************************************************************/
/* Adjust */
typedef enum
{
    __ADJUST_FLOAT = 0,
    __ADJUST_INT,
    __ADJUST_BOOL,
    __ADJUST_STRING
} __ADJUST_TYPE;

typedef struct __ADJUST_ENTRY
{
    __ADJUST_TYPE type;
    size_t line_number;
    void *data;
} __ADJUST_ENTRY;

typedef struct __ADJUST_FILE
{
    char *file_name;
    __ADJUST_ENTRY *adjustables;
    // size_t *line_numbers;
    // __ADJUST_TYPE* types;
    // void **data;
} __ADJUST_FILE;

__ADJUST_FILE *__a_floats;

void _adjust_add_float(float *val, const char *file_name, const int line_number)
{
    bool found = FALSE;
    size_t i;
    const size_t length = _a_da_length(__a_floats);
    for (i = 0; i < length; ++i)
    {
        if (strcmp(file_name, __a_floats[i].file_name) == 0)
        {
            found = TRUE;
            break;
        }
    }

    if (!found)
    {
        __ADJUST_FILE new_file;
        new_file.file_name = (char *)file_name;
        new_file.line_numbers = (size_t *)_a_da_init(size_t, 4);
        new_file.data = (float **)_a_da_init(float *, 4);

        _a_da_append(__a_floats, new_file);
    }

    _a_da_append(__a_floats[i].line_numbers, line_number);
    _a_da_append(__a_floats[i].data, val);
}

#define ADJUST_VAR(T, name, val)                                               \
    T name = val;                                                              \
    _adjust_add(&name, __FILE__, __LINE__)

#define ADJUST_CONST(T, name, val)                                             \
    T name = val;                                                              \
    _adjust_add(&name, __FILE__, __LINE__)

static void adjust_begin(void)
{
    __a_floats = _a_da_init(__ADJUST_FILE, 2);
    __a_floats->line_numbers = _a_da_init(size_t, 8);
    __a_floats->data = _a_da_init(float, 8);
}

static void adjust_update(void)
{
    size_t file_index, data_index, data_length;
    __ADJUST_FILE af;
    FILE *file;
    char *adjust_pos;
    char buffer[256];
    int current_line;

    const size_t length = _a_da_length(__a_floats);
    for (file_index = 0; file_index < length; ++file_index)
    {
        af = __a_floats[file_index];

        file = fopen(af.file_name, "r");
        if (file == NULL)
        {
            fprintf(stderr, "Error: unable to open file: %s\n", af.file_name);
            exit(1);
        }

        data_length = _a_da_length(af.line_numbers);
        for (data_index = 0; data_index < data_length; ++data_index)
        {
            current_line = 1;
            while (current_line < af.line_numbers[data_index] &&
                   fgets(buffer, sizeof(buffer), file))
            {
                ++current_line;
            }

            if (fgets(buffer, sizeof(buffer), file))
            {
                adjust_pos = strstr(buffer, ");");
                if (adjust_pos == NULL)
                {
                    fprintf(stderr,
                            "Error, unable to find valid end of line `);`: "
                            "%s:%lu\n",
                            af.file_name, af.line_numbers[data_index]);
                    fclose(file);
                    exit(1);
                }

                // TODO: error handling to prevent infinite loop via bad editing
                // from developer
                while (*adjust_pos != ' ')
                {
                    --adjust_pos;
                }

                ++adjust_pos;
                // TODO: use the type of the adjustable to decide how to scan
                // it. I think something speciall will have to be done for bool
                // since we are looking for true or false, else we error out.
                if (sscanf(adjust_pos, "%f", af.data[data_index]) != 1)
                {
                    fprintf(stderr, "Error, failed to parse float: %s:%lu\n",
                            af.file_name, af.line_numbers[data_index]);
                    fclose(file);
                    exit(1);
                }
            }
            else
            {
                printf("Error getting tweak value: %s:%lu\n", af.file_name,
                       af.line_numbers[data_index]);
                fclose(file);
                exit(1);
            }
        }

        fclose(file);
    }
}

static void adjust_end()
{
    _a_da_free(__a_floats);
}
#endif

#endif
