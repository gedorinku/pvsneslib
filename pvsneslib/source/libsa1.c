/*---------------------------------------------------------------------------------

    Copyright (C) 2012-2021
        Alekmaul

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:

    1.	The origin of this software must not be misrepresented; you
        must not claim that you wrote the original software. If you use
        this software in a product, an acknowledgment in the product
        documentation would be appreciated but is not required.
    2.	Altered source versions must be plainly marked as such, and
        must not be misrepresented as being the original software.
    3.	This notice may not be removed or altered from any source
        distribution.

---------------------------------------------------------------------------------*/
/*
 *  linux/lib/vsprintf.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/* vsprintf.c -- Lars Wirzenius & Linus Torvalds. */
/*
 * Wirzenius wrote this portably, Torvalds fucked it up :-)
 */

/*
 * Fri Jul 13 2001 Crutcher Dunnavant <crutcher+kernel@datastacks.com>
 * - changed to provide snprintf and vsnprintf functions
 */

/* simple malloc()/free() implementation
 * adapted from here: http://www.flipcode.com/archives/Simple_Malloc_Free_Functions.shtml
 */

#include <stdarg.h>
#include <string.h>
#include <limits.h>

extern volatile void (*call_s_cpu_func)();
extern volatile int call_s_cpu_args_size;
extern volatile char call_s_cpu_args[];
extern volatile char call_s_cpu_return[];
extern volatile int call_s_cpu_return_flag;

void call_s_cpu(void (*func)(), size_t ret_size, void *ret, size_t args_size, ...)
{
    call_s_cpu_func = func;
    call_s_cpu_args_size = args_size;

    va_list args;
    va_start(args, args_size);
    memcpy(&call_s_cpu_args, args, args_size);
    va_end(args);

    call_s_cpu_return_flag = 0;

    // IRQ
    *(char *)(0x2209) = 0xd0;

    while (!call_s_cpu_return_flag) {}

    if (ret != NULL) {
        memcpy(ret, call_s_cpu_return, ret_size);
    }
}

/**
 * @brief Check if a character is a decimal digit.
 *
 * @param c The character to check.
 * @return Nonzero if the character is a decimal digit, zero otherwise.
 */
int isdigit(int c)
{
    if (c >= '0' && c <= '9')
        return 1;
    else
        return 0;
}

/**
 * @brief Check if a character is a hexadecimal digit.
 *
 * @param c The character to check.
 * @return Nonzero if the character is a hexadecimal digit, zero otherwise.
 */
int isxdigit(int c)
{
    if (isdigit(c) ||
        (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F'))
        return 1;
    else
        return 0;
}

/**
 * @brief Check if a character is a lowercase letter.
 *
 * @param c The character to check.
 * @return Nonzero if the character is a lowercase letter, zero otherwise.
 */
int islower(int c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    else
        return 0;
}

/**
 * @brief Convert a lowercase letter to uppercase.
 *
 * @param c The character to convert.
 * @return The uppercase equivalent of the input character, or the input character if it is not a lowercase letter.
 */
int toupper(int c)
{
    if (islower(c))
        return c - ('a' - 'A');
    else
        return c;
}

/**
 * @brief Check if a character is a whitespace character.
 *
 * @param c The character to check.
 * @return Nonzero if the character is a whitespace character, zero otherwise.
 */
int isspace(int c)
{
    if (c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r')
        return 1;
    else
        return 0;
}

/**
 * @brief Determines whether the given character is printable or not.
 *
 * This function checks whether the given character is a printable ASCII character
in the range of 0x20 to 0x7E (inclusive).
 * @param c The character to be checked for printability.
 * @return Returns 1 if the character is printable, and 0 otherwise.
 */
int isprint(int c)
{
    if (c >= ' ' && c <= '~')
        return 1;
    else
        return 0;
}
/**
 * @def USED
 *
 * @brief A macro used to mark memory blocks as used in the memory allocation algorithm.
 */
#define USED 1

/**
 * @brief The memory block header used by the custom memory allocation implementation.
 *
 * This structure represents the header of each memory block that is allocated using the custom memory allocation implementation. It only contains the size of the block, which is used to manage the allocation and deallocation of memory.
 */
typedef struct
{
    unsigned int size; /**< The size of the memory block, including the header. */
} unit;

/**
 * @brief Memory system structure that holds the free and heap units.
 */
typedef struct
{
    unit *free; /**< Pointer to the beginning of the free unit */
    unit *heap; /**< Pointer to the beginning of the heap unit */
} msys_t;

/**
 * @brief The memory system instance.
 *
 * This global variable is an instance of the memory system structure used by the malloc() and free() functions to manage memory.
 */
static msys_t msys;

/**
 * @brief Compacts the free memory blocks in the heap to ensure contiguous space for future allocations.
 *
 * This function searches the heap for consecutive free blocks of memory and merges them together. If a block of
memory can fit the required size, it is returned to the caller. If the heap is not large enough to contain
such a block of memory, the function returns NULL.
 * @param p Pointer to the start of the heap.
 * @param nsize The size of the memory block to allocate.
 * @return A pointer to the allocated block of memory, or NULL if the heap is not large enough.
 */
static unit *__compact(unit *p, unsigned int nsize)
{
    unsigned int bsize, psize;
    unit *best;

    best = p;
    bsize = 0;

    while (psize = p->size, psize)
    {
        if (psize & USED)
        {
            if (bsize != 0)
            {
                best->size = bsize;
                if (bsize >= nsize)
                {
                    return best;
                }
            }
            bsize = 0;
            best = p = (unit *)((void *)p + (psize & ~USED));
        }
        else
        {
            bsize += psize;
            p = (unit *)((void *)p + psize);
        }
    }

    if (bsize != 0)
    {
        best->size = bsize;
        if (bsize >= nsize)
        {
            return best;
        }
    }

    return 0;
}

/**
 * @brief Frees a block of memory that was previously allocated using malloc().
 *
 * @param ptr A pointer to the beginning of the block to free.
 */
void sa1_free(void *ptr)
{
    if (ptr)
    {
        unit *p;

        p = (unit *)((void *)ptr - sizeof(unit));
        p->size &= ~USED;
    }
}

/**
 * @brief Allocates a block of memory of the specified size.
 *
 * @param size The size of the block to allocate.
 * @return void* Returns a pointer to the beginning of the allocated block, or
NULL if the allocation failed.
 */
void *sa1_malloc(unsigned int size)
{
    unsigned int fsize;
    unit *p;

    if (size == 0)
        return 0;

    size += 3 + sizeof(unit);
    size >>= 2;
    size <<= 2;

    if (msys.free == 0 || size > msys.free->size)
    {
        msys.free = __compact(msys.heap, size);
        if (msys.free == 0)
            return 0;
    }

    p = msys.free;
    fsize = msys.free->size;

    if (fsize >= size + sizeof(unit))
    {
        msys.free = (unit *)((void *)p + size);
        msys.free->size = fsize - size;
    }
    else
    {
        msys.free = 0;
        size = fsize;
    }

    p->size = size | USED;

    return (void *)((void *)p + sizeof(unit));
}

/**
 * @brief Initializes a dynamic memory allocation system.
 *
 * This function initializes a dynamic memory allocation system using the specified heap of the given length.
It sets the free pointer and the heap pointer to the start of the heap, and sets the size of both pointers to the length of the heap minus the size of a memory unit.
It also sets the last word of the heap to zero to indicate the end of the heap.
 * @param heap A pointer to the start of the heap.
 * @param len The length of the heap.
 */
void sa1_malloc_init(void *heap, unsigned int len)
{
    len >>= 2;
    len <<= 2;
    msys.free = msys.heap = (unit *)heap;
    msys.free->size = msys.heap->size = len - sizeof(unit);
    *(unsigned int *)((char *)heap + len - sizeof(unit)) = 0;
}

/**
 * @brief Compacts the memory heap by consolidating adjacent free blocks.
 *
 * This function compacts the memory heap by consolidating adjacent free blocks.
 * It sets the free block pointer to point to the start of the newly compacted heap.
 */
void sa1_compact(void)
{
    msys.free = __compact(msys.heap, 0xffff);
}
