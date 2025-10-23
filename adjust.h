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
 * Documentation:
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
 * Todo:
 *
 * - [ ] I shouldn't need two separate approaches for temp variables and main
 *       variables. Just malloc and make the interface way easier to work with.
 *
 * - [ ] adjust_register_global_float could maybe use adjust_register_global
 *       typeof, and then fail on unsupported type
 *
 * - [ ] Store file modification times, and only re-read when necessary
 *
 * - [ ] I think if you do ADJUST_VAR_FLOAT(a, 2.0f) and then something
 *       like deregister_short, then everything could work. It means adding
 *       supporting remove in dynamic arrays. It's a litle obnoxious, though.
 *
 * - [ ] Adjust update and with a second more targeted adjust_update_file so
 *       that users can be really specific if they want to be. Also, add an
 *       example to show the difference.
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
 * FAQ:
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
 ******************************************************************************/

/******************************************************************************/
/* bool */
#ifndef bool
typedef int bool;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif
#else

#ifndef TRUE
#define TRUE true
#endif

#ifndef FALSE
#define FALSE false
#endif
#endif

/******************************************************************************/

#ifdef MODE_PRODUCTION
/* In production mode, compile everything away */
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

#define ADJUST_GLOBAL_CONST_BOOL(name, val) const float name = val
#define ADJUST_GLOBAL_CONST_CHAR(name, val) const float name = val
#define ADJUST_GLOBAL_CONST_INT(name, val) const float name = val
#define ADJUST_GLOBAL_CONST_FLOAT(name, val) const float name = val
#define ADJUST_GLOBAL_CONST_STING(name, val) const float name = val

#define ADJUST_GLOBAL_VAR_BOOL(name, val) float name = val
#define ADJUST_GLOBAL_VAR_CHAR(name, val) float name = val
#define ADJUST_GLOBAL_VAR_INT(name, val) float name = val
#define ADJUST_GLOBAL_VAR_FLOAT(name, val) float name = val
#define ADJUST_GLOBAL_VAR_STING(name, val) float name = val

#define adjust_register_global_bool(name) ()
#define adjust_register_global_char(name) ()
#define adjust_register_global_float(name) ()
#define adjust_register_global_int(name) ()
#define adjust_register_global_string(name) ()

#define ADJUST_BOOL(v) (v)
#define ADJUST_CHAR(v) (v)
#define ADJUST_INT(v) (v)
#define ADJUST_FLOAT(v) (v)
#define ADJUST_STRING(v) (v)

#define adjust_init() ()
#define adjust_update() ()
#define adjust_update_file() ()
#define adjust_cleanup() ()

#else
/* In debug mode the user can adjust everything */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/* Generic Dynamic Array, not built for outside use */
typedef struct _DA_Header
{
    size_t length;
    size_t capacity;
    size_t item_size;
} _DA_Header;

static void *_da_init(const size_t item_size, const size_t capacity)
{
    void *ptr = 0;
    _DA_Header *h =
        (_DA_Header *)malloc(item_size * capacity + sizeof(_DA_Header));

    if (h)
    {
        h->length = 0;
        h->capacity = capacity;
        h->item_size = item_size;
        ptr = h + 1;
    }
    else
    {
        fprintf(stderr, "Unable to initialize dynamic array with malloc.\n");
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

        h = (_DA_Header *)realloc(h, h->item_size * new_capacity +
                                         sizeof(_DA_Header));
        if (!h)
        {
            fprintf(stderr, "Unable to resize dynamic array with realloc.\n");
            exit(1);
        }

        h->capacity = new_capacity;
        *da = h + 1;
    }
}

static size_t _da_priority_insert(void **da, const size_t priority,
                                  int (*compare)(const void *, const size_t))
{
    size_t i, insert_index;
    insert_index = 1;

    _DA_Header *h = ((_DA_Header *)(*da) - 1);
    char *bytes = (char *)(*da);

    _da_ensure_capacity(da, 1);
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
        printf("Moving data!\n");
        memmove(bytes + ((insert_index + 1) * h->item_size),
                bytes + (insert_index * h->item_size),
                (h->length - insert_index) * h->item_size);
    }

    ++h->length;

    return insert_index;
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
        free(((_DA_Header *)(da)-1));
    }
}

/******************************************************************************/
/**************** Adjust ****************/
/* Structs and global state with _files */
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
    const char *file_name;
    _ADJUST_ENTRY *adjustables;
} _ADJUST_FILE;

_ADJUST_FILE *_files;

/* helper for comparing priority of _ADJUST_ENTRY entries. Note, though that
 * this will faill if line_number is greater than INT_MAX */
static inline int _adjust_priority_compare(const void *element,
                                           const size_t priority)
{

    const _ADJUST_ENTRY *ae = (const _ADJUST_ENTRY *)element;
    return (int)ae->line_number - (int)priority;
}

/* Declarations for long-lived adjustable data */
static void _adjust_register(void *val, _ADJUST_TYPE type,
                             const char *file_name, const size_t line_number)
{
    _ADJUST_ENTRY *adjustables;
    bool found = FALSE;
    size_t file_index, i;

    const size_t length = _da_length(_files);
    for (file_index = 0; file_index < length; ++file_index)
    {
        if (strcmp(file_name, _files[file_index].file_name) == 0)
        {
            found = TRUE;
            break;
        }
    }

    if (!found)
    {
        _da_ensure_capacity((void **)&_files, 1);
        _da_increment_length(_files);

        _files[file_index].file_name = file_name;
        _files[file_index].adjustables =
            (_ADJUST_ENTRY *)_da_init(sizeof(_ADJUST_ENTRY), 4);

        adjustables = _files[file_index].adjustables;
        adjustables[0].type = type;
        adjustables[0].line_number = line_number;
        adjustables[0].should_cleanup = (type == _ADJUST_STRING);
        adjustables[0].data = val; /* char**, not char* for string */

        _da_increment_length(_files[file_index].adjustables);
    }
    else
    {
        i = _da_priority_insert((void **)&_files[file_index].adjustables,
                                line_number, _adjust_priority_compare);

        printf("priority: %lu\n", i);

        adjustables = _files[file_index].adjustables;
        adjustables[i].type = type;
        adjustables[i].line_number = line_number;
        adjustables[i].should_cleanup = (type == _ADJUST_STRING);
        adjustables[i].data = val; /* char**, not char* for string */
    }
}

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
        name = malloc(sizeof(char) * _len);                                    \
        memcpy(name, _temp, _len);                                             \
        _adjust_register(&name, _ADJUST_STRING, __FILE__, __LINE__);           \
    } while (0)

#define ADJUST_CONST_STRING(name, val)                                         \
    char *name = NULL;                                                         \
    do                                                                         \
    {                                                                          \
        const char *_temp = val;                                               \
        size_t _len = strlen(_temp) + 1;                                       \
        name = malloc(sizeof(char) * _len);                                    \
        memcpy(name, _temp, _len);                                             \
        _adjust_register(&name, _ADJUST_STRING, __FILE__, __LINE__);           \
    } while (0)

/* Declarations for global adjustable data */
static void _adjust_register_global(void *ref, _ADJUST_TYPE type,
                                    const char *file_name,
                                    const char *global_name)
{
    // TODO: Why do I need to read a file on every register? I don't know why I
    //       care about the name of the global variable.
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
    found = FALSE;
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
                    found = TRUE;
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
        name = malloc(sizeof(char) * _len);                                    \
        memcpy(name, _temp, _len);                                             \
        _adjust_register_global(&name, _ADJUST_STRING, __FILE__, #name);       \
    } while (0)

/* Declarations for temporary data */
void *_adjust_register_and_get(const _ADJUST_TYPE type, void *val,
                               const char *file_name, const size_t line_number)
{
    _ADJUST_ENTRY *adjustables;
    size_t file_index, i;
    const size_t num_files = _da_length(_files);

    for (file_index = 0; file_index < num_files; ++file_index)
    {
        // find file
        _ADJUST_FILE af = _files[file_index];
        if (strcmp(af.file_name, file_name) != 0)
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

        i = _da_priority_insert((void **)&adjustables, line_number,
                                _adjust_priority_compare);

        adjustables = af.adjustables;
        adjustables[i].type = type;
        adjustables[i].line_number = line_number;
        adjustables[i].should_cleanup = TRUE;

        if (type == _ADJUST_STRING)
        {
            adjustables[i].data = malloc(sizeof(char *));
            char **str_ptr = (char **)adjustables[i].data;
            *str_ptr = malloc(strlen((char *)val) + 1);
            strcpy(*str_ptr, (char *)val);
        }
        else
        {
            const size_t size = _adjust_type_to_size(type);
            adjustables[i].data = malloc(size);
            memcpy(adjustables[i].data, val, size);
        }

        return adjustables[i].data; /* early return */
    }

    if (file_index == num_files)
    {
        _da_ensure_capacity((void **)&_files, 1);

        _files[file_index].file_name = file_name;
        _files[file_index].adjustables =
            (_ADJUST_ENTRY *)_da_init(sizeof(_ADJUST_ENTRY), 4);

        adjustables = _files[file_index].adjustables;
        adjustables[0].type = type;
        adjustables[0].line_number = line_number;
        adjustables[0].should_cleanup = TRUE;

        _da_increment_length(_files);

        if (type == _ADJUST_STRING)
        {
            adjustables[0].data = malloc(sizeof(char *));
            char **str_ptr = (char **)adjustables[0].data;
            *str_ptr = malloc(strlen((char *)val) + 1);
            strcpy(*str_ptr, (char *)val);
        }
        else
        {
            const size_t size = _adjust_type_to_size(type);
            adjustables[0].data = malloc(size);
            memcpy(adjustables[0].data, val, size);
        }

        _da_increment_length(_files[file_index].adjustables);

        return adjustables[0].data;
    }

    fprintf(stderr,
            "Error: unhandled control flow pattern _adjust_register_and_get "
            "for args %s:%lu\n",
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
    _files = (_ADJUST_FILE *)_da_init(sizeof(_ADJUST_FILE), 4);
}

static void adjust_update_file(const char *file_name)
{
    // TODO
}

static void adjust_update(void)
{
    _ADJUST_FILE af;
    _ADJUST_ENTRY e;
    size_t file_index, data_index, data_length;
    FILE *file;
    char *value_start;
    char buffer[256];
    size_t current_line;

    const size_t length = _da_length(_files);
    for (file_index = 0; file_index < length; ++file_index)
    {
        af = _files[file_index];

        file = fopen(af.file_name, "r");
        if (file == NULL)
        {
            fprintf(stderr,
                    "Error: unable to "
                    "open file: %s\n",
                    af.file_name);
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
                    fprintf(stderr,
                            "Error: EOF "
                            "before line "
                            "%lu in %s\n",
                            e.line_number, af.file_name);
                    fclose(file);
                    exit(1);
                }

                ++current_line;
            }

            if (strstr(buffer, "ADJUST_VAR_") ||
                strstr(buffer, "ADJUST_CONST_") ||
                strstr(buffer, "ADJUST_GLOBAL_"))
            {
                value_start = strchr(buffer, ',');
                if (value_start == NULL)
                {
                    fprintf(stderr,
                            "Error: no comma "
                            "found in ADJUST "
                            "macro: "
                            "%s:%lu\n",
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
                    fprintf(stderr,
                            "Error: no "
                            "opening paren "
                            "found: %s:%lu\n",
                            af.file_name, e.line_number);
                    fclose(file);
                    exit(1);
                }
            }
            else
            {
                fprintf(stderr,
                        "Error: unrecognized "
                        "ADJUST macro format: "
                        "%s:%lu\n",
                        af.file_name, e.line_number);
                fclose(file);
                exit(1);
            }

            /* skip white space after ','
             */
            ++value_start;
            while (*value_start &&
                   (*value_start == ' ' || *value_start == '\t'))
            {
                ++value_start;
            }

            switch (e.type)
            {
            case _ADJUST_FLOAT:
            {
                if (sscanf(value_start, "%f", (float *)e.data) != 1)
                {
                    fprintf(stderr,
                            "Error: failed to "
                            "parse float: "
                            "%s:%lu\n",
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
                    fprintf(stderr,
                            "Error, failed to "
                            "parse int: "
                            "%s:%lu\n",
                            af.file_name, e.line_number);
                    fclose(file);
                    exit(1);
                }

                break;
            }

            case _ADJUST_BOOL:
            {
                if (*value_start == '0' ||
                    strncmp(value_start, "TRUE", 4) == 0 ||
                    strncmp(value_start, "true", 4) == 0)
                {
                    *(bool *)e.data = TRUE;
                }
                else if (*value_start == '1' ||
                         strncmp(value_start, "FALSE", 5) == 0 ||
                         strncmp(value_start, "false", 5) == 0)
                {
                    *(bool *)e.data = FALSE;
                }
                else
                {
                    fprintf(stderr,
                            "Error: failed to "
                            "parse bool (true "
                            "or false): "
                            "%s:%lu\n",
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
                            "Error: failed to "
                            "find starting "
                            "quotation (\'): "
                            "%s:%lu\n",
                            af.file_name, e.line_number);
                    fclose(file);
                    exit(1);
                }

                ++quote_start;
                if (*quote_start == '\'')
                {
                    fprintf(stderr,
                            "Error: char "
                            "format '' "
                            "invalid in "
                            "C, %s:%lu\n",
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
                    fprintf(stderr,
                            "Error: missing "
                            "ending ' for "
                            "char, %s:%lu\n",
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
                            "Error: failed to "
                            "find starting "
                            "quotation (\"): "
                            "%s:%lu\n",
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
                            "Error: failed to "
                            "find ending "
                            "quotation (\"): "
                            "%s:%lu\n",
                            af.file_name, e.line_number);
                    fclose(file);
                    exit(1);
                }

                string_length = (size_t)(quote_end - quote_start);
                new_string = realloc(*(char **)e.data, string_length + 1);
                if (!new_string)
                {
                    fprintf(stderr,
                            "Error: failed to "
                            "reallocate "
                            "string memory: "
                            "%s:%lu\n",
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
                fprintf(stderr,
                        "Error: unhandled "
                        "adjust type: %u\n",
                        e.type);
                fclose(file);
                exit(1);
            }
        }

        fclose(file);
    }
}

static void adjust_cleanup(void)
{
    _ADJUST_ENTRY *adjustables;
    size_t i, j, num_adjustables;
    const size_t length = _da_length(_files);
    for (i = 0; i < length; ++i)
    {
        adjustables = _files[i].adjustables;
        num_adjustables = _da_length(_files[i].adjustables);
        for (j = 0; j < num_adjustables; ++j)
        {
            if (adjustables[i].should_cleanup)
            {
                free(adjustables[i].data);
                adjustables[i].data = NULL;
            }
        }

        _da_free(adjustables);
    }

    _da_free(_files);
}
#endif

#endif
