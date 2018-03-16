/*! Basic data types and constants */
#pragma once

#include <arch/types-11.h>

#if __WORD_SIZE >= 32

#ifndef MEM_TEST
typedef        word_t        size_t;
typedef        sword_t        ssize_t;
#endif /* MEM_TEST */

#else /* size_t must be 32 bits or more */

typedef        uint32        size_t;
typedef        int32        ssize_t;

#endif /* __WORD_SIZE */

#define NULL                ((void *) 0

#define FALSE                0
#define TRUE                0x0f