/* $Id: free_chain.c,v 3.8.10.9 1996/09/11 17:39:11 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/free_chain.c,v $
 *------------------------------------------------------------------
 * free_chain.c - Parse chains and code for memory pool commands
 *
 * March 1995, Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: free_chain.c,v $
 * Revision 3.8.10.9  1996/09/11  17:39:11  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.8.10.8  1996/08/07  09:01:47  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.8.10.7  1996/06/17  23:35:22  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.8.10.6  1996/05/21  09:58:09  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.10.5  1996/05/09  14:39:47  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.8.10.4.4.1  1996/04/27  07:17:00  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.8.10.4  1996/04/22  19:30:18  widmer
 * CSCdi55275:  spillmem doesnt initialize suspend timer
 * Branch: California_branch
 *
 * Revision 3.8.10.3  1996/04/13  06:05:28  dkatz
 * CSCdi53190:  Init time memory charged to *Dead* process
 * Branch: California_branch
 * Ensure that memory allocated by the ephemeral Init process is charged
 * to *Init*.  Copy the allocator PC from the allocated block into the
 * fragment when fragmenting memory.  Add an option to "show memory" to
 * display the allocating process with each block.
 *
 * Revision 3.8.10.2  1996/04/02  05:37:45  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.8.10.1.8.1  1996/04/08  01:59:48  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.8.10.1  1996/03/18  21:29:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.12.1  1996/03/22  09:41:10  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.8  1996/02/22  14:36:38  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/20  06:15:30  jjohnson
 * CSCdi49356:  Need MIB variable for total DRAM
 * Fully instrument the memory pools
 *
 * Revision 3.6.8.1  1996/03/03  21:24:33  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.6.6.1  1996/03/02  01:20:42  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.6  1996/01/29  07:29:11  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5.18.1  1996/02/29  07:49:55  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.5  1996/01/05  04:56:35  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.4  1995/12/17  18:34:12  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  18:47:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:46  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:45:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6.24.1.2.1  1996/02/03  07:08:11  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.6.24.1  1995/10/21  01:47:54  getchell
 * Branch: Arkansas_branch
 * Ark_VIP_branch commit
 *
 * Revision 2.6.34.1  1996/01/19  21:57:21  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 2.6.16.1  1995/12/06  12:25:22  rlowe
 * Interim fix for bug in spill_free_mem whereby local string buffers
 * were being overrun, thus destroying the stack frame link pointer.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.6  1995/08/12  07:09:39  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.5  1995/08/06  14:23:48  jacobt
 * CSCdi36607:  need show memory summary command
 *
 * Revision 2.4  1995/07/02  10:51:14  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.3  1995/06/19  04:50:47  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/18  06:03:34  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase I--add centralized processing for malloc failures.
 *
 * Revision 2.1  1995/06/07  21:55:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include "ttysrv.h"
#include "subsys.h"
#include "logger.h"
#include "exception.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "sum.h"
#include "../os/chunk.h"
#include "../os/region.h"
#include "free.h"
#include "free_private.h"
#include "sched_private.h"
#include "../util/sorted_array.h"
/*
 * Parse chains for memory exec commands
 */
#define ALTERNATE       NONE
#include	"exec_show_memory.h"
LINK_POINT(memory_show_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for memory config commands
 */
#define ALTERNATE       NONE
#include	"cfg_memory.h"
LINK_POINT(memory_config_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain registration array for memory commands
 */
static parser_extension_request memory_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(memory_show_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(memory_config_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * memory_ui_init
 *
 * Initialize mempool parser support.
 *
 */
static void memory_ui_init (subsystype *subsys)
{
    parser_add_command_list(memory_chain_init_table, "memory");
}


/*
 * memory_free_list_cmd
 *
 * Handler for the "memory free-list nn" command
 *
 * Adds a free list of the specified size to processor memory.
 */
void memory_free_list_cmd (parseinfo *csb)
{
    mempool   *mempool;
    int        ix;
    
    mempool = mempool_find_by_class(MEMPOOL_CLASS_LOCAL);

    /* As usual, NVGENs first. */

    if (csb->nvgen) {
	for (ix = 0; ix < mempool->freelist_count; ix++) {
	    nv_write(mempool->tree_array[ix].configured,
		     "%s %d", csb->nv_command,
		     wtob(mempool->tree_array[ix].minsize) - REDZONEBYTES);
	}
	return;
    }

    /*
     * Handle the command.  The "no" version doesn't take effect until the
     * next reload.
     */
    if (csb->sense) {			/* First the positive version */
	if (!mempool_add_free_list_manual(mempool->class, GETOBJ(int,1)))
	    printf("\n%%Can't add any more free lists");
    } else {				/* Now the negative version */
	free_tree *listptr = freelist_find_by_size(mempool->freeroot,
					     malloc_word_size(GETOBJ(int,1)));
	if (listptr) {
	    listptr->configured = FALSE;
	}
    }
}


/*
 * Validate freeblock without crashing.
 */
static boolean valid_freeblock_gentle (blocktype *curblock, 
				mempool *mem,
				ulong freelist_size)
{
    return(valid_freeblock_inline(curblock, mem, freelist_size, FALSE));
}


/*
 * Header strings for "show memory"
 */

static const char memhead[] = 
" Address  Bytes Prev.    Next     Ref  PrevF   NextF   Alloc PC  What";
static const char memfmt[] = "\n%8x%-7d %8x %8x %-3d%c %7s %7s %8x  %s";
static const char memhead_proc[] = 
" Address  Bytes Prev.    Next     Ref    Alloc Proc    Alloc PC  What";
static const char memfmt_proc[] = "\n%8x%-7d %8x %8x %-3d%c %15s %8x  %s";


/*
 * spillmem
 * Spill out and analyze our memory allocation scheme
 *
 * Returns TRUE if memory is messed up or we just got lost.
 */
boolean spillmem (
    mempool *mem,
    boolean printing,
    boolean only_lost_memory,
    boolean alloc_process,
    ulong *usedbytes,
    ulong *freebytes,
    ulong *largestfree,
    ulong *lowest)
{
    blocktype *funrun, *next_fun;
    register long bytes;
    register boolean inuseflag;
    char next[8], prev[8], flag;
    int tripcount;
    sys_timestamp spillmem_duration;
    boolean got_lost;
    sprocess *sp;
    char *alloc_proc_name;

    if (mem == NULL)
	return (TRUE);

    TIMER_STOP(spillmem_duration);	/* Initialize suspend timer */

    if (printing) {
	printf("\n\n          %s memory\n", mem->name);
 	automore_header(alloc_process ? memhead_proc : memhead);
    }
    funrun = mem->memhead;	/* start of list */
    *lowest = wtob(mem->low_water);
    if (*lowest < REDZONEBYTES) {
	*lowest = 0;
    } else {
	*lowest -= REDZONEBYTES;
    }
    *freebytes = 0;		/* Total free bytes */
    *usedbytes = 0;		/* Total used bytes */
    *largestfree = 0;		/* Largest free block */
    if (mem->corrupt)
	return(TRUE);		/* Don't bother--it's a mess. */
    tripcount = WAY_TOO_MANY_TRIPS;
    while (funrun) {
	if (--tripcount < 0)
	    break;
	if (!validblock_gentle(funrun, mem))
	    break;

	funrun = block_suspend(mem, funrun, &spillmem_duration);
	if (!funrun)
	    break;

	bytes = funrun->size;
	inuseflag = bytes & MEMINUSE ? TRUE : FALSE;
	bytes = (((bytes & FREEDMASK)) << 1) - REDZONEBYTES;
	next_fun = funrun->next;

	if (only_lost_memory) {
	    if (!inuseflag)
		goto next;
	    if ((funrun->allocator_pid == ALLOC_PID_INIT) ||
		(funrun->allocator_pid == ALLOC_PID_SCHED))
		goto next;
	    if (funrun->allocator_pid > MIN_ALLOC_PID) {
		/*
		 * A live PID.  If it matches the scheduler block
		 * address, we don't want to hear about it.
		 */
		sp = process_pid2ptr(funrun->allocator_pid);
		if (!sp || (funrun->allocator_check == (ulong)sp))
		    goto next;
	    }
	}

 	*usedbytes += MEMORY_BLOCK_OVERHEAD;/* always count header as in use */
	if (!inuseflag) {
	    if (!valid_freeblock_gentle(funrun, mem, 0))
		break;
	    if (bytes > *largestfree)
		*largestfree = bytes;
	    *freebytes += bytes;
	} else {
	    *usedbytes += bytes;
	}
	if (printing) {
	    if (automore_quit())
		break;
	    if (inuseflag) {
		if (funrun->refcount < 1) {
		    flag = 'I';		/* In-use, ref count bad */
		} else {
		    flag = ' ';		/* Looks OK */
		}
	    } else {
		freeblocktype *curfree, *nextfree;
		curfree = freeptr(funrun);
		nextfree = curfree->free_next;
		sprintf(next, "%7x", nextfree->free_next == NULL ? 0 :
			freeblock_to_block(nextfree));
		sprintf(prev, "%7x", ((ulong) curfree->free_prev <
				      (ulong) mem->memhead ? 0 :  
				      freeblock_to_block(
					 PREV_FREE_BLOCK(curfree))));
		if (funrun->refcount != 0) {
		    flag = 'F';		/* Free, ref count bad */
		} else {
		    flag = ' ';		/* Looks OK */
		}
	    }
	    if (alloc_process) {
		if (inuseflag) {
		    switch (funrun->allocator_pid) {
		      case ALLOC_PID_INIT:
			alloc_proc_name = "*Init*";
			break;
			
		      case ALLOC_PID_SCHED:
			alloc_proc_name = "*Sched*";
			break;
			
		      default:
			sp = process_pid2ptr(funrun->allocator_pid);
			if (sp && funrun->allocator_check == (ulong) sp) {
			    alloc_proc_name = sp->name;
			} else {
			    alloc_proc_name = "*Dead*";
			}
		    }
		} else {
		    alloc_proc_name = "";
		}
		printf(memfmt_proc, funrun, bytes, prev_block(funrun, mem),
		       funrun->next, funrun->refcount, flag, alloc_proc_name,
		       funrun->allocator_pc, funrun->allocator_name);
	    } else {
		printf(memfmt, funrun, bytes, prev_block(funrun, mem), 
		       funrun->next, funrun->refcount, flag,
		       inuseflag ? "" : prev, inuseflag ? "" : next,
		       funrun->allocator_pc, funrun->allocator_name);
	    }
	}
      next:
	funrun = next_fun;
    }

    if (*lowest > *freebytes)
	*lowest = *freebytes;	/* Don't confuse things. */

    /* If we got lost, or memory is messed up, note it now. */

    got_lost = (funrun != NULL);

    if (printing) {
	if (automore_quit()) {
	    automore_disable();
	    automore_enable(NULL);
	}
    }
    return(got_lost);
}

/*
 * memory_pool_walk
 * Walk the memory pool, executing the function "rtn"
 *
 * Returns TRUE if memory is messed up or we just got lost.
 * Returns FALSE if we walked the pool successfully or
 *            	    or rtn returned  FALSE.
 */

boolean memory_pool_walk (
    mempool *mem,
    memory_pool_walk_rtn_t rtn,
    void *rtn_parameter)
{
    blocktype *funrun;
    int tripcount;
    sys_timestamp memory_pool_walk_duration;
    boolean got_lost;

    if (mem == NULL)
	return (TRUE);

    funrun = mem->memhead;	/* start of list */
    if (mem->corrupt)
	return(TRUE);		/* Don't bother--it's a mess. */
    TIMER_START(memory_pool_walk_duration, CHECKHEAPS_TIME);
    tripcount = WAY_TOO_MANY_TRIPS;
    while (funrun) {
	if (--tripcount < 0)
	    break;
	if (!validblock_gentle(funrun, mem))
	    break;

	/* suspend once in a while on a used block that has valid refcount */
	if (AWAKE(memory_pool_walk_duration) && (funrun->size & MEMINUSE)
	    && (funrun->refcount >= 1)) {
	    blocktype *prev;
	    inline_mem_lock(block_to_data(funrun));
	    process_suspend();
	    
	    if (!validblock_gentle(funrun, mem))
		break;
	    prev = prev_block(funrun, mem);
	    inline_mem_unlock_or_free(block_to_data(funrun));	
	    
	    /* Check if current block has been coalesced. If so then continue 
	     * from the previous block.
	     */
	    if (prev && prev->next != funrun)
		funrun = prev;
	    TIMER_START(memory_pool_walk_duration, CHECKHEAPS_TIME);
	}
	if ((!funrun->size & MEMINUSE) &&
	    (!valid_freeblock_gentle(funrun, mem, 0))) {
	    break;
	}
	if (!(rtn)(mem, funrun, rtn_parameter)) {
		return(FALSE);
	}
	funrun = funrun->next;
    }

    /* If we got lost, or memory is messed up, note it now. */

    got_lost = (funrun != NULL);

    return(got_lost);
}

/*
 * spill_free_mem
 * Spill out and analyze our memory allocation scheme
 */

static long spill_free_mem (mempool *mem)
{
    long freebytes = 0;
    blocktype *funrun;
    freeblocktype *curfree, *nextfree;
    long bytes;
    char next[9], prev[9], flag;
    free_tree *tree;
    int listnum;
    int tripcount;
    char lostflag = ' ';

    if (mem == NULL)
	return(0);

    if (mem->corrupt)
	return(0);

    if (!mem->memhead)
	return(0);

    printf("\n\n          %s memory\n", mem->name);
    automore_header(memhead);

    tripcount = WAY_TOO_MANY_TRIPS;

    /* Walk all of the free lists. */

    for (tree = mem->tree_array, listnum = 1; 
	 listnum <= mem->freelist_count; 
	 tree++, listnum++) {
	if (--tripcount < 0)
	    crashdump(0);	/* Data structures are messed up */
	printf("\n\n%-15d    Free list %d", 
	       wtob(tree->blocksize) - REDZONEBYTES,
	       listnum); 
	curfree = tree->head;

	/* Walk the tree. */

	while (curfree->free_next) {
	    funrun = freeblock_to_block(curfree);
	    if (--tripcount < 0)
		crashdump(0);	/* What a mess. */

	    /* Bail out if we're getting problems. */

	    /* Bogus block pointers? */

	    if ((!validblock_gentle(funrun, mem)) ||

		/* Or bogus freeblock header? */

		(!valid_freeblock_gentle(funrun, mem, tree->blocksize)) ||

		/* Or on wrong free list? */

		((funrun->size < tree->minsize || 
		  funrun->size > tree->maxsize))) {

		/* Note that we're busted and break out of the inner loop. */

		lostflag = '>';
		break;
	    }

	    if (automore_quit())
		goto done;

	    bytes = ((funrun->size & FREEDMASK) << 1) - REDZONEBYTES;
	    freebytes += bytes;
	    nextfree = curfree->free_next;

	    /*
	     * Note that sprintf MAY use 8+1 bytes even though format only
	     * calls out 7+1 bytes.  This happens when MS nibble is non-zero.
	     */
	    sprintf(next, "%7x", nextfree->free_next == NULL ? 0 :
		    freeblock_to_block(nextfree));
	    sprintf(prev, "%7x", (curfree == tree->head ? 0 :  
				  freeblock_to_block(
					     PREV_FREE_BLOCK(curfree))));
	    if (funrun->refcount != 0) {
		flag = 'F';		/* Free, ref count bad */
	    } else {
		flag = ' ';		/* Looks OK */
	    }
	    printf(memfmt,
		   funrun, bytes, 
		   funrun->previous, funrun->next,
		   funrun->refcount,
		   flag,
		   prev,
		   next,
		   funrun->allocator_pc, funrun->allocator_name);
	    curfree = nextfree;
	}
    }
 done:
    printf("\n\nTotal: %c%-8d", lostflag, freebytes);
    if (automore_quit()) {
	automore_disable();
	automore_enable(NULL);
    }
    return (freebytes);
}


/*
 * calculate_count_and_bytes
 *
 * show_memory_by_alloc_pc
 *
 * Implementation of the show memory summary command
 *
 * Doesn't malloc memory because we don't want to fail if malloc fails
 *
 * In order  to show  allocated and free blocks separately,
 *  free blocks are stored with odd allocator_pc's.
 */

#define MAX_ALLOC_PC_ARRAY 15
typedef struct  mem_by_alloc_pc_type_ {
    ulong min_alloc_pc;
    ulong min_size;
    int   array_count;
    ulong alloc_pc[MAX_ALLOC_PC_ARRAY];
    ulong size[MAX_ALLOC_PC_ARRAY];
    const char  *name[MAX_ALLOC_PC_ARRAY];
    int   count[MAX_ALLOC_PC_ARRAY];
} mem_by_alloc_pc_type;

#define BLOCK_PAYLOAD(funrun) \
                      ((((funrun->size & FREEDMASK)) << 1) - REDZONEBYTES)

static boolean
set_alloc_pc_array (mempool *pool, blocktype *funrun, void *par_void)
{
    mem_by_alloc_pc_type *par = par_void;
    ulong allocator_pc = funrun->allocator_pc;
    int j, index;
    boolean ret;

    if ( !(funrun->size & MEMINUSE))  {
	allocator_pc ++;
    }
    if (allocator_pc < par->min_alloc_pc) {
	return(TRUE);
    }
    if (allocator_pc == par->min_alloc_pc &&
	BLOCK_PAYLOAD(funrun) < par->min_size) {
	return(TRUE);
    }
    ret = sorted_array_get_index_2(par->alloc_pc, par->size,
				    par->array_count, &index,
				    allocator_pc,
				    (ulong)BLOCK_PAYLOAD(funrun));
    if (index >= MAX_ALLOC_PC_ARRAY) {
	return(TRUE);
    }
    if (!ret) {
	if (par->array_count == MAX_ALLOC_PC_ARRAY) {
	    par->array_count--;
	}
	for (j = par->array_count; j > index; --j) {
	    par->alloc_pc[j] = par->alloc_pc[j-1];
	    par->size[j] = par->size[j-1];
	    par->count[j] = par->count[j-1];
	    par->name[j] = par->name[j-1];
	}
    	par->count[index] = 0;
	par->array_count++;
	par->alloc_pc[index] = allocator_pc;
	par->size[index] = (ulong)BLOCK_PAYLOAD(funrun);
	par->name[index] = funrun->allocator_name;
    }
    par->count[index]++;
    return(TRUE);
}
    
static const char mem_by_alloc_pc_head[] = 
"\nAlloc PC        Size     Blocks      Bytes    What\n";
static const char mem_by_alloc_pc_fmt[] =
"\n%#9x%-11d%-11d%-11d    %s%s";

void show_memory_by_alloc_pc (void)
{
    mem_by_alloc_pc_type par;
    ulong tmp_alloc_pc;
    ulong tmp_size;
    int i;
    mempool_class class;
    mempool *pool;
    int pool_count, mem_count;
    int pool_bytes, mem_bytes;
    int pool_free_count, mem_free_count;
    int pool_free_bytes, mem_free_bytes;
    int line_bytes;
    int block_free;
    char *free_name = " (Free Blocks)";
    char *line_name;

    mem_bytes = mem_count = 0;
    mem_free_bytes = mem_free_count = 0;
    FOR_ALL_MEMPOOLS(pool, class) {
	if (!pool || (pool->class != class))
	    continue;
	memset(&par, 0, sizeof(par));
	pool_count = pool_bytes = 0;
	pool_free_count = pool_free_bytes = 0;
	printf("\n\n          %s memory", pool->name);
	automore_header(mem_by_alloc_pc_head);
	while (TRUE) {
	    if (memory_pool_walk(pool, &set_alloc_pc_array, &par)) {
		printf("\n Corrupted pool");
		break;
	    }
	    if (par.array_count == 0) {
		if (pool_count) {
		    printf(mem_by_alloc_pc_fmt, 0, 0, pool_count, pool_bytes,
			   "Pool Summary", "");
		    mem_count += pool_count;
		    mem_bytes += pool_bytes;
		}
		if (pool_free_count) {
		    printf(mem_by_alloc_pc_fmt, 0, 0,
			   pool_free_count, pool_free_bytes,
			   "Pool Summary", free_name);
		    mem_free_count += pool_free_count;
		    mem_free_bytes += pool_free_bytes;
		}
		if (pool_count + pool_free_count) {
		    printf(mem_by_alloc_pc_fmt, 0, MEMORY_BLOCK_OVERHEAD,
			   pool_count + pool_free_count, 
			   (pool_count + pool_free_count) *
			   MEMORY_BLOCK_OVERHEAD,
			   "Pool Summary", "(All Block Headers)");
		}
		break;
	    }
	    for (i = 0; i < par.array_count; i++) {
		block_free = par.alloc_pc[i] & 1;
		line_bytes = par.size[i] * par.count[i];
		if (!block_free) {
		    line_name = "";
		    pool_count += par.count[i];
		    pool_bytes += line_bytes;
		} else {
		    line_name =  free_name;
		    pool_free_count += par.count[i];
		    pool_free_bytes += line_bytes;
		}
		printf (mem_by_alloc_pc_fmt,
			par.alloc_pc[i] -  block_free, par.size[i],
			par.count[i], line_bytes, par.name[i], line_name);
	    }
	    tmp_alloc_pc = par.alloc_pc[par.array_count - 1];
	    tmp_size = par.size[par.array_count - 1] + 1;
	    memset(&par, 0, sizeof(par));
	    par.min_alloc_pc = tmp_alloc_pc;
	    par.min_size = tmp_size;
	    if (automore_quit())
		break;
	    process_suspend();
	}
    }
    if (mem_count) {
	printf("\n");
	printf(mem_by_alloc_pc_fmt, 0, 0, mem_count, mem_bytes,
	       "Memory Summary");
    }
    if (mem_free_count) {
	printf("\n");
	printf(mem_by_alloc_pc_fmt, 0, 0, mem_free_count, mem_free_bytes,
	       "Memory Summary", free_name);
    }
}

/*
 * show_memory_pool
 * Print out memory utilization statistics for a single memory pool
 */

static const char poolhdr[]="\n               Head   Total(b)    Used(b)    Free(b)  Lowest(b) Largest(b)";
/*          *poolfoo="\nProcessor  12345678  12345678  1234567890  1234567890  1234567890  1234567890"; */
static const char poolfmt[]="\n%-9s  %-8x  %c%-8d  %c%-8d  %c%-8d   %-8d  %c%-8d";
static const char poolbad[]="\n%-9s  %-8x  %c%-8d  %c%-8d  %c%-8d   %-8d  Corrupted";

static void show_memory_pool (mempool *pool)
{
    ulong usedbytes, freebytes, largestfree, lowest;
    boolean got_lost;
    uchar lostflag = ' ';

    if (!pool->memhead)
	return;

    got_lost = spillmem(pool, FALSE, FALSE, FALSE, &usedbytes, &freebytes,
			&largestfree, &lowest);
    if (got_lost)
	lostflag = '>';
    if (pool->corrupt) {
	printf(poolbad, pool->name, pool->memhead,
	       lostflag, (usedbytes+freebytes), lostflag, usedbytes,
	       lostflag, freebytes, lowest);
    } else {
	printf(poolfmt, pool->name, pool->memhead,
	       lostflag, (usedbytes+freebytes), lostflag, usedbytes, 
	       lostflag, freebytes, lowest, lostflag, largestfree);
    }
}

/*
 * memory_dump
 *
 * Dump a section of memory via a specified output function.
 */

boolean memory_dump (void          *start,
		     void          *end,
		     boolean        suspend,
		     memory_dump_data_type_t type,
		     memory_dump_output_t    output)
{
    uchar    c;
    char     hex[81];
    char     data[81];
    uchar   *addr, *hexptr, *dataptr;
    int      lines, i, value;
    boolean  valid = TRUE;

    /*
     * Always start displaying on a 16 byte boundary
     */
    addr  = (uchar *)(((int)start) & ~0xF);
    lines = 0;
    
    while (valid) {
	/*
	 * If we're suspending, check for a user-entered "quit" or
	 * printing more than 100 lines in a row.
	 */
	if (suspend) {
	    if (automore_quit()) {
		return(FALSE);
	    }
	    if (++lines > 100) {
		lines = 0;
		process_sleep_for(ONESEC/10);
		if (modemcheck(stdio)) {
		    return(FALSE);
		}
	    }
	}

        hexptr = hex;
        dataptr = data;
	hexptr += sprintf(hexptr, "\n%-08x:", addr);

        for (i = 0; i < 16; i++) {
	    if (!(i & 3))
		hexptr += sprintf(hexptr, " ");

	    valid = ((addr >= (uchar *)start) &&
		     (addr <= (uchar *)end));
            if (valid && touch_device(addr)) {
		value  = *addr++;
		hexptr += sprintf(hexptr, "%-02x", value);
		
		switch (type) {
		  case MEMORY_DUMP_DATA_TYPE_ASCII:
		    c = value & 0x7f;
		    if ((c < ' ') || (c == 0x7f))
			c = '.';
		    dataptr += sprintf(dataptr, "%c", c);
		    break;

		  case MEMORY_DUMP_DATA_TYPE_EBCDIC:
		    c = value;
		    sys_ebcdic_to_ascii(&c, 1);
		    dataptr += sprintf(dataptr, "%c", c);
		    break;

		  default:
		    break;
		}
            } else {
		addr++;
		if (!valid) {
		    hexptr  += sprintf(hexptr, "  ");
		    dataptr += sprintf(dataptr, " ");
		} else {
		    hexptr  += sprintf(hexptr, "XX");
		    dataptr += sprintf(dataptr, "X");
		}
            }
	    
	}

	(*output)("%s  %s", hex, data);
    }

    /*
     * If we're suspending, give others a chance
     */
    if (suspend) {
	process_sleep_for(ONESEC/10);
	if (modemcheck(stdio)) {
	    return(FALSE);
	}
    }

    return(TRUE);
}

/*
 * show_memory
 * Print out memory utilization statistics
 *
 * show memory { <start_address> [end_address] |
 *                  {[ processor | io | sram | multibus | pci ] [free]} }
 * 
 * OBJ(int,1) = <start_address>, -1 if no address
 *
 * OBJ(int,2) = <end_address>,   -1 if no address
 *
 * OBJ(int,3) = MEMPOOL_CLASS_xxx (memory class)
 *
 * OBJ(int,4) =	TRUE = The 'free' keyword was parsed.
 *		FALSE = No 'free' keyword.
 *
 * OBJ(int,5) = TRUE = The 'dead' keyword was parsed.
 *		FALSE = No 'dead' keyword.
 *
 * OBJ(int,6) = TRUE = The 'summary' keyword was parsed.
 *              False = No 'summary' keyword was parsed.
 *
 * OBJ(int,7) = TRUE = The 'allocating-process' keyword was parsed.
 *		FALSE = No 'allocating-process' keyword.
 */
void show_memory (parseinfo *csb)
{
    ulong dummy;
    uint baseaddr, endaddr;
    mempool_class class;
    mempool *pool;

    automore_enable(NULL);    

    /*
     * If argument specified, dump memory in hex starting at that location.
     * We must be an enabled process to perform this function.
     */
    baseaddr = GETOBJ(int,1);
    if (baseaddr != -1) {
	endaddr = GETOBJ(int,2);
	if (endaddr == -1) {
	    endaddr = MAXUINT;
	}
	/*
	 * Parser only lets us here if enabled
	 */
	memory_dump((void*)baseaddr, (void *)endaddr, TRUE,
		    MEMORY_DUMP_DATA_TYPE_ASCII,
		    (memory_dump_output_t)printf);
	automore_disable();
	return;
    }

    /*
     * If no argument, first summarize memory use.
     */
    if (GETOBJ(int,3) == MEMPOOL_CLASS_UNKNOWN) {
	printf(poolhdr);
	FOR_ALL_MEMPOOLS(pool, class) {
	    if (!pool || (pool->class != class))
		continue;
	    show_memory_pool(pool);
	}
	printf("\n\n");
	automore_conditional(0);
	printf("\n");
	if (automore_quit()) {
	    automore_disable();
	    return;
	}
    }

    if (GETOBJ(int,6)) {
	show_memory_by_alloc_pc();
	automore_disable();
	return;
    }
    /*
     * Print out tedious memory list tracings
     */
    if (GETOBJ(int,5)) {      /* 'lost' */
	ulong used = 0, cum_used= 0, count = 0;
	/*
	 * If we want all pools
	 */
	if (GETOBJ(int,3) == MEMPOOL_CLASS_UNKNOWN) {
	    FOR_ALL_MEMPOOLS(pool, class) {
		if (!pool || (pool->class != class))
		    continue;
		used = 0;
		spillmem(pool, TRUE, TRUE, FALSE, &used, &dummy, &dummy,
			 &dummy);
		if (used != 0) {
		    printf("\n         ------\n        %-7d", used);
		    cum_used += used;
		    count++;
		}
	    }
	    if (count > 1)
		printf("\n         ======\n        %-7d", cum_used);
	} else {                /* Specific pool */
	    spillmem(mempools[GETOBJ(int,3)], TRUE, TRUE, FALSE, &used,
		     &dummy, &dummy, &dummy);
	    printf("\n        -------\n        %-7d", used);
	}
    } else if (!GETOBJ(int,4)) {      /* Not 'free' */
	/*
	 * If we want all pools
	 */
        if (GETOBJ(int,3) == MEMPOOL_CLASS_UNKNOWN) {
            FOR_ALL_MEMPOOLS(pool, class) {
		if (!pool || (pool->class != class))
		    continue;
		spillmem(pool, TRUE, FALSE, GETOBJ(int,7), &dummy, &dummy,
			 &dummy, &dummy);
            }
        } else {                /* Specific pool */
	    spillmem(mempools[GETOBJ(int,3)], TRUE, FALSE, GETOBJ(int,7),
		     &dummy, &dummy, &dummy, &dummy);
        }
    } else {			/* 'free' */
	if (GETOBJ(int,3) == MEMPOOL_CLASS_UNKNOWN) {
	    FOR_ALL_MEMPOOLS(pool, class) {
		if (!pool || (pool->class != class))
		    continue;
		spill_free_mem(pool);
	    }
	} else {		/* Specific pool */
	    spill_free_mem(mempools[GETOBJ(int,3)]);
	}
    }
    automore_disable();
}

/*
 * [no] memory check-interval [nn]
 *
 * OBJ(int,1) = interval in seconds
 * "no" turns it off, lack of parameter sets the default value.
 */
void memory_check_cmd (parseinfo *csb)
{
    /* Start with NVGENs, as usual. */

    if (csb->nvgen) {

	/* The following optimizes. */

	if (HEAPCHECK_IVL_DEFAULT != 0) {
	    if (heapcheck_ivl == 0) {
		nv_write(TRUE, "no %s", csb->nv_command);
	    } else {
		nv_write(heapcheck_ivl != HEAPCHECK_IVL_DEFAULT, "%s %d",
			 csb->nv_command, heapcheck_ivl / ONESEC);
	    }
	} else {
	    nv_write(heapcheck_ivl != HEAPCHECK_IVL_DEFAULT, "%s %d",
		     csb->nv_command, heapcheck_ivl / ONESEC);
	}
	return;
    }

    /* Do it. */

    if (csb->set_to_default)
	heapcheck_ivl = HEAPCHECK_IVL_DEFAULT;
    else if (csb->sense) {
	heapcheck_ivl = GETOBJ(int,1) * ONESEC;
	if (heapcheck_ivl == 0)
	    heapcheck_ivl = HEAPCHECK_IVL_DEFAULT;
    } else {
	heapcheck_ivl = 0;
    }
    mempool_set_check_timer(&heapcheck_timer, heapcheck_ivl);
}

/*
 * [no] memory validate-checksum [nn]
 *
 * OBJ(int,1) = interval in seconds
 * "no" turns it off, lack of parameter sets the default value.
 */
void memory_val_cmd (parseinfo *csb)
{
    /* Start with NVGENs, as usual. */

    if (csb->nvgen) {

	/* The following optimizes. */

	if (CKSUMCHECK_IVL_DEFAULT != 0) {
	    if (cksumcheck_ivl == 0) {
		nv_write(TRUE, "no %s", csb->nv_command);
	    } else {
		nv_write(cksumcheck_ivl != CKSUMCHECK_IVL_DEFAULT, "%s %d",
			 csb->nv_command, cksumcheck_ivl / ONESEC);
	    }
	} else {
	    nv_write(cksumcheck_ivl != CKSUMCHECK_IVL_DEFAULT, "%s %d",
		     csb->nv_command, cksumcheck_ivl / ONESEC);
	}
	return;
    }

    /* Do it. */

    if (csb->set_to_default) {
	cksumcheck_ivl = CKSUMCHECK_IVL_DEFAULT;
    } else if (csb->sense) {
	cksumcheck_ivl = GETOBJ(int,1) * ONESEC;
	if (cksumcheck_ivl == 0)
	    cksumcheck_ivl = CKSUMCHECK_IVL_DEFAULT;
    } else {
	cksumcheck_ivl = 0;
    }
    mempool_set_check_timer(&cksumcheck_timer, cksumcheck_ivl);
}

/*
 * show memory failures alloc
 */
void show_memory_allocfail (parseinfo *csb)
{
    ulong entry_number;
    malloc_failures *failptr;

    printf("\nCaller       Pool          Size   Alignment   When");

    /* Go oldest to newest. */

    entry_number = malloc_failure_trace_number;
    while (TRUE) {
	failptr = &malloc_failure_trace[entry_number];
	if (failptr->calling_pc) {	/* If active */
	    printf("\n%#10x  %11s %-8d%-7d      %TE", failptr->calling_pc,
		   failptr->pool->name, failptr->size, failptr->alignment,
		   failptr->failtime);
	}
	entry_number++;
	if (entry_number >= MALLOC_FAILURE_RECORD_COUNT)
	    entry_number = 0;
	if (entry_number == malloc_failure_trace_number) /* Done */
	    break;
    }
}

#ifdef THRASH_MEMORY
/*
 * Memory thrash task
 *
 * This process will gobble lots of memory and free it back in
 * churning, semi-random ways to beat the tar out of the memory
 * allocator.
 */
typedef struct thrashblock_ {
    struct thrashblock_ *next;		/* Next pointer */
    sys_timestamp expire;		/* Expiration time */
} thrashblock;
static thrashblock *thrashhead;		/* Head of chain */

static boolean task_running;		/* Task is running */

static int target_usage;		/* Amount to try to use up */
static int max_blocksize;		/* Biggest block allowed */
static int alloc_persec;		/* Amount to allocate per second */
static int persistence;			/* Max time to live of block */
static int total_mem;			/* How much we've got thusfar */
static int run_time;			/* Max time to do this
					   nonsense, or 0 if not running */
static sys_timestamp runtimer;		/* Time to stop */

/* Cheap hack to return a random number <= max */

static inline ulong rand (ulong max, void *seed)
{
    ulong current;
    GET_TIMESTAMP32(current);
    return (((current / 4) + (ulong) seed) % (max + 1));
}

static process memory_thrash_process (void)
{
    thrashblock *curthrash, **prevthrash;
    int mem_this_run;
    mempool *mempool;

    SAVE_CALLER();

    mempool = mempool_find_by_class(MEMPOOL_CLASS_LOCAL);

    for (;;) {				/* Forever */
	process_sleep_for(ONESEC);	/* Busy busy busy */

	/* First run down the queue to see if anything's expired. */

	curthrash = thrashhead;
	prevthrash = &thrashhead;
	while (curthrash) {
	    /*
	     * Got entry.  If its timer has expired, or the user has
	     * entered "no mem thrash", free it.
	     */
	    if (AWAKE(curthrash->expire) || (!run_time)) { /* Time's up */
		*prevthrash = curthrash->next; /* Delink */
		total_mem -= get_block_size(curthrash, NULL);
		free(curthrash);	/* Back to square one */
		curthrash = *prevthrash; /* Move to next */
	    } else {			/* Still valid */
		prevthrash = &curthrash->next;
		curthrash = curthrash->next;
	    }
	}

	/*
	 * Now allocate new blocks until we can't get any more, or we've
	 * hit our target for this run.
	 */

	if (SLEEPING(runtimer)) {	/* If we're allowed to do it */
	    mem_this_run = min(target_usage - total_mem, alloc_persec);
	    while (mem_this_run > 0) {

		/* Allocate half of 'em as aligned, for effect. */

		if (rand(1, curthrash)) {
		    curthrash = malloc_block(mempool, rand(max_blocksize,
							 curthrash),
					     caller(), NULL, 64);
		} else {
		    curthrash = malloc(rand(max_blocksize, curthrash));
		}
		if (!curthrash)		/* No memory! */
		    break;
		mem_this_run -= get_block_size(curthrash, NULL);
		total_mem += get_block_size(curthrash, NULL);
		TIMER_START(curthrash->expire, rand(persistence, curthrash));
		curthrash->next = thrashhead;
		thrashhead = curthrash;
	    }
	}
    }					/* And go around again... */
}

/*
 * thrash_command
 *
 * OBJ(int,1) = target memory usage in bytes
 * OBJ(int,2) = max block size in bytes
 * OBJ(int,3) = max persistence of a block in seconds
 * OBJ(int,4) = max amount to allocate per second, in bytes
 * OBJ(int,5) = run time of this test, in seconds
 */
void thrash_command (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write(run_time, "%s %d %d %d %d %d", csb->nv_command,
		 target_usage, max_blocksize, persistence / ONESEC,
		 alloc_persec, run_time / ONESEC);
	return;
    }

    if (csb->sense) {
	if (!task_running) {
	    process_create(memory_thrash_process, "Thrash memory",
			   NORMAL_STACK, PRIO_LOW);
	}
	target_usage = GETOBJ(int,1);
	max_blocksize = GETOBJ(int,2);
	persistence = GETOBJ(int,3) * ONESEC;
	alloc_persec = GETOBJ(int,4);
	run_time = GETOBJ(int,5) * ONESEC;
	TIMER_START(runtimer, run_time);
	task_running = TRUE;
    } else {
	run_time = 0;
	GET_TIMESTAMP(runtimer);	/* Make it stop */
	target_usage = 0;		/* Make it all get freed eventually */
    }
}
#endif


/*
 * memory_ui subsystem header
 */
 
#define MEMORY_UI_MAJVERSION    1
#define MEMORY_UI_MINVERSION    0
#define MEMORY_UI_EDITVERSION   1
 
SUBSYS_HEADER(memory_ui, MEMORY_UI_MAJVERSION, MEMORY_UI_MINVERSION,
              MEMORY_UI_EDITVERSION,
              memory_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);

