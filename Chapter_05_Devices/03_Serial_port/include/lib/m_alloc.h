/*! Dynamic memory allocator - first fit (simple)
 *
 * In this implementation double linked list are used.
 * Free list is not sorted. Search is started from first element until chunk
 * with adequate size is found (same or greater than required).
 * When chunk is freed, first join is tried with left and right neighbor chunk
 * (by address). If not joined, chunk is marked as free and put at list start.
 */

#pragma once

#ifdef MEM_TEST
#include "test/test.h"
#endif
#include <types/basic.h>
#define INCREMENT 32
#define LOG2_INCR msb_index(INCREMENT)
//#define N (8 * sizeof(int) - LOG2_INCR)
#define N 32

#define IND_PUT(SIZE)	((SIZE) < (INCREMENT) ? 0 : 1 + msb_index (SIZE) - LOG2_INCR)
#define IND_GET(SIZE)	( 1 + IND_PUT ( (SIZE) -1 ) )




#ifndef _FF_SIMPLE_C_

typedef void m_mpool_t;

/*! interface */
void *m_init ( void *mem_segm, size_t size );
void *m_alloc ( m_mpool_t *mpool, size_t size );
int m_free ( m_mpool_t *mpool, void *chunk_to_be_freed );

/*! rest is only for first_fit.c */
#else /* _FF_SIMPLE_C_ */

/* free chunk header (in use chunk header is just 'size') */
typedef struct _m_hdr_t_
{
	size_t		     size;
			     /* chunk size, including head and tail headers */
	struct _m_hdr_t_  *prev;
			     /* previous free in list */
	struct _m_hdr_t_  *next;
			     /* next free in list */
}
m_hdr_t;

/* chunk tail (and header for in use chunks) */
typedef struct _m_tail_t_
{
	size_t  size;
		/* chunk size, including head and tail headers */
}
m_tail_t;

typedef struct _m_mpool_t_
{
	m_hdr_t *first[N];
}
m_mpool_t;

#define HEADER_SIZE ( sizeof (m_hdr_t) + sizeof (m_tail_t) )

/* use LSB of 'size' to mark chunk as used (otherwise size is always even) */
#define MARK_USED(HDR)	do { (HDR)->size |= 1;  } while(0)
#define MARK_FREE(HDR)	do { (HDR)->size &= ~1; } while(0)

#define CHECK_USED(HDR)	((HDR)->size & 1)
#define CHECK_FREE(HDR)	!CHECK_USED(HDR)

#define GET_SIZE(HDR)	((HDR)->size & ~1)

#define GET_AFTER(HDR)	(((void *) (HDR)) +  GET_SIZE(HDR))
#define GET_TAIL(HDR)	(GET_AFTER(HDR) - sizeof (m_tail_t))
#define GET_HDR(TAIL)	(((void *)(TAIL)) - GET_SIZE(TAIL) + sizeof(m_tail_t))

#define CLONE_SIZE_TO_TAIL(HDR)	\
	do { ( (m_tail_t *) GET_TAIL(HDR) )->size = (HDR)->size; } while(0)

#define ALIGN_VAL	( (size_t) sizeof(size_t) )
#define ALIGN_MASK	( ~( ALIGN_VAL - 1 ) )
#define ALIGN(P)	\
	do { (P) = ALIGN_MASK & ( (size_t) (P) ); } while(0)
#define ALIGN_FW(P)	\
	do { (P) = ALIGN_MASK & (((size_t) (P)) + (ALIGN_VAL - 1)) ; } while(0)

void *m_init ( void *mem_segm, size_t size );
void *m_alloc ( m_mpool_t *mpool, size_t size );
int m_free ( m_mpool_t *mpool, void *chunk_to_be_freed );

static void m_remove_chunk ( m_mpool_t *mpool, m_hdr_t *chunk );
static void m_insert_chunk ( m_mpool_t *mpool, m_hdr_t *chunk );

#endif /* _FF_SIMPLE_C_ */
