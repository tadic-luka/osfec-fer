/*!  Dynamic memory allocator - first fit */

#define _FF_SIMPLE_C_
#include <types/bits.h>
#include <lib/m_alloc.h>
#include <stdio.h>

#ifndef ASSERT
#include ASSERT_H
#endif

static void m_print(m_mpool_t *mpool, char *msg)
{
	int i, vel = 0;
	m_hdr_t *chunk;

	printf(">>>> %s\n", msg);
	for (i = 0; i < N; ++i) {
		kprintf("%d (%d): ", i, vel);
		chunk = mpool->first[i];
		while(chunk) {
			kprintf("adr:%x size:%d ", chunk, GET_SIZE(chunk));
			chunk = chunk->next;

		}
		kprintf("\n");
		vel = (i == 0 ? INCREMENT: vel * 2);
	}
	printf("<<<< %s\n", msg);
}

/*!
 * Initialize dynamic memory manager
 * \param mem_segm Memory pool start address
 * \param size Memory pool size
 * \return memory pool descriptor
*/
void *m_init ( void *mem_segm, size_t size )
{
	size_t start, end;
	m_hdr_t *chunk, *border;
	m_mpool_t *mpool;

	ASSERT ( mem_segm && size > sizeof (m_hdr_t) * 2 );

	/* align all on 'size_t' (if already not aligned) */
	start = (size_t) mem_segm;
	end = start + size;
	ALIGN_FW ( start );
	mpool = (void *) start;		/* place mm descriptor here */
	start += sizeof (m_mpool_t);
	ALIGN ( end );

	for(int i = 0; i < N; ++i) {
		mpool->first[i] = NULL;
	}
	/*mpool->first = NULL;*/

	if ( end - start < 2 * HEADER_SIZE )
		return NULL;

	border = (m_hdr_t *) start;
	border->size = sizeof (size_t);
	MARK_USED ( border );

	chunk = GET_AFTER ( border );
	chunk->size = end - start - 2 * sizeof(size_t);
	MARK_FREE ( chunk );
	CLONE_SIZE_TO_TAIL ( chunk );

	border = GET_AFTER ( chunk );
	border->size = sizeof (size_t);
	MARK_USED ( border );

	m_insert_chunk ( mpool, chunk ); /* [> first and only free chunk <] */

	return mpool;
}

/*!
 * Get free chunk with required size (or slightly bigger)
 * \param mpool Memory pool to be used (if NULL default pool is used)
 * \param size Requested chunk size
 * \return Block address, NULL if can't find adequate free chunk
 */
void *m_alloc ( m_mpool_t *mpool, size_t size )
{
	m_print( mpool, "prije dodjele");
	m_hdr_t *iter, *chunk;
	int i = IND_GET(size);

	ASSERT ( mpool );

	size += sizeof (size_t) * 2; /* add header and tail size */
	if ( size < HEADER_SIZE )
		size = HEADER_SIZE;

	/* align request size to higher 'size_t' boundary */
	ALIGN_FW ( size );

	iter = mpool->first[i];
	int first = i;
	do {
		iter = mpool->first[i];
		while(iter != NULL && iter->size < size)
			iter = iter->next;
		if( iter != NULL)
			break;
		++i;
	}while(i != first);
	if(iter == NULL)
		return NULL;
	/*while ( iter != NULL && iter->size < size )*/
		/*iter = iter->next;*/
	
	/*if ( iter == NULL )*/
		/*return NULL; [> no adequate free chunk found <]*/
	
	if ( iter->size >= size + HEADER_SIZE )
	{
		/* split chunk */
		/* first part remains in free list, just update size */
		// if splitted memory now belongs to another list
		if(i != IND_PUT(iter->size - size)) {
			m_remove_chunk(mpool, iter);
		}
		iter->size -= size;
		CLONE_SIZE_TO_TAIL ( iter );

		chunk = GET_AFTER ( iter );
		chunk->size = size;
		if(i != IND_PUT(iter->size))
			m_insert_chunk(mpool, iter);
	}
	else { /* give whole chunk */
		chunk = iter;

		/* remove it from free list */
		m_remove_chunk ( mpool, chunk );
	}

	MARK_USED ( chunk );
	CLONE_SIZE_TO_TAIL ( chunk );

	m_print( mpool, "poslije dodjele");
	return ( (void *) chunk ) + sizeof (size_t);
}

/*!
 * Free memory chunk
 * \param mpool Memory pool to be used (if NULL default pool is used)
 * \param chunk Chunk location (starting address)
 * \return 0 if successful, -1 otherwise
 */
int m_free ( m_mpool_t *mpool, void *chunk_to_be_freed )
{
	m_hdr_t *chunk, *before, *after;

	ASSERT ( mpool && chunk_to_be_freed );

	chunk = chunk_to_be_freed - sizeof (size_t);
	ASSERT ( CHECK_USED ( chunk ) );

	MARK_FREE ( chunk ); /* mark it as free */

	/* join with left? */
	before = ( (void *) chunk ) - sizeof(size_t);
	if ( CHECK_FREE ( before ) )
	{
		before = GET_HDR ( before );
		m_remove_chunk ( mpool, before );
		before->size += chunk->size; /* join */
		chunk = before;
	}

	/* join with right? */
	after = GET_AFTER ( chunk );
	if ( CHECK_FREE ( after ) )
	{
		m_remove_chunk ( mpool, after );
		chunk->size += after->size; /* join */
	}

	/* insert chunk in free list */
	m_insert_chunk ( mpool, chunk );

	/* set chunk tail */
	CLONE_SIZE_TO_TAIL ( chunk );
	m_print( mpool, "nakon oslobadjanja" );
	return 0;
}

/*!
 * Routine that removes an chunk from 'free' list (free_list)
 * \param mpool Memory pool to be used
 * \param chunk Chunk header
 */
static void m_remove_chunk ( m_mpool_t *mpool, m_hdr_t *chunk )
{
	int i = IND_PUT(chunk->size);
	if ( chunk == mpool->first[i] ) /* first in list? */
		mpool->first[i] = chunk->next;
	else
		chunk->prev->next = chunk->next;

	if ( chunk->next != NULL )
		chunk->next->prev = chunk->prev;
}

/*!
 * Routine that insert chunk into 'free' list (free_list)
 * \param mpool Memory pool to be used
 * \param chunk Chunk header
 */
static void m_insert_chunk ( m_mpool_t *mpool, m_hdr_t *chunk )
{
	int i = IND_PUT(chunk->size);
	chunk->next = mpool->first[i];
	chunk->prev = NULL;

	if ( mpool->first[i] )
		mpool->first[i]->prev = chunk;

	mpool->first[i] = chunk;
}

