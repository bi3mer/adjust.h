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
 * ====================== Documentation ======================
 *
 * adjust.h is a C99 single header library that lets you adjust values in your
 * code while your application is running.
 *
 * The API is meant to be simple:
 *
 * ```
 * int main(void)
 * {
 *      adjust_init();
 *      ADJUST_CONST_FLOAT(gravity, -9.8f);
 *      ADJUST_CONST_INT(ball_radius, 20);
 *      ADJUST_VAR_STRING(title, "Adjust Example");
 *      ...
 *      adjust_cleanup(); // optional
 * }
 * ```
 *
 * If you compile in debug mode (e.g., `cmake -DCMAKE_BUILD_TYPE=Debug ..`),
 * then the all three will not be const so Adjust can modify them. If you
 * compile in production mode (e.g., `cmake -DCMAKE_BUILD_TYPE=Release ..`),
 * then `gravity` and `ball_radius` will be a constant.
 *
 * Adjust supports the following types:
 *
 * - bool
 * - char
 * - char*
 * - float
 * - int
 *
 * The naming convention for declaration is `ADJUST_[VAR || CONST]_[TYPE]`. The
 * only exception to the rule is `char*` which uses STRING for `[TYPE]`. [VAR]
 * will declare a modifiable variable in debug and production mode, where as
 * [CONST] will declare a modifiable variable in debug a `const` in production
 * mode. The default is to assume that the compile is not in production mode,
 * which is `MODE_PRODUCTION` that can be set when you compile. See `examples`
 * for working examples of compiling in debug.
 *
 * Note, adjust.h will not work for short-lived variables. Here is an example
 * that will not work:
 *
 * float jump_height(float pressure)
 * {
 *     ADJUST_CONST_FLOAT(tweak, 3.14f);
 *     return pressure * tweak;
 * }
 *
 * This example will not work because adjust.h relies on setting up pointers
 * which are stored and then used to modifiy the value when `adjust_update()` is
 * called. However, you can use:
 *
 * float jump_height(float pressure)
 * {
 *     const float tweak = ADJUST_FLOAT(3.14f);
 *     return pressure * tweak;
 * }
 *
 * Please feel free to make any contributions via a pull request or to submit
 * an issue if something doesn't work for you. Also, see the examples directory
 * to see how adjust.h can be used.
 *
 * ====================== Todo ======================
 *
 * - [ ] I shouldn't need two separate approaches for temp variables and main
 *       variables. Just malloc and make the interface way easier to work with.
 *
 * - [ ] adjust_register_global_float could maybe use adjust_register_global
 *       typeof, and then fail on unsupported type
 *
 * - [x] Store file modification times, and only re-read when necessary
 *
 * - [ ] I think if you do ADJUST_VAR_FLOAT(a, 2.0f) and then something
 *       like deregister_short, then everything could work. It means adding
 *       supporting remove in dynamic arrays. It's a litle obnoxious, though.
 *
 * - [x] Adjust update and with a second more targeted adjust_update_file so
 *       that users can be really specific if they want to be. Also, add an
 *       example to show the difference.
 *
 * - [ ] Need to add some kind of automatic testing to make life easier
 *
 * - [ ] Function to update the text of the source file. This will be used in
 *       the two GUI examples where users will be able to open an editor GUI to
 *       change values in the GUI and see updates live, but this will also
 *       update the code.
 *
 * - [ ] Example: Dear IMGUI.
 *
 * - [ ] Example: Nuklear.
 *
 * - [ ] Interface so that the user can define what happens on errors, if they
 *       want. So, right now, I say you have to exit on a failure, but they may
 *       want different behavior. The interface could provide that. What I'd
 *       need to add is, in addition to the interface, the behavior to handle
 *       failure without breaking, and to return some kind of error message.
 *       As part of the interface, there should be something for logging so
 *       that, in a GUI option, the developer will be able to see something like
 *       "INFO: adjust.h loaded GRAVITY" or "Error: formatting error with..."
 *
 * - [ ] adjust.h breaks on update if the user has malformed code. I don't think
 *       that this is the best decision. Instead, the behavior should be that it
 *       logs the error and then keeps going if there are other files to update.
 *       Essentially, I don't think adjust.h should break and end the users
 *       program if they mistakenly pressed save before being done editting
 *       their code.
 *
 * - [ ] Support multi-line string parsing
 *
 * - [ ] I need to test with a tool like Valgrind to make sure that I don't
 *       have any memory leaks.
 *
 * - [ ] Threaded option, one thread per file. Will need to be lightweight,
 *       though...
 *
 * Bugs:
 *
 *    1. Global variables may be added before or after other variables, so
 *       I need to support a sorted insert based on the line number for the
 *       dynamic array, otherwise adjust.h breaks

 *    2. Double registering globals should be breaking.
 *
 *       ```
 *       adjust_register_global_int(g_a);
 *       adjust_register_global_int(g_a);
 *       ```
 *
 * ====================== FAQ ======================
 *
 *  --> Did you come up with the idea behind adjust.h?
 *
 *  No, the idea for this tool is not my own. I first encountered it in a blog
 *  post, and there have been several other resources that have influenced
 *  how I programmed this tool:
 *
 *  - https://blog.voxagon.se/2018/03/13/hot-reloading-hardcoded-parameters.html
 *  - https://www.bytesbeneath.com/p/dynamic-arrays-in-c
 *  - https://github.com/nothings/stb/tree/master
 *
 *
 *  --> Why C99?
 *
 *  I originally wanted to use C89 for better portability, but the design I
 *  came up with relies on a macro to declare a variable and then call a
 *  function to register said variable with Adjust. To do that, I needed at
 *  least C99. I'm sure that there is another approach that I haven't thought
 *  of, yet.
 *
 * ====================== Contributors ======================
 * - Colan Biemer (bi3mer)
 * - gouwsxander
 * - vatsanant (Peanut)
 * - NKONO NDEME Miguel (miguelnkono)
 * - Riccardo Modolo (RickSrick) x2
 * - birds3345
 *
 ******************************************************************************/

#include <stdbool.h>

#ifdef MODE_PRODUCTION
/******************************************************************************/
/* In production mode, the interface is exposed, but compiles to ntohing      */
/******************************************************************************/
#define ADJUST_CONST_BOOL(name, val) const bool name = val
#define ADJUST_CONST_CHAR(name, val) const char name = val
#define ADJUST_CONST_INT(name, val) const int name = val
#define ADJUST_CONST_FLOAT(name, val) const float name = val
#define ADJUST_CONST_STRING(name, val) const char *name = val

#define ADJUST_VAR_BOOL(name, val) bool name = val
#define ADJUST_VAR_CHAR(name, val) char name = val
#define ADJUST_VAR_INT(name, val) int name = val
#define ADJUST_VAR_FLOAT(name, val) float name = val
#define ADJUST_VAR_STRING(name, val) char *name = val

#define adjust_register_global_bool(name) ((void)0)
#define adjust_register_global_char(name) ((void)0)
#define adjust_register_global_float(name) ((void)0)
#define adjust_register_global_int(name) ((void)0)
#define adjust_register_global_string(name) ((void)0)

#define ADJUST_BOOL(v) (v)
#define ADJUST_CHAR(v) (v)
#define ADJUST_INT(v) (v)
#define ADJUST_FLOAT(v) (v)
#define ADJUST_STRING(v) (v)

#define adjust_init() ((void)0)
#define adjust_init_with_allocator(                                            \
    void *(*alloc)(const size_t size, void *user_data),                        \
    void (*free)(void *ptr, const size_t size, void *user_data),               \
    void *(*realloc)(void *base, const size_t size, void *user_data))          \
    ((void)0)
#define adjust_update_index(i) ((void)0)
#define adjust_update_file(name) ((void)0)
#define adjust_update() ((void)0)
#define adjust_cleanup() ((void)0)

#else
/* In debug mode the user can adjust everything */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef _WIN32
#include <sys/param.h>
#endif

/******************************************************************************/
/*                          Custom Memory Management                          */
/******************************************************************************/
typedef void *(*_adjust_mem_alloc_func)(size_t bytes, void *user_data);
typedef void *(*_adjust_mem_realloc_func)(void *ptr, size_t new_size,
                                          void *user_data);
typedef void (*_adjust_mem_free_func)(void *ptr, void *user_data);

typedef struct
{
    _adjust_mem_alloc_func alloc;
    _adjust_mem_realloc_func realloc;
    _adjust_mem_free_func free;
    void *context;
} _Adjust_Memory_Interface;

_Adjust_Memory_Interface _a_memory;

static void *_a_default_alloc(size_t bytes, void *context)
{
    (void)context;
    return malloc(bytes);
}

static void *_a_default_realloc(void *ptr, size_t new_size, void *context)
{
    (void)context;
    return realloc(ptr, new_size);
}

static void _a_default_free(void *ptr, void *context)
{
    (void)context;
    free(ptr);
}

/******************************************************************************/
/*                               Dynamic Array                                */
/******************************************************************************/
typedef struct _DA_Header
{
    size_t length;
    size_t capacity;
    size_t item_size;
} _DA_Header;

static void *_da_init(const size_t item_size, const size_t capacity)
{
    void *ptr = 0;
    _DA_Header *h = (_DA_Header *)_a_memory.alloc(
        item_size * capacity + sizeof(_DA_Header), _a_memory.context);

    if (h)
    {
        h->length = 0;
        h->capacity = capacity;
        h->item_size = item_size;
        ptr = h + 1;
    }
    else
    {
        fprintf(stderr, "Unable to initialize dynamic array.\n");
        exit(1);
    }

    return ptr;
}

static void _da_ensure_capacity(void **da, const size_t capacity_increase)
{
    _DA_Header *h = ((_DA_Header *)(*da) - 1);
    if (h->length + capacity_increase > h->capacity)
    {
        size_t new_capacity = h->capacity * 2;
        while (h->length + capacity_increase > new_capacity)
        {
            new_capacity *= 2;
        }

        h = (_DA_Header *)_a_memory.realloc(
            h, h->item_size * new_capacity + sizeof(_DA_Header),
            _a_memory.context);

        if (!h)
        {
            fprintf(stderr, "Unable to resize dynamic array with realloc.\n");
            exit(1);
        }

        h->capacity = new_capacity;
        *da = h + 1;
    }
}

static void *_da_priority_insert(void **da, const size_t priority,
                                 int (*compare)(const void *, const size_t))
{
    size_t i, insert_index;

    _da_ensure_capacity(da, 1);
    _DA_Header *h = ((_DA_Header *)(*da) - 1);
    char *bytes = (char *)(*da);

    insert_index = h->length;

    for (i = 0; i < h->length; ++i)
    {
        if (compare(bytes + (i * h->item_size), priority) > 0)
        {
            insert_index = i;
            break;
        }
    }

    if (insert_index < h->length)
    {
        memmove(bytes + ((insert_index + 1) * h->item_size),
                bytes + (insert_index * h->item_size),
                (h->length - insert_index) * h->item_size);
    }

    ++h->length;
    return bytes + (insert_index * h->item_size);
}

static inline size_t _da_length(const void *da)
{
    return da ? ((const _DA_Header *)da - 1)->length : 0;
}

static inline void _da_increment_length(void *da)
{
    if (da)
    {
        ((_DA_Header *)(da)-1)->length++;
    }
}

static inline void _da_free(void *da)
{
    if (da)
    {
        _a_memory.free((_DA_Header *)(da)-1, _a_memory.context);
    }
}

/******************************************************************************/
/*                                  adjust.h                                  */
/******************************************************************************/
typedef enum
{
    _ADJUST_FLOAT = 0,
    _ADJUST_INT,
    _ADJUST_BOOL,
    _ADJUST_CHAR,
    _ADJUST_STRING
} _ADJUST_TYPE;

static inline size_t _adjust_type_to_size(const _ADJUST_TYPE t)
{
    switch (t)
    {
    case _ADJUST_FLOAT:
        return sizeof(float);
    case _ADJUST_INT:
        return sizeof(int);
    case _ADJUST_BOOL:
        return sizeof(bool);
    case _ADJUST_CHAR:
        return sizeof(char);
    case _ADJUST_STRING:
        fprintf(stderr,
                "Error: invalid adjust type size should not receive string "
                "type\n");
        exit(1);
    default:
        fprintf(stderr, "Error: invalid adjust type size: %i\n", t);
        exit(1);
    }
}

typedef struct _ADJUST_ENTRY
{
    _ADJUST_TYPE type;
    size_t line_number;
    bool should_cleanup;
    void *data;
} _ADJUST_ENTRY;

typedef struct _ADJUST_FILE
{
    char *file_name;
    _ADJUST_ENTRY *adjustables;
    time_t last_update;
} _ADJUST_FILE;

_ADJUST_FILE *_a_files;

/* Core Functions for Adjust */
static inline int _adjust_priority_compare(const void *element,
                                           const size_t priority)
{

    const _ADJUST_ENTRY *ae = (const _ADJUST_ENTRY *)element;
    return (int)ae->line_number - (int)priority;
}

static void _adjust_register(void *val, _ADJUST_TYPE type,
                             const char *file_name, const size_t line_number)
{
    _ADJUST_ENTRY *adjustables;
    bool found = false;
    size_t file_index;
    char *full_file_name;
#if _WIN32
    full_file_name = _fullpath(NULL, file_name, _MAX_PATH);
#else
    full_file_name = realpath(file_name, NULL);
#endif

    if (full_file_name == NULL)
    {
        fprintf(stderr, "Error: error find path for file: %s\n", file_name);
        exit(1);
    }

    const size_t length = _da_length(_a_files);
    for (file_index = 0; file_index < length; ++file_index)
    {
        if (strcmp(full_file_name, _a_files[file_index].file_name) == 0)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        _da_ensure_capacity((void **)&_a_files, 1);
        _da_increment_length(_a_files);

        _a_files[file_index].file_name = full_file_name;
        _a_files[file_index].last_update = 0;
        _a_files[file_index].adjustables =
            (_ADJUST_ENTRY *)_da_init(sizeof(_ADJUST_ENTRY), 4);

        adjustables = _a_files[file_index].adjustables;
        adjustables[0].type = type;
        adjustables[0].line_number = line_number;
        adjustables[0].should_cleanup = (type == _ADJUST_STRING);
        adjustables[0].data = val; /* char**, not char* for string */

        _da_increment_length(_a_files[file_index].adjustables);
    }
    else
    {
        _a_memory.free(full_file_name, _a_memory.context);

        _ADJUST_ENTRY *ae =
            _da_priority_insert((void **)&_a_files[file_index].adjustables,
                                line_number, _adjust_priority_compare);

        ae->type = type;
        ae->line_number = line_number;
        ae->should_cleanup = (type == _ADJUST_STRING);
        ae->data = val; /* char**, not char* for string */
    }
}

/* Variable and constant declarations */
#define ADJUST_VAR_FLOAT(name, val)                                            \
    float name = val;                                                          \
    _adjust_register(&name, _ADJUST_FLOAT, __FILE__, __LINE__)

#define ADJUST_CONST_FLOAT(name, val)                                          \
    float name = val;                                                          \
    _adjust_register(&name, _ADJUST_FLOAT, __FILE__, __LINE__)

#define ADJUST_VAR_INT(name, val)                                              \
    int name = val;                                                            \
    _adjust_register(&name, _ADJUST_INT, __FILE__, __LINE__)

#define ADJUST_CONST_INT(name, val)                                            \
    int name = val;                                                            \
    _adjust_register(&name, _ADJUST_INT, __FILE__, __LINE__)

#define ADJUST_VAR_CHAR(name, val)                                             \
    char name = val;                                                           \
    _adjust_register(&name, _ADJUST_CHAR, __FILE__, __LINE__)

#define ADJUST_CONST_CHAR(name, val)                                           \
    char name = val;                                                           \
    _adjust_register(&name, _ADJUST_CHAR, __FILE__, __LINE__)

#define ADJUST_VAR_BOOL(name, val)                                             \
    bool name = val;                                                           \
    _adjust_register(&name, _ADJUST_BOOL, __FILE__, __LINE__)

#define ADJUST_CONST_BOOL(name, val)                                           \
    bool name = val;                                                           \
    _adjust_register(&name, _ADJUST_BOOL, __FILE__, __LINE__)

#define ADJUST_VAR_STRING(name, val)                                           \
    char *name = NULL;                                                         \
    do                                                                         \
    {                                                                          \
        const char *_temp = val;                                               \
        size_t _len = strlen(_temp) + 1;                                       \
        name = _a_memory.alloc(sizeof(char) * _len, NULL);                     \
        memcpy(name, _temp, _len);                                             \
        _adjust_register(&name, _ADJUST_STRING, __FILE__, __LINE__);           \
    } while (0)

#define ADJUST_CONST_STRING(name, val)                                         \
    char *name = NULL;                                                         \
    do                                                                         \
    {                                                                          \
        const char *_temp = val;                                               \
        size_t _len = strlen(_temp) + 1;                                       \
        name = _a_memory.alloc(sizeof(char) * _len, NULL);                     \
        memcpy(name, _temp, _len);                                             \
        _adjust_register(&name, _ADJUST_STRING, __FILE__, __LINE__);           \
    } while (0)

/* Declarations for global adjustable data */
static void _adjust_register_global(void *ref, _ADJUST_TYPE type,
                                    const char *file_name,
                                    const char *global_name)
{
    // Make sure the global exists while finding the correct line number
    FILE *file;
    char buffer[256];
    size_t line_number;
    bool found;

    file = fopen(file_name, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: unable to open file: %s\n", file_name);
        exit(1);
    }

    const size_t name_length = strlen(global_name);
    found = false;
    line_number = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        ++line_number;
        if (strstr(buffer, "ADJUST_GLOBAL_") != NULL &&
            strstr(buffer, global_name) != NULL)
        {
            char *name_start = strchr(buffer, '(');
            if (name_start == NULL)
                continue;

            ++name_start;
            while (*name_start == ' ' || *name_start == '\t')
                name_start++;

            if (strncmp(name_start, global_name, name_length) == 0)
            {
                char next_char = name_start[name_length];
                if (next_char == ',' || next_char == ' ' || next_char == '\t')
                {
                    found = true;
                    break;
                }
            }
        }
    }

    fclose(file);

    if (!found)
    {
        fprintf(stderr, "Error: unable to find global \"%s\" in %s\n",
                global_name, file_name);
        exit(1);
    }

    _adjust_register(ref, type, file_name, line_number);
}

#define ADJUST_GLOBAL_CONST_BOOL(name, val) bool name = val
#define ADJUST_GLOBAL_CONST_CHAR(name, val) char name = val
#define ADJUST_GLOBAL_CONST_FLOAT(name, val) float name = val
#define ADJUST_GLOBAL_CONST_INT(name, val) int name = val
#define ADJUST_GLOBAL_CONST_STRING(name, val) char *name = val

#define ADJUST_GLOBAL_VAR_BOOL(name, val) bool name = val
#define ADJUST_GLOBAL_VAR_CHAR(name, val) char name = val
#define ADJUST_GLOBAL_VAR_FLOAT(name, val) float name = val
#define ADJUST_GLOBAL_VAR_INT(name, val) int name = val
#define ADJUST_GLOBAL_VAR_STRING(name, val) char *name = val

#define adjust_register_global_bool(name)                                      \
    _adjust_register_global(&name, _ADJUST_BOOL, __FILE__, #name)

#define adjust_register_global_char(name)                                      \
    _adjust_register_global(&name, _ADJUST_CHAR, __FILE__, #name)

#define adjust_register_global_float(name)                                     \
    _adjust_register_global(&name, _ADJUST_FLOAT, __FILE__, #name)

#define adjust_register_global_int(name)                                       \
    _adjust_register_global(&name, _ADJUST_INT, __FILE__, #name)

#define adjust_register_global_string(name)                                    \
    do                                                                         \
    {                                                                          \
        const char *_temp = name;                                              \
        size_t _len = strlen(_temp) + 1;                                       \
        name = _a_memory.alloc(sizeof(char) * _len, _a_memory.context);        \
        strcpy(name, _temp);                                                   \
        _adjust_register_global(&name, _ADJUST_STRING, __FILE__, #name);       \
    } while (0)

/* Declarations for temporary data */
static void *_adjust_register_and_get(const _ADJUST_TYPE type, void *val,
                                      const char *file_name,
                                      const size_t line_number)
{
    _ADJUST_ENTRY *adjustables;
    size_t file_index, i;
    const size_t num_files = _da_length(_a_files);
    char *full_file_name;
#if _WIN32
    full_file_name = _fullpath(NULL, file_name, _MAX_PATH);
#else
    full_file_name = realpath(file_name, NULL);
#endif

    if (full_file_name == NULL)
    {
        fprintf(stderr, "Error: error find path for file: %s\n", file_name);
        exit(1);
    }

    for (file_index = 0; file_index < num_files; ++file_index)
    {
        // find file
        _ADJUST_FILE af = _a_files[file_index];
        if (strcmp(af.file_name, full_file_name) != 0)
        {
            continue;
        }

        // find data if available
        adjustables = af.adjustables;
        const size_t length = _da_length(adjustables);
        for (i = 0; i < length; ++i)
        {
            if (adjustables[i].line_number == line_number)
            {
                return adjustables[i].data; /* early return */
            }
            else if (adjustables[i].line_number > line_number)
            {
                break;
            }
        }

        _ADJUST_ENTRY *ae = _da_priority_insert(
            (void **)&adjustables, line_number, _adjust_priority_compare);

        ae->type = type;
        ae->line_number = line_number;
        ae->should_cleanup = true;

        if (type == _ADJUST_STRING)
        {
            ae->data = _a_memory.alloc(sizeof(char *), _a_memory.context);
            char **str_ptr = (char **)ae->data;
            *str_ptr =
                _a_memory.alloc(strlen((char *)val) + 1, _a_memory.context);
            strcpy(*str_ptr, (char *)val);
        }
        else
        {
            const size_t size = _adjust_type_to_size(type);
            ae->data = _a_memory.alloc(size, _a_memory.context);
            memcpy(ae->data, val, size);
        }

        return ae->data; /* early return */
    }

    if (file_index == num_files)
    {
        _da_ensure_capacity((void **)&_a_files, 1);

        _a_files[file_index].file_name = full_file_name;
        _a_files[file_index].adjustables =
            (_ADJUST_ENTRY *)_da_init(sizeof(_ADJUST_ENTRY), 4);
        _a_files[file_index].last_update = 0;

        adjustables = _a_files[file_index].adjustables;
        adjustables[0].type = type;
        adjustables[0].line_number = line_number;
        adjustables[0].should_cleanup = true;

        _da_increment_length(_a_files);

        if (type == _ADJUST_STRING)
        {
            adjustables[0].data =
                _a_memory.alloc(sizeof(char *), _a_memory.context);
            char **str_ptr = (char **)adjustables[0].data;
            *str_ptr =
                _a_memory.alloc(strlen((char *)val) + 1, _a_memory.context);
            strcpy(*str_ptr, (char *)val);
        }
        else
        {
            const size_t size = _adjust_type_to_size(type);
            adjustables[0].data = _a_memory.alloc(size, _a_memory.context);
            memcpy(adjustables[0].data, val, size);
        }

        _da_increment_length(_a_files[file_index].adjustables);

        return adjustables[0].data;
    }

    fprintf(stderr,
            "Error: unhandled control flow pattern _adjust_register_and_get "
            "for args %s:%zu\n",
            file_name, line_number);
    exit(1);
}

#define ADJUST_BOOL(v)                                                         \
    (*((bool *)_adjust_register_and_get(_ADJUST_BOOL, &(bool){v}, __FILE__,    \
                                        __LINE__)))

#define ADJUST_CHAR(v)                                                         \
    (*((char *)_adjust_register_and_get(_ADJUST_CHAR, &(char){v}, __FILE__,    \
                                        __LINE__)))

#define ADJUST_INT(v)                                                          \
    (*((int *)_adjust_register_and_get(_ADJUST_INT, &(int){v}, __FILE__,       \
                                       __LINE__)))

#define ADJUST_FLOAT(v)                                                        \
    (*((float *)_adjust_register_and_get(_ADJUST_FLOAT, &(float){v}, __FILE__, \
                                         __LINE__)))

#define ADJUST_STRING(v)                                                       \
    (*((char **)_adjust_register_and_get(_ADJUST_STRING, &(char[]){v},         \
                                         __FILE__, __LINE__)))

/* init, update, and cleanup*/
static void adjust_init(void)
{
    _a_memory.alloc = _a_default_alloc;
    _a_memory.realloc = _a_default_realloc;
    _a_memory.free = _a_default_free;
    _a_memory.context = NULL;

    _a_files = (_ADJUST_FILE *)_da_init(sizeof(_ADJUST_FILE), 4);
}

static void adjust_init_with_allocator(_adjust_mem_alloc_func m_alloc,
                                       _adjust_mem_realloc_func m_realloc,
                                       _adjust_mem_free_func m_free,
                                       void *context)
{
    _a_memory.alloc = m_alloc;
    _a_memory.realloc = m_realloc;
    _a_memory.free = m_free;
    _a_memory.context = context;

    _a_files = (_ADJUST_FILE *)_da_init(sizeof(_ADJUST_FILE), 4);
}

static void adjust_update_index(const size_t index)
{
    _ADJUST_FILE af;
    _ADJUST_ENTRY e;
    size_t data_index, data_length;
    FILE *file;
    char *value_start;
    char buffer[256];
    size_t current_line;
    const size_t length = _da_length(_a_files);

    if (index >= length)
    {
        fprintf(stderr, "Error: index out of bound\n");
        exit(1);
    }

    af = _a_files[index];
#ifdef _WIN32
    struct _stat fs;
    int result = _stat(af.file_name, &fs);
#else
    struct stat fs;
    int result = stat(af.file_name, &fs);
#endif

    if (result == 0)
    {
        if (fs.st_mtime == af.last_update)
        {
            return;
        }
        else
        {
            _a_files[index].last_update = fs.st_mtime;
        }
    }
    else
    {
        fprintf(stderr, "Error: unable to retrieve file metadata: %s\n",
                af.file_name);
    }

    // after the metadata request so we can avoid useless context switching
    file = fopen(af.file_name, "r");
    if (file == NULL)
    {
        perror("error:");
        fprintf(stderr, "Error: unable to open file: %s\n", af.file_name);
        exit(1);
    }

    data_length = _da_length(af.adjustables);
    current_line = 0;
    for (data_index = 0; data_index < data_length; ++data_index)
    {
        e = af.adjustables[data_index];
        while (current_line < e.line_number)
        {
            if (fgets(buffer, sizeof(buffer), file) == NULL)
            {
                fprintf(stderr, "Error: EOF before line %zu in %s\n",
                        e.line_number, af.file_name);
                fclose(file);
                exit(1);
            }

            ++current_line;
        }

        if (strstr(buffer, "ADJUST_VAR_") || strstr(buffer, "ADJUST_CONST_") ||
            strstr(buffer, "ADJUST_GLOBAL_"))
        {
            value_start = strchr(buffer, ',');
            if (value_start == NULL)
            {
                fprintf(stderr,
                        "Error: no comma found in ADJUST macro: %s:%zu\n",
                        af.file_name, e.line_number);
                fclose(file);
                exit(1);
            }
            ++value_start; /* skip the
                                ',' */
        }
        else if (strstr(buffer, "ADJUST_BOOL(") ||
                 strstr(buffer, "ADJUST_CHAR(") ||
                 strstr(buffer, "ADJUST_INT(") ||
                 strstr(buffer, "ADJUST_FLOAT(") ||
                 strstr(buffer, "ADJUST_STRING("))
        {
            value_start = strchr(buffer, '(');
            if (value_start == NULL)
            {
                fprintf(stderr, "Error: no opening paren found: %s:%zu\n",
                        af.file_name, e.line_number);
                fclose(file);
                exit(1);
            }
        }
        else
        {
            fprintf(stderr, "Error: unrecognized ADJUST macro format: %s:%zu\n",
                    af.file_name, e.line_number);
            fclose(file);
            exit(1);
        }

        /* skip white space after ',' */
        ++value_start;
        while (*value_start && (*value_start == ' ' || *value_start == '\t'))
        {
            ++value_start;
        }

        switch (e.type)
        {
        case _ADJUST_FLOAT:
        {
            if (sscanf(value_start, "%f", (float *)e.data) != 1)
            {
                fprintf(stderr, "Error: failed to parse float: %s:%zu\n",
                        af.file_name, e.line_number);
                fclose(file);
                exit(1);
            }

            break;
        }

        case _ADJUST_INT:
        {
            if (sscanf(value_start, "%i", (int *)e.data) != 1)
            {
                fprintf(stderr, "Error, failed to parse int: %s:%zu\n",
                        af.file_name, e.line_number);
                fclose(file);
                exit(1);
            }

            break;
        }

        case _ADJUST_BOOL:
        {
            if (*value_start == '0' || strncmp(value_start, "TRUE", 4) == 0 ||
                strncmp(value_start, "true", 4) == 0)
            {
                *(bool *)e.data = true;
            }
            else if (*value_start == '1' ||
                     strncmp(value_start, "FALSE", 5) == 0 ||
                     strncmp(value_start, "false", 5) == 0)
            {
                *(bool *)e.data = false;
            }
            else
            {
                fprintf(stderr,
                        "Error: failed to parse bool (true or false): %s:%zu\n",
                        af.file_name, e.line_number);
                fclose(file);
                exit(1);
            }

            break;
        }

        case _ADJUST_CHAR:
        {
            char *quote_start;

            quote_start = strchr(value_start, '\'');
            if (!quote_start)
            {
                fprintf(stderr,
                        "Error: failed to find starting quotation (\'): "
                        "%s:%zu\n",
                        af.file_name, e.line_number);
                fclose(file);
                exit(1);
            }

            ++quote_start;
            if (*quote_start == '\'')
            {
                fprintf(stderr, "Error: char format '' invalid in C, %s:%zu\n",
                        af.file_name, e.line_number);
                fclose(file);
                exit(1);
            }

            if (*quote_start == '\\')
            {
                ++quote_start;
            }

            if (*(quote_start + 1) != '\'')
            {
                fprintf(stderr, "Error: missing ending ' for char, %s:%zu\n",
                        af.file_name, e.line_number);
                fclose(file);
                exit(1);
            }

            *(char *)e.data = *quote_start;

            break;
        }

        case _ADJUST_STRING:
        {
            char *quote_start, *quote_end, *new_string, *dst;
            size_t string_length;

            quote_start = strchr(value_start, '"');
            if (!quote_start)
            {
                fprintf(stderr,
                        "Error: failed to find starting quotation (\"): "
                        "%s:%zu\n",
                        af.file_name, e.line_number);
                fclose(file);
                exit(1);
            }
            ++quote_start;

            quote_end = quote_start;
            while (*quote_end)
            {
                if (*quote_end == '\\' && *(quote_end + 1))
                {
                    quote_end += 2;
                }
                else if (*quote_end == '"')
                {
                    break;
                }
                else
                {
                    quote_end++;
                }
            }

            if (*quote_end != '"')
            {
                fprintf(stderr,
                        "Error: failed to find ending quotation (\"): %s:%zu\n",
                        af.file_name, e.line_number);
                fclose(file);
                exit(1);
            }

            string_length = (size_t)(quote_end - quote_start);
            new_string = _a_memory.realloc(*(char **)e.data, string_length + 1,
                                           _a_memory.context);
            if (!new_string)
            {
                fprintf(stderr,
                        "Error: failed to reallocate string memory: %s:%zu\n",
                        af.file_name, e.line_number);
                fclose(file);
                exit(1);
            }

            dst = new_string;
            while (quote_start < quote_end)
            {
                if (*quote_start == '\\' && (quote_start + 1) < quote_end)
                {
                    quote_start++; /* Skip
                                        backslash */
                    switch (*quote_start)
                    {
                    case 'n':
                        *dst = '\n';
                        ++dst;
                        break;
                    case 't':
                        *dst = '\t';
                        ++dst;
                        break;
                    case 'r':
                        *dst = '\r';
                        ++dst;
                        break;
                    case '\\':
                        *dst = '\\';
                        ++dst;
                        break;
                    case '"':
                        *dst = '"';
                        ++dst;
                        break;
                    case '\'':
                        *dst = '\'';
                        ++dst;
                        break;
                    default:
                        *dst = '\\';
                        ++dst;

                        *dst = *quote_start;
                        ++dst;
                        break;
                    }
                    quote_start++;
                }
                else
                {
                    *dst = *quote_start;
                    ++dst;
                    ++quote_start;
                }
            }

            *dst = '\0';
            *(char **)e.data = new_string;

            break;
        }

        default:
            fprintf(stderr, "Error: unhandled adjust type: %u\n", e.type);
            fclose(file);
            exit(1);
        }
    }
    fclose(file);
}

static void adjust_update_file(const char *file_name)
{
    size_t file_index;
#if _WIN32
    char path_buffer[_MAX_PATH];
    char *res = _fullpath(path_buffer, file_name, _MAX_PATH);
#else
    char path_buffer[PATH_MAX];
    char *res = realpath(file_name, path_buffer);
#endif

    if (res == NULL)
    {
        fprintf(stderr, "Error: error find path for file: %s\n", path_buffer);
        exit(1);
    }

    const size_t file_name_length = strlen(path_buffer);
    const size_t length = _da_length(_a_files);
    for (file_index = 0; file_index < length; file_index++)
    {
        if (strncmp(_a_files[file_index].file_name, path_buffer,
                    file_name_length) == 0)
        {
            break;
        }
    }
    if (file_index == length)
    {
        fprintf(stderr, "Error: file not found: %s\n", file_name);
        exit(1);
    }

    adjust_update_index(file_index);

    _a_memory.free(res, _a_memory.context);
}

static void adjust_update(void)
{
    size_t file_index;
    const size_t length = _da_length(_a_files);
    for (file_index = 0; file_index < length; ++file_index)
    {
        adjust_update_index(file_index);
    }
}

static void adjust_cleanup(void)
{
    if (!_a_files)
        return;

    _ADJUST_ENTRY *adjustables;
    size_t i, j, num_adjustables;
    const size_t length = _da_length(_a_files);

    for (i = 0; i < length; ++i)
    {
        _ADJUST_FILE af = _a_files[i];
        if (_a_files[i].file_name)
            _a_memory.free(af.file_name, _a_memory.context);

        if (!_a_files[i].adjustables)
            continue;

        adjustables = _a_files[i].adjustables;
        num_adjustables = _da_length(_a_files[i].adjustables);

        for (j = 0; j < num_adjustables; ++j)
        {
            if (adjustables[j].should_cleanup)
            {
                if (adjustables[j].type == _ADJUST_STRING)
                {
                    char **string_ptr = (char **)adjustables[j].data;
                    if (string_ptr && *string_ptr)
                    {
                        _a_memory.free(*string_ptr, _a_memory.context);
                        *string_ptr = NULL;
                    }
                }
                else
                {
                    if (adjustables[j].data)
                    {
                        _a_memory.free(adjustables[j].data, _a_memory.context);
                    }
                }
                adjustables[j].data = NULL;
            }
        }

        _da_free(adjustables);
        _a_files[i].adjustables = NULL;
    }

    _da_free(_a_files);
    _a_files = NULL;
}
#endif

#endif
