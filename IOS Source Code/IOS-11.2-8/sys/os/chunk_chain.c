/* $Id: chunk_chain.c,v 3.1.2.1 1996/06/19 23:17:12 smackie Exp $
 * $Source: /release/112/cvs/Xsys/os/chunk_chain.c,v $
 *------------------------------------------------------------------
 * chunk_chain.c - Parser chains and functions for chunk manager
 *
 * June 1996, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: chunk_chain.c,v $
 * Revision 3.1.2.1  1996/06/19  23:17:12  smackie
 * The chunk manager displays it's new look for 1996. (CSCdi60779)
 *
 *   o Soothe the raging masses and implement lockable chunks
 *   o Start hiding chunk structures and defines in a private header
 *   o Add a chain file for "show chunk" and remove entry from chain.c
 *   o Tidy up some redundant sections of code
 *
 * Branch: California_branch
 *
 * Revision 3.1  1996/06/15  01:07:26  smackie
 * Add placeholders for chunk manager spring clean
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../os/free.h"
#include "chunk.h"
#include "chunk_private.h"
    

/*
 * Parser chains for "show chunk"
 */
#define ALTERNATE NONE
#include	"../parser/exec_show_chunk.h"
LINK_POINT(chunk_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Chunk parse chain table
 */
static parser_extension_request chunk_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(chunk_show_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * chunk_ui_init
 *
 * Initialize chunk parser support.
 *
 */

static void chunk_ui_init (subsystype *subsys)
{
    parser_add_command_list(chunk_chain_init_table, "chunk");
}


/*
 * chunk_display_efficiency
 *
 * Works out how much you saved or lost using this scheme
 */

static int chunk_display_efficiency (chunk_type *chunk)
{
    int saved;
    
    /*
     * Work out how much the malloc would have cost for this memory
     */
    saved = ((chunk->maximum - chunk->index) *
	     (chunk->size + MEMORY_BLOCK_OVERHEAD));

    /*
     * How much the chunk manager cost you
     */
    saved -= MEMORY_BLOCK_OVERHEAD + (chunk->end - chunk->data) +
	     ((uchar *)&chunk->freelist[chunk->maximum] - (uchar *)chunk);

    /*
     * If it's an independant chunk, there's one more malloc
     */
    if (chunk_is_independant(chunk))
	saved -= MEMORY_BLOCK_OVERHEAD;

    return(saved);
}


/*
 * chunk_display_flags
 *
 * Print out a couple of tawdry characters for the flags
 */

static void chunk_display_flags (chunk_type *chunk, char *buffer)
{
    int offset = 0;

    if (chunk_is_dynamic(chunk))
	offset += sprintf(buffer + offset, "D");
    else
	offset += sprintf(buffer + offset, " ");

    if (chunk_is_sibling(chunk))
	offset += sprintf(buffer + offset, "S");
    else
	offset += sprintf(buffer + offset, " ");

    if (chunk_is_lockable(chunk))
	offset += sprintf(buffer + offset, "L");
    else
	offset += sprintf(buffer + offset, " ");

    if (chunk_is_independant(chunk))
	offset += sprintf(buffer + offset, "I");
    else
	offset += sprintf(buffer + offset, " ");
}


/*
 * chunk_display_summary
 *
 * Print out a status of current chunks
 */
static void chunk_display_summary (void)
{
    chunk_type *chunk, *sibling, *next;
    uchar       flags[6];
    int         total_saved;
    int         total_free, total_max;

    printf("\n\nFlags   Size(b)   Free  Alloc    HWM  Saved(b)  Name");

    chunk = chunkQ.qhead;

    while (chunk) {
	
	/*
         * Lock this down so it won't be freed under us
	 */
	mem_lock(chunk);

	/*
	 * Get some totals going
	 */
	total_saved = chunk_display_efficiency(chunk);
	total_free  = chunk->index;
	total_max   = chunk->maximum;

	/*
	 * Add in values for any sibling chunks
	 */
	sibling = chunk->next_sibling;
	while (sibling) {

	    /*
	     * Lock down the sibling
	     */
	    mem_lock(sibling);

	    /*
	     * Add up our running totals and step to the next sibling.
	     */
	    total_saved += chunk_display_efficiency(sibling);
	    total_free  += sibling->index;
	    total_max   += sibling->maximum;
	    next = sibling->next_sibling;

	    /*
	     * Unlock the sibling
	     */
	    free(sibling);

	    sibling = next;
	}

	/*
	 * See if we've got any flags
	 */
	chunk_display_flags(chunk, flags);

	/*
	 * Print out stats for this chunk
	 */
	printf("\n%-5s  %-8d  %-5d  %-5d  %-5d  %-8d  %s",
	       flags, chunk->size, total_free, total_max,
	       chunk->inuse_hwm, total_saved, chunk->name);

	next = chunk->next;

	/*
	 * Unlock this chunk
	 */
	free(chunk);

	/*
	 * Skip on to the next one
	 */
	chunk = next;
    }
}

/*
 * chunk_display
 *
 * Print out a status of current chunks
 */

void chunk_display (parseinfo *csb)
{
    chunk_type *chunk, *sibling, *next;
    uchar       flags[6];
    int         total_saved, sibling_saved;
    int         total_free, total_max;

    printf("\nChunk Manager:");
    printf("\n %d chunks created, %d chunks destroyed",
	   chunk_created, chunk_destroyed);
    printf("\n %d siblings created, %d siblings trimmed",
	   chunk_sibling_created, chunk_sibling_trimmed);

    if (GETOBJ(int,1)) {
	chunk_display_summary();
	return;
    }

    printf("\n\n    Chunk  Flags   Size(b)   Free    Max  Saved(b)  Name");

    chunk = (chunk_type *)chunkQ.qhead;

    while (chunk) {

	/*
         * Lock this down so it won't be freed under us
	 */
	mem_lock(chunk);

	/*
	 * Get some totals going
	 */
	total_saved = chunk_display_efficiency(chunk);
	total_free  = chunk->index;
	total_max   = chunk->maximum;

	/*
	 * See if we've got any flags
	 */
	chunk_display_flags(chunk, flags);

	/*
	 * Print out stats for this chunk
	 */
	printf("\n %-8x  %-5s  %-8d  %-5d  %-5d  %-8d  %s",
	       chunk, flags, chunk->size, chunk->index, chunk->maximum,
	       total_saved,
	       chunk->name);

	/*
	 * Print out any sibling chunks
	 */
	sibling = chunk->next_sibling;

	while (sibling) {

	    /*
	     * Lock down the sibling
	     */
	    mem_lock(sibling);

	    sibling_saved = chunk_display_efficiency(sibling);
	    chunk_display_flags(sibling, flags);

	    printf("\n %-8x  %-5s  %-8d  %-5d  %-5d  %-8d  (sibling)",
		   sibling, flags, sibling->size, sibling->index,
		   sibling->maximum, sibling_saved);

	    /*
	     * Add up our running totals and step to the next sibling.
	     */
	    total_saved += sibling_saved;
	    total_free  += sibling->index;
	    total_max   += sibling->maximum;
	    next = sibling->next_sibling;

	    /*
	     * Unlock the sibling
	     */
	    free(sibling);

	    sibling = next;

	    /*
	     * This is the last sibling. Print a total summary
	     */
	    if (!sibling)
		printf("\n    Total                   %-5d  %-5d  %-8d",
		       total_free, total_max, total_saved);
	}

	next = chunk->next;
	
	/*
	 * Unlock the chunk
	 */
	free(chunk);

	/*
	 * Step to the next chunk
	 */
	chunk = next;
    }
}

#ifdef TEST

#define CHUNK_TEST_SIZE  20
#define CHUNK_TEST_MAX   100


void chunk_test (void)
{
    int index;
    chunk_type *test;
    uchar *chunk_free_list[CHUNK_TEST_MAX];
    uchar *chunk_trigger;
    
    test = chunk_create(CHUNK_TEST_SIZE,
			CHUNK_TEST_MAX,
			CHUNK_FLAGS_DYNAMIC,
			NULL, 0,
			"Test");
    
    printf("\nChunk: Allocated test structure at 0x%x", test);
    for (index = 0; index < CHUNK_TEST_MAX; index++)
	chunk_free_list[index] = chunk_malloc(test);
    
    printf("\n Chunk: Sibling Check #1, 0x%x", test->next_sibling);
    chunk_trigger = chunk_malloc(test);
    printf("\n Chunk: Sibling Check #2, 0x%x", test->next_sibling);
    
    if (chunk_free(test, chunk_trigger)) {
	printf("\n Chunk: Sibling Check #3, 0x%x", test->next_sibling);
    } else {
	printf("\n Chunk: Sibling Check #3 Failed!");
	return;
    }
    
    test->flags &= ~CHUNK_FLAGS_DYNAMIC;
    if (chunk_malloc(test)) {
	printf("\n Chunk: Sibling Check #4 Failed!");
	return;
    }
    test->flags |= CHUNK_FLAGS_DYNAMIC;
    
    for (index = 0; index < CHUNK_TEST_MAX - 1; index++)
	chunk_free(test, chunk_free_list[index]);
    
    if (chunk_destroy(test)) {
	printf("\n Chunk: Destroy Check #1 Failed!");
	return;
    }
    
    if (!chunk_free(test, chunk_free_list[CHUNK_TEST_MAX - 1])) {
	printf("\n Chunk: Destroy Check #2 Failed!");
	return;
    }
    
    printf("\n Chunk: chunk 0x%x, index %d, max %d, flags 0x%x",
	   test, test->index, test->maximum, test->flags);
    
    if (chunk_destroy(test))
	printf("\n Chunk: Destroy Check #3 Passed");
    else
	printf("\n Chunk: Destroy Check #3 Failed!");
}

#endif

/*
 * chunk_ui subsystem header
 */
 
#define CHUNK_UI_MAJVERSION    1
#define CHUNK_UI_MINVERSION    0
#define CHUNK_UI_EDITVERSION   1
 
SUBSYS_HEADER(chunk_ui, CHUNK_UI_MAJVERSION, CHUNK_UI_MINVERSION,
              CHUNK_UI_EDITVERSION,
              chunk_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);
