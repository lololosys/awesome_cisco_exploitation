/* $Id: chunk.h,v 3.2.62.2 1996/08/03 05:08:35 smackie Exp $
 * $Source: /release/112/cvs/Xsys/os/chunk.h,v $
 *------------------------------------------------------------------
 * chunk.h - Header file for the fixed memory chunk manager
 *
 * April 1994, Scott Mackie
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: chunk.h,v $
 * Revision 3.2.62.2  1996/08/03  05:08:35  smackie
 * Add support for unzeroed chunks. (CSCdi65048)
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/06/19  23:17:10  smackie
 * The chunk manager displays it's new look for 1996. (CSCdi60779)
 *
 *   o Soothe the raging masses and implement lockable chunks
 *   o Start hiding chunk structures and defines in a private header
 *   o Add a chain file for "show chunk" and remove entry from chain.c
 *   o Tidy up some redundant sections of code
 *
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:45:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/19  18:38:39  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.2  1995/08/03  21:48:30  hampton
 * Chunk manager should record chunk usage high water mark. [CSCdi38207]
 *
 * Revision 2.1  1995/06/07 21:53:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CHUNK_H__
#define __CHUNK_H__

/*
 * Chunk template
 *
 * The chunk looks like this in memory
 *
 * +------------+  <---- chunk
 * |            |
 * | chunk_type |
 * |            |
 * +------------+  <---- chunk->freelist
 * |            |
 * | free list  |
 * |            |
 * +------------+  
 * | |||||||||| | 
 * +------------+  <---- chunk->data (discontiguous if alignment is used)
 * | chunk 0    |
 * Z            Z
 * | chunk n    |
 * +------------+  <---- chunk->end
 *
 *
 * Caveat
 * ------
 *
 * You should go and read the comments in os/chunk.c before using this
 * code for the first time.
 *
 */

/*
 * Chunk Manager defines
 *
 * WARNING!
 *
 * CHUNK_FLAGS_DYNAMIC is only valid when chunk_malloc() and chunk_free()
 * are called from process level. Interrupt level growing of a chunk is
 * *not* supported.
 */

#define CHUNK_FLAGS_NONE        0x00000000
#define CHUNK_FLAGS_DYNAMIC     0x00000001
#define CHUNK_FLAGS_SIBLING     0x00000002
#define CHUNK_FLAGS_INDEPENDANT 0x00000004
#define CHUNK_FLAGS_LOCKABLE    0x00000008
#define CHUNK_FLAGS_UNZEROED    0x00000010

/*
 * Prototypes
 */

extern void        chunk_init(void);
extern chunk_type *chunk_create(ulong, ulong, ulong,
				mempool *mempool, ulong alignment,
				char *name);
extern boolean     chunk_destroy(chunk_type *chunk);
extern void       *chunk_malloc(chunk_type *chunk);
extern boolean     chunk_free(chunk_type *chunk, void *data);
extern boolean     chunk_lock(chunk_type *chunk, void *data);
extern boolean	   chunk_is_destroyable(chunk_type *chunk);
#endif





