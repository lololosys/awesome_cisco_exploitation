/* $Id: memory.c,v 1.3 1996/05/12 23:50:15 towu Exp $
 * $Source: /cscka/t1_neat/src/common/memory.c,v $
 *------------------------------------------------------------------
 * memory.c - neat's memory management
 *
 * Feb 1996, Tony Wu
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: memory.c,v $
 * Revision 1.3  1996/05/12 23:50:15  towu
 * Fixed bugs for ct1 rbs features.
 *
 * Revision 1.2  1996/05/09  21:40:01  towu
 * Added feature to support dynamic memory allocation
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef T1_FIRMWARE
#include 	"t1_headers.h"
#else
#include        "e1_headers.h"
#endif
#include        "as_memory.h"

/* global definitions */
static mem_queue 	mem_chunk;
static mem_queue 	mem_freelist;

/*
 * mem_freelist_remove -
 * remove an item from free list.
 */
static bool mem_freelist_remove (mem_header *item)
{
	if ( item->flags & MEM_BUSY )
		return FALSE;
        if ( mem_freelist.first == (mem_header *)&mem_freelist) {
		return FALSE;
        }
	if (mem_freelist.last == item)
		mem_freelist.last = item->free_prev;
	else
		item->free_next->free_prev = item->free_prev;
		
	if (mem_freelist.first == item)
		mem_freelist.first = item->free_next;
	else 
		item->free_prev->free_next = item->free_next;
	item->free_next = item->free_prev = NULL;
	item->flags |= MEM_BUSY;
	mem_freelist.count--;
	return TRUE;
}

/*
 * mem_freelist_enq -
 * put an item on to the end of the freelist queue.
 */
static void mem_freelist_enq (mem_header *item)
{
	if ( mem_freelist.count == 0 ) {
		mem_freelist.first = mem_freelist.last = item;
		item->free_prev = item->free_next = (mem_header *)&mem_freelist;
	}
	else {
		
		item->free_prev = mem_freelist.last; 
		item->free_next = (mem_header *)&mem_freelist;
		mem_freelist.last->free_next = item;
		mem_freelist.last = item;
	}
	mem_freelist.count++;
	item->flags &= ~MEM_BUSY;
}

/*
 * mem_chunk_create --
 * Add a new chunk of memory to memory chunk list
 */ 
static void mem_chunk_create (mem_header *item)
{
	mem_header *p;

	item->next = (mem_header *)&mem_chunk;
	p = mem_chunk.last;

	if ( p  == (mem_header *)&mem_chunk) {
		mem_chunk.last = item;
		mem_chunk.first = item;
		item->prev = (mem_header *)&mem_chunk;
        }
	else {
		p->next = item;
		item->prev = p;
		mem_chunk.last = item;
 	}
	item->magic = MEM_MAGIC_NUMBER;
	mem_chunk.count++;
}
/*
 * mem_insert_new_chunk -
 * insert an new block in chunk list
 * input:
 *	mem - header pointer where the new block will
 *		be inserted after.
 *	new_block - header pointer to be inserted.
 * return:
 * 	FALSE : chunk is not contiguous with previous
 *		one.
 *	TRUE : insert completed.
 *
 */
static bool mem_insert_new_chunk (mem_header *mem, mem_header *new_block)
{
	mem_header *next_chunk;
	typedef enum {
		QUE_NONE = 0,
		QUE_FIRST,
		QUE_LAST,
		QUE_MIDDLE
		} q_item_loc;
	q_item_loc status;


	/* check if the memory chunk is ok to be inserted in this place */
	if (mem != (mem_header *)&mem_chunk) {
		if ((char *)new_block != MEM_NEXT_CHUNK(mem))
			return FALSE;
		next_chunk = mem->next;
		status = QUE_MIDDLE;
	}
	else {
		next_chunk = mem_chunk.first;
		status = QUE_FIRST;
	}

	if (next_chunk != (mem_header *)&mem_chunk) { 
	   if ((char *)next_chunk !=  MEM_NEXT_CHUNK(new_block))
			return FALSE;
	} 
	else {
		if ( status == QUE_FIRST )
			status = QUE_NONE;
		else
			status = QUE_LAST;	
	}

	/* insert new block after mem */
	new_block->magic = MEM_MAGIC_NUMBER;
	if ( status == QUE_NONE) {
		mem_chunk.last = mem_chunk.first = new_block;
		new_block->prev = new_block->next = (mem_header *)&mem_chunk;
	}
	if (status == QUE_FIRST) {
		new_block->next = mem_chunk.first;
		new_block->prev = mem; 
		new_block->next->prev = new_block;
		mem_chunk.first = new_block;
	}
	if (status == QUE_LAST) {
		new_block->next = (mem_header *)&mem_chunk;
		new_block->prev = mem;
		mem->next = new_block;
		mem_chunk.last = new_block;
	}
	if (status == QUE_MIDDLE) {
		new_block->next = mem->next;
		new_block->prev = mem;
		mem->next->prev = new_block;
		mem->next = new_block;
	}			
	mem_chunk.count++;
	/* put in the freelist */
	mem_freelist_enq(new_block);
	return TRUE;
}


/*
 * mem_merge_prev_block -
 * 	merge with previous free block and return the previous header
 *	pointer.
 */
static mem_header *mem_merge_prev_block (mem_header *item)
{
	mem_header *p, *n;

	p = item->prev;
	n = item->next; 
	p->size += ( item->size + sizeof (mem_header));
	p->next = item->next;
	if ( n == (mem_header *)&mem_chunk)
		mem_chunk.last = p;
	else
		n->prev = p;
	/* remove the magic number */
	item->magic = 0;
	mem_chunk.count--;
	return p;
}

/*
 * mem_merge_next_block --
 * merge with the next bock
 */
static void mem_merge_next_block (mem_header *item)
{
	mem_header *n, *p;

	n = item->next;
	p = item->prev;
	/* extend current chunk to include next chunk */
	/* extend the size first */
	item->size += (n->size + sizeof (mem_header));
	/* remove next block from chunk list */
	item->next = n->next;
	if ( mem_chunk.last == n)
		mem_chunk.last = item;
	else
		n->next->prev = item;
	mem_chunk.count--;
	/* remove next block from free list */
	mem_freelist_remove(n);
}

static void mem_error(err_code)
ushort err_code;
{
}


/*
 * mem_within_page -
 *  check to see if the block of memory can be allocated within single page by
 *  the memory chunk.
 * input:
 *	mem - memory header point to the block of memory
 *	size - the size of memory request
 *      alloc_option - pointer of output buffer.
 * output:
 *	it is only available when return value is true.
 *	MEM_ALLOC_FRONT - allocate from beginning of block
 *	MEM_ALLOC_END -	allocate from end of block.
 * return:
 *	TRUE  - The allocation can be done for the memory block.
 *	FALSE  - The allocation cannot be done for the memory block.
 */
static bool mem_within_page (mem_header *mem, int size, ushort *alloc_option)
{
	char *start, *end;
	
	if (mem->size < size )
		return FALSE;
	if ( size > SRAM_PAGESIZE )
		return FALSE;
	start = (char *)mem + sizeof(mem_header);
	end = start + size;
	/* check if the first block of memory is in the page */
	if (((uint)start & SRAM_PAGEMASK) == ((uint)end & SRAM_PAGEMASK)) {
		*alloc_option = MEM_ALLOC_FRONT;
		return TRUE;
	}
	/* check if the last block of memory is in the page */
	if (mem->size <= (size + sizeof (mem_header))) 
	/* We also need to allocate another header */
		return FALSE;
	end = (char *)mem + sizeof(mem_header) + mem->size;
	start = end - size;
	if (((uint)start & SRAM_PAGEMASK) == ((uint)end & SRAM_PAGEMASK)) {
		*alloc_option = MEM_ALLOC_END;
		return TRUE;
	}
	/* try to look at  page before last page */
	if (mem->size < (size + ((ulong)end & (SRAM_PAGESIZE - 1))))
		return FALSE;
	*alloc_option = MEM_ALLOC_MID;
	return TRUE;
}

/*
 * mem_alloc_from_block -
 *	allocate memory space with the the length of size within the block
 *	pointed by mem.
 * input -
 *	mem: memory header for a free block.
 *	option: allocation options:
		MEM_ALLOC_FULL
		MEM_ALLOC_FRONT
		MEM_ALLOC_END

 *	size: number of bytes to allocate.
 * return:
 *	address: the pointer of allocated space.
 *	NULL:	allocation failed.
 */
static char *mem_alloc_from_block (mem_header *mem, int size,  uchar option )
{
	mem_header *new_block;
	ulong	temp_size;	

	if ( mem->flags & MEM_BUSY )
		return NULL;
	/* check if it is worth to split the block */

	if ((mem->size <= (size + sizeof (mem_header) + 16)) &&
		(option != MEM_ALLOC_END) && (option != MEM_ALLOC_MID)) {
		mem_freelist_remove(mem);
		return ((char *)mem + sizeof(mem_header));
	}
	else {
	 /* we only do block split if there are more than 16 bytes left */
		if ( option == MEM_ALLOC_FRONT) {
			new_block = (mem_header *)((char *)mem + sizeof (mem_header)
				+ size);
			temp_size = mem->size - (size + sizeof(mem_header));
			new_block->size = temp_size;
			mem->size = size;
			mem_insert_new_chunk(mem, new_block);
			mem_freelist_remove(mem);
			return ((char *)mem + sizeof(mem_header));
		}
		if (option == MEM_ALLOC_END) {
			/* allocate space on the end portion */
			new_block = (mem_header *)((char *)mem + mem->size - size);
			mem->size = mem->size - sizeof(mem_header) - size;
			new_block->size = size;
			if (mem_insert_new_chunk(mem, new_block) == FALSE) {
				mem_error( MEM_ERR_INSERT_FAIL);
				return NULL;
			}
			mem_freelist_remove(new_block);
			return ((char *)new_block + sizeof(mem_header));
		}
		if (option == MEM_ALLOC_MID) {
			ulong last_page_size;
			uchar *mem_end, *new_end;
			mem_header *last_page_block;

			mem_end =  MEM_NEXT_CHUNK(mem);
			last_page_size = ((ulong)mem_end & (SRAM_PAGESIZE - 1));
			new_end = mem_end - last_page_size - 1;
			new_block = (mem_header *)(new_end - size - 
						sizeof (mem_header) + 1);
			if ( last_page_size < (sizeof (mem_header) + 16)) {
				new_block->size = size + last_page_size;
				mem->size = mem->size - sizeof(mem_header) -
					new_block->size;
				if (mem_insert_new_chunk(mem, new_block) 
							== FALSE) {
					mem_error( MEM_ERR_INSERT_FAIL);
					return NULL;
				}
				
			}	
			else {
				last_page_block = (mem_header *)(new_end + 1);
				last_page_block->size = last_page_size - 
						sizeof (mem_header);
				new_block->size = size + last_page_size;
				mem->size = mem->size - sizeof(mem_header) -
							size;
				mem->size -=  last_page_size;
				if (mem_insert_new_chunk(mem, new_block) 
							== FALSE) {
					mem_error( MEM_ERR_INSERT_FAIL);
					return NULL;
				}
				new_block->size -= last_page_size;
				if (mem_insert_new_chunk(new_block, 
						last_page_block) == FALSE) {
					mem_error( MEM_ERR_INSERT_FAIL);
					return NULL;
				}
			}
			mem_freelist_remove(new_block);
			return ((char *)new_block + sizeof(mem_header));
		}
	}
	return NULL;
}

/*
 * mem_alloc --
 * 	Generic memory allocation routine. It can handle the options with
 *	page/unpage memory boudary.
 * input:
 *	size - the number of bytes to allocate
 *	flag - boundary option
 * return:
 *	memory address.
 *	zero if no memory available.
 */
static char *mem_alloc ( int size, uchar option)
{
	mem_header *mem;
	struct mem_select {
		mem_header *mem;
		ushort option;
	} best;
	ushort	org_sr, alloc_option;
	char	*mem_addr;

	if ( size <= 0)
		return NULL;
	/* always allocate even bytes */
	if ( size & 1)
		size += 1;

	/* raise interrupt level to 6 */
	org_sr = set_ipl(PRIO_FDL);

        for ( mem = mem_freelist.first, best.mem = NULL; 
		mem != (mem_header *)&mem_freelist; mem = mem->free_next) {
	/* we will try to find the one with the same size first, then
	   the size is bigger than request but is the smallest one */
		if ( mem->size < size )
			continue;
		alloc_option = MEM_ALLOC_FRONT;  /* set initial value */
		if ((option == MEM_NO_BOUNDARY) || 
			mem_within_page(mem, size, &alloc_option)) {
		     if ( size == mem->size ) {
		        best.mem = mem;
			best.option = MEM_ALLOC_FULL;
			break;		
		     }
		     if (( best.mem == NULL )|| ( best.mem->size > mem->size )) {
			best.mem = mem;
			best.option = alloc_option;
		     }
		     
		}	
	}		

	if ( best.mem != NULL )
		mem_addr = mem_alloc_from_block(best.mem, size, best.option);
	else
		mem_addr = NULL;
	reset_ipl(org_sr);
	return mem_addr;
}
/*
 * mem_queue_init -
 *  Initialize memory queue, which is a double link list.
 */
static void mem_queue_init ( mem_queue *q)
{
	/* point to itself */
	q->first = (mem_header *)q;
	q->last = (mem_header *)q;

	/* reset counter */
	q->count = 0; 
}

/*
 * Return the memory chunk back to free list.
 */
static void mem_free ( mem_header *mem)
{
	if (mem->magic != MEM_MAGIC_NUMBER) {
		mem_error(MEM_ERR_CLUBER);
		return;
	}
	if (mem->next != (mem_header *)&mem_chunk &&
	    mem->next->magic != MEM_MAGIC_NUMBER) {
		mem_error(MEM_ERR_CLUBER);
		return;
	}
	if (mem->prev != (mem_header *)&mem_chunk &&
	    mem->prev->magic != MEM_MAGIC_NUMBER) {
		mem_error(MEM_ERR_CLUBER);
		return;
	}

	if ( IS_MEM_BLOCK_FREE(mem->prev) )
	   	/* the previous memory chunk is in the free list;
		   then, we just modify the chunk_list by extending
		   the size of previous chunk and eliminating the
		   current one. */
		mem = mem_merge_prev_block(mem);
	if ( IS_MEM_BLOCK_FREE(mem->next)) 
	  /* the next memory chunk is in the free list */	
		mem_merge_next_block(mem);

	if ( mem->flags & MEM_BUSY) 
		mem_freelist_enq(mem);
}

/*
 * mem_init -
 * memory management init routine. 
 */
void mem_init (void)
{
	mem_header *first_memory_chunk;

	/* init memory queue */
	mem_queue_init(&mem_chunk);
	mem_queue_init(&mem_freelist);

	first_memory_chunk = (mem_header *)DYNAMIC_MEM_AREA;
	/*
	 * The size of first memory chunk is the total memory size take away
	 * the space for code and ipc section and one memory header.
	 */
	first_memory_chunk->size = (NEAT_SRAM_END - DYNAMIC_MEM_AREA) - 
						sizeof(mem_header);
	mem_chunk_create(first_memory_chunk);
	mem_freelist_enq(first_memory_chunk);
}
	
/*
 * malloc -
 *	memory allocation
 * input - size : number of bytes
 * return - memory address.
 *          0 no memory available.
 */

char *malloc ( int size)
{
	if ( size == 0 )
		return NULL;
	return mem_alloc(size, MEM_NO_BOUNDARY);
}
/*
 * malloc_within_4k -
 *   allocate memory without accrossing  4k boundary
 * input - size : number of bytes
 * return - memory address.
 *          0 no memory available.
 */

char *malloc_within_page (int size)
{
	if ( size == 0 )
		return NULL;
	return mem_alloc(size, MEM_WITH_BOUNDARY);
}

void free(char *addr)
{
	ushort org_sr = set_ipl(PRIO_FDL);
	mem_free((mem_header *)((uint)addr - sizeof(mem_header)));
	reset_ipl(org_sr);
}

