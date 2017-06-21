/* $Id: memory.h,v 1.1 1996/05/12 05:39:08 towu Exp $
 * $Source: /cscka/t1_neat/src/common/memory.h,v $
 *------------------------------------------------------------------
 * memory management header file
 *
 * Feb 1996, Tony Wu
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: memory.h,v $
 * Revision 1.1  1996/05/12 05:39:08  towu
 * File created.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef struct _mem_header {
	ushort	                magic;		/* magic number  0x4444 */
	ushort			flags;		/* status flags */
	ulong			size;		/* memory size */
	struct _mem_header	*next;  	/* next free space */
	struct _mem_header	*prev;		/* prev free space */
	struct _mem_header	*free_prev;	/* free space link list */
	struct _mem_header	*free_next;     /* free space link list */
} mem_header;

#define MEM_MAGIC_NUMBER	0x4444

#define MEM_VALID		0x0000
#define MEM_BUSY		0x0001

typedef struct _mem_queue {
	mem_header	*first;
	mem_header	*last;
	ulong		count;
} mem_queue;

#define IS_MEM_BLOCK_FREE(m) \
	((m) != (mem_header *)&mem_chunk) && (!((m)->flags & MEM_BUSY))
#define MEM_NEXT_CHUNK(mem) \
	((char *)mem+mem->size + sizeof(mem_header)) 

#define MEM_ALLOC_FRONT 	1    /* allocate space from beginning of block */
#define MEM_ALLOC_MID		2    /* allocate space from middle of block */
#define MEM_ALLOC_END   	3    /* allocate space from end of block */
#define MEM_ALLOC_FULL		4    /* Allocate whole chunk */

#define MEM_NO_BOUNDARY		0
#define MEM_WITH_BOUNDARY	1

/* error code */
#define MEM_ERR_INSERT_FAIL	1
#define MEM_ERR_CLUBER		2
