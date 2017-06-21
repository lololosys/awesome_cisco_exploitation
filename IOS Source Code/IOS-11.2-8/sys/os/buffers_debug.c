/* $Id: buffers_debug.c,v 3.1.70.5 1996/08/28 13:02:53 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/buffers_debug.c,v $
 *------------------------------------------------------------------
 * buffers_debug.c  This file contains all routines to perform sanity
 * checking on the system's buffers.  Portions removed from 'buffers.c'.
 *
 * October 1995, Francis Bruneault
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: buffers_debug.c,v $
 * Revision 3.1.70.5  1996/08/28  13:02:53  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.70.4  1996/07/25  09:52:16  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.1.70.3  1996/06/29  00:22:31  mshannon
 * CSCdi61596:  crash flag not set on mem corruption, core dump not
 * possible
 * Branch: California_branch
 * Set crash flag once a single corruption is detecte
 *
 * Revision 3.1.70.2  1996/06/11  19:19:42  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Restore ukernel build using 'kernel_validblock_diagnose'
 *
 * Revision 3.1.70.1  1996/06/04  05:15:55  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Added initial debug sanity code for memory corruptions
 *
 * Revision 3.1  1995/11/09  12:43:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/11  23:29:04  francisb
 * Placeholder for Intello_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "pool.h"
#include "packet.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "buffers.h"
#include "buffers_private.h"
#include "buffers_inline.h"
#include "buffers_debug.h"
#include "sched_private.h"
#include "region.h"
#include "free.h"
#include "free_private.h"
#include "config.h"
#include "../ui/debug.h"
#include "../os/os_debug.h"
#include "../if/network.h"


/* Function declarations */
void buffer_sanity_init(void);


/* Structure definitions */
#define MAX_BD_DATA   256
#define MAXBUFFTRAIL  2048
#define MAXBUFFLEAD   256

typedef struct buf_diag {
    long bd_address;
    blocktype bd_buffer;
    uchar bd_data[MAX_BD_DATA];
} buf_diag_t;

typedef struct sanity_check_ {
    bad_code_t	code;
    buf_diag_t	bp;
    buf_diag_t	bp_prev;
    buf_diag_t	bp_next;
    char        buff_leading[MAXBUFFLEAD];
    char        buff_trailing[MAXBUFFTRAIL];
    long        lead_address;
    long        trail_address;
    int         trail_size;
} sanity_check;

/* Variable declarations */

/* This array of messages should be aligned with the enumeration
 * 'bad_code' defined in 'free.h'.  There is one entry for each
 * possible enumeration value.
 */

static char *const bad_msg[] = {
    "",
    "Corrupted redzone ",                      /*BADBLOCK_REDZONE*/
    "Corrupted magic value in in-use block ",  /*BADBLOCK_MAGIC*/
    "Corrupted previous pointer ",             /*BADBLOCK_PREVIOUS*/
    "Size and previous do not correspond to block address ", 
                                               /*BADBLOCK_PREV_SIZE*/
    "Corrupted next pointer ",                 /*BADBLOCK_NEXT*/
    "Block address and size do not correspond to next ", /*BADBLOCK_NEXT_SIZE*/
    "Freespace does not end at end of the pool ", /*BADBLOCK_MEMTOP*/
    "Invalid block in free list ",             /*BADBLOCK_INUSE*/
    "Refcount on freed block is non-zero ",    /*BADBLOCK_REFCOUNT*/
    "Corrupted magic value in freed block ",   /*BADBLOCK_FREEMAGIC*/
    "Block is not long word aligned ",         /*BADBLOCK_NOTALIGNED*/
    "Attempt to free a block that is in use ", /*BADFREE_INUSE*/
    "Refcount on freed block is non-zero ",    /*BADFREE_REFCOUNT*/
    "Previous pointer on first free block is not tree head ", 
                                               /*BADFREE_PREV_PTR*/
    "Head does not correspond to tail in empty free list ", 
                                               /*BADFREE_NEXT_PTR,*/
    "Free block on wrong list ",               /*BADFREE_WRONG_LIST*/
    "Corrupted previous pointer in next of freed block ", /*BADFREE_PREV*/
    "Tail previous pointer on free list is corrupted " /*BADFREE_TAIL*/
};


sanity_check debug_diag;

/*
 * debug_show_buffers_preprocessor_flags
 */

static void debug_show_buffers_preprocessor_flags (void)
{
#ifdef BUFDEBUG
    printf("\nBUFDEBUG is ON");
#endif
#ifdef DEBUG
    printf("\nDEBUG is ON");
#endif
#ifdef BUFRETDEBUG
    printf("\nBUFRETDEBUG is ON");
#endif
}

/*
 * buffer_sanity_init
 *
 * Initialize anu buffer sanity checking/debugging.
 */

void buffer_sanity_init (void) 
{
    kernel_set_validblock_diagnose_hook(validblock_diagnose);
    reg_add_debug_show(debug_show_buffers_preprocessor_flags,
		       "debug_show_buffers_preprocessor_flags");
}

/*
 * buffer_validate
 *
 * Validate pak->pool and call pool specific, buffer validating routine.
 * Must to be interrupt safe.
 *
 */

boolean buffer_validate (paktype *pak)
{
    pooltype *pool = pak->pool;
    boolean result;

    if (!POOLVALID(pool)) {
	errmsg(&msgsym(BADPOOL, SYS), pak, pool);
	return (FALSE);
    }
    result = (*pool->item.validate)(pool, pak);
    return(result);
}

/*
 * pak_pool_item_validate
 *
 * Called to do debug validation on buffers
 */

boolean pak_pool_item_validate (pooltype *pool, void *item)
{
    paktype   *pak = item;
    static boolean crash_flag = FALSE;

    if (!(pool->flags & POOL_SANITY))
	return (TRUE);

    if (crash_flag == TRUE)
	return(TRUE);
    
    if (validmem_complete_interrupt(data_area_to_pakdata(pak->data_area)) &&
	(validmem_complete_interrupt(pak)))
	return(TRUE);

    crash_flag = TRUE;
    return(FALSE);
}

/*
 * log_debug_buffer
 */
static void log_debug_buffer (blocktype *bp, 
			      buf_diag_t *bdp, 
			      mempool *mp,
			      char *identity)
{
    void *dp = NULL;

    buginf("\n\n%s memory block, bp = 0x%x, \nmemory pool type is %s",
	   identity, bp, mp->name);
    bcopy(bp, &bdp->bd_buffer, sizeof(blocktype));
    bdp->bd_address = (long)bp;  /* store the address of the block
       				in case logger not available */

    /*
     * get data snapshot
     */
    dp = &bp->data_area;
    buginf("\ndata check, ptr = 0x%x",dp);
    bcopy(dp, &bdp->bd_data, MAX_BD_DATA);
}

/*
 * validblock_diagnose
 */
char *validblock_diagnose (mempool * mp, 
			   blocktype * bp, 
			   bad_code_t code)
{
    uchar *sp = NULL;
    uchar *rp = NULL;
    blocktype *bp_prev;

    /* make sure we get some info. out */
    buginf("\nvalidblock_diagnose, code = %d", code);
    debug_diag.code = code;
    if (!mempool_find_region_by_addr(mp, bp)) {
       	buginf("\nbp (0x%x) not in mempool(0x%x)", bp, mp);
       	if (!mempool_find_by_addr(bp)) {
       		buginf("\nbp(0x%x) not in any mempool", bp);
       		return bad_msg[code];
       	}
    }

    /*
     * log 'bp'
     * copy a region around 'bp' in here so we can look at it in a dump
     * make sure we get some info. out
     */
    log_debug_buffer(bp, &(debug_diag.bp), mp, "current");
    sp = (char *)bp - MAXBUFFLEAD;
    debug_diag.lead_address = (long)sp;
    if (mempool_find_by_addr(sp))
	bcopy(sp, debug_diag.buff_leading, MAXBUFFLEAD);

    rp = (char *)(bp + 1);
    debug_diag.trail_address = (long)rp;
    debug_diag.trail_size = wtob(bp->size);
    if (debug_diag.trail_size > MAXBUFFTRAIL)
        debug_diag.trail_size = MAXBUFFTRAIL;
    sp = rp + debug_diag.trail_size - 1;
    if (mempool_find_by_addr(rp) && mempool_find_by_addr(sp) )
	bcopy(rp, debug_diag.buff_trailing, debug_diag.trail_size);

    sp = NULL;
    rp = NULL;

    if (mempool_find_by_addr(bp->next))
      log_debug_buffer(bp->next, &(debug_diag.bp_next), mp, "next");
    else
      buginf("\nbp->next(0x%x) not in any mempool", bp->next);
    
    bp_prev =  prev_block(bp, mp);
    if (mempool_find_by_addr(bp_prev))
      log_debug_buffer(bp_prev, &(debug_diag.bp_prev), mp, "previous");
    else
      buginf("\nbp_prev(0x%x) not in any mempool", bp_prev);

    return bad_msg[code];
}
