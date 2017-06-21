/* $Id: sse_ssp.c,v 3.2.60.3 1996/08/28 12:46:12 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/sse_ssp.c,v $
 *------------------------------------------------------------------
 * sse_spp.c - Support for the SSE on the SPP board
 *
 * May 1993 Tony Li
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sse_ssp.c,v $
 * Revision 3.2.60.3  1996/08/28  12:46:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.2  1996/04/05  01:02:24  hampton
 * One of the files included by these file may not exist when dependencies
 * are built.  To prevent these files from being re-compiled with every
 * make, use a full path name in the include statement.  [CSCdi53732]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:44:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:22:52  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/01/24  21:48:28  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:21:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/28  09:25:11  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/09  13:04:58  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:45:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "../hes/msg_sse.c"		/* Not a typo, see logger.h */
#include "../parser/parser_defs_sse.h"
#include "../util/range.h"
#include "if_mci.h"
#include "sse.h"
#include "sse_private.h"
#include "sse_registry.h"
#include "subsys.h"
#include "../hes/ssesym.h"

static boolean ssp_memory_tested;

/*
 * sse_ssp_reset
 * Reset the SSE data structure.  Process level work.
 */

static void sse_ssp_reset (mcitype *cbus)
{
    ssetype *sse;
    uint treememsize, rwmemsize;
    boolean treemem, rwmem;
    uint origtree, origrw;
    uint *addr;
    uchar *caddr;
    shadowwordtype sw;
    long *memptr;

    sse = cbus->sseptr;

    treemem = touch_device(SSE_TREE);
    rwmem = touch_device(SSE_REWRITE);
    cbus->sse_present = treemem && rwmem;

    if (sse_debug) 
	bugtrace();

    if (!cbus->sse_present) {
	if (sse) {
	    sse_sw_reset(sse, 0, 0);
	    free(cbus->sseptr);
	    cbus->sseptr = NULL;
	}
	return;
    }
    
    sse_hw_off(cbus);
    /*
     * This shouldn't be necessary, but just in case.
     */
    if (sse) {
	sse->state |= SSE_STATE_HWRESET|SSE_STATE_SWRESET; 
	sse->state &= ~SSE_STATE_INITIALIZED;
    }

    /*
     * Size memory.  
     */
    treememsize = 0;
    caddr = (uchar *) SSE_TREE;
    while ((caddr <= (uchar *) SSE_TREE_END) && touch_device(caddr)) {
	caddr += SSE_STEPSIZE;
	treememsize += SSE_STEPSIZE;
	process_may_suspend();
    }
    rwmemsize = 0;
    caddr = SSE_REWRITE;
    while ((caddr < SSE_REWRITE_END) && touch_device(caddr)) {
	caddr += SSE_STEPSIZE;
	rwmemsize += SSE_STEPSIZE;
	process_may_suspend();
    }
    origtree = treememsize;
    origrw = rwmemsize;

    /*
     * Now actually hang the hardware.  We need to do this before we walk
     * thru memory otherwise, the engine might start when we rewrite the
     * instruction it's looking at.  This can cause really bad things like
     * DONE instructions.
     */
    sse_peek_treemem(sse, SSE_GLOBAL_RESET, &sw);
    sw.lt.inst = SSE_INST_HANG;
    sw.eq.inst = SSE_INST_HANG;
    sw.gt.inst = SSE_INST_HANG;
    sse_poke_treemem(sse, SSE_GLOBAL_RESET, &sw);
    memptr = (long *)SSE_INITREG;
    *memptr = (SSE_GLOBAL_RESET << 16);

    if (!ssp_memory_tested) {
	ssp_memory_tested = TRUE;

	/*
	 * Test tree memory
	 */
	for (addr = (uint *)SSE_TREE;
	     addr < (uint *)(SSE_TREE + treememsize); addr++) {
	    
	    /*
	     * Don't touch the fourth longword, it's not really there.
	     */
	    if (((int)addr & 0xf) == 0xc)
		continue;
	    *addr = 0;
	    process_may_suspend();
	}
	
	for (addr = (uint *)SSE_TREE;
	     addr < (uint *)(SSE_TREE + treememsize); addr++) {
	    
	    /*
	     * Don't touch the fourth longword, it's not really there.
	     */
	    if (((int)addr & 0xf) == 0xc)
		continue;
	    if (*addr != 0) {
		errmsg(&msgsym(MEMERROR, SSE), addr, 0, *addr);
		treememsize = (int)(addr - 1) - (int) SSE_TREE; 
		break;
	    }
	    *addr = 0xffffffff;
	    process_may_suspend();
	}
	
	for (addr = (uint *)SSE_TREE;
	     addr < (uint *)(SSE_TREE + treememsize); addr++) {
	    
	    /*
	     * Don't touch the fourth longword, it's not really there.
	     */
	    if (((int)addr & 0xf) == 0xc)
		continue;
	    if (*addr != 0xffffffff) {
		errmsg(&msgsym(MEMERROR, SSE), addr, 0xffffffff, *addr);
		treememsize = (int)(addr - 1) - (int) SSE_TREE;
		break;
	    }
	    process_may_suspend();
	}
	
	/*
	 * Test rewrite memory.
	 */
	for (addr = (uint *)SSE_REWRITE;
	     addr < (uint *)(SSE_REWRITE + rwmemsize); addr++) {
	    *addr = 0;
	    process_may_suspend();
	}
	
	for (addr =  (uint *)SSE_REWRITE;
	     addr < (uint *)(SSE_REWRITE + rwmemsize); addr++) {
	    if (*addr != 0) {
		errmsg(&msgsym(MEMERROR, SSE), addr, 0, *addr);
		rwmemsize = (int)(addr - 1) - (int) SSE_REWRITE;
		break;
	    }
	    *addr = 0xffffffff;
	    process_may_suspend();
	}
	
	for (addr = (uint *) SSE_REWRITE;
	     addr < (uint *)(SSE_REWRITE + rwmemsize); addr++) {
	    if (*addr != 0xffffffff) {
		errmsg(&msgsym(MEMERROR, SSE), addr, 0xffffffff, *addr);
		rwmemsize = (int)(addr - 1) - (int) SSE_REWRITE;
		break;
	    }
	    process_may_suspend();
	}
    }

    if ((treememsize != origtree) || (rwmemsize != origrw)) 
	errmsg(&msgsym(BADMEMORY, SSE), origtree, origrw,
	       treememsize, rwmemsize);
    
    if ((treememsize == 0) || (rwmemsize == 0)){
	if (sse_debug)
	    buginf("\nSSE: no SSE memory on unit %d", cbus->unit);
	return;
    }
    
    treememsize >>= 4;		/* convert to tree words */

    sse_finish_reset(cbus, treememsize, rwmemsize, TRUE);
}

/*
 * show_sse_memory
 * Dump the sse memory.
 */

void show_sse_memory (ssetype *sse, int addr)
{
    shadowwordtype word;

    automore_enable(NULL);
    if (addr == -1) 	 
	addr = 0; 	
    if (addr > sse->words_avail)
	return; 	
    if (addr < 0) 	 
	return; 	
    for (; addr < sse->words_avail; addr++) {
	sse_peek_treemem(sse, addr, &word);
	printf("\n%04x: %04x%02x%02x %04x%02x%02x %04x%02x%02x", addr,
	       word.lt.ptr, word.lt.value, word.lt.inst,
	       word.eq.ptr, word.eq.value, word.eq.inst,
	       word.gt.ptr, word.gt.value, word.gt.inst);   	 
	if (automore_quit())
	    return; 	
    }
    automore_disable();
}

/*
 * show_sse_rewrite
 * Dump the sse rewrite memory.
 */

void show_sse_rewrite (ssetype *sse, int addr)
{
    uint longword, i;

    automore_enable(NULL);
    if (addr == -1) 	 
	addr = 0; 	
    if (addr & 1)
	addr--;
    if (addr < 0) 	 
	return; 	
    while (addr < sse->rwbytes_avail) {
	printf("\n%05x: ", addr);
	for (i = 0; (i < 4) && (addr < sse->rwbytes_avail); i++) {
	    longword = sse_peek_rwmem(sse, addr) << 16;
	    addr += 2;
	    if (addr < sse->rwbytes_avail)
		longword |= sse_peek_rwmem(sse, addr);
	    addr += 2;
	    printf(" %08x", longword);
	} 
	if (automore_quit())
	    return; 	
    }
    automore_disable();
}

/*
 * ssp_idb_capable
 *
 * Given a hwidb, figure out if we can SSE switch on it.
 */

static boolean ssp_idb_capable (hwidbtype *hwidb, boolean fast_ok)
{
    mcitype *sp;

    if (!(hwidb->status & IDB_CBUS))
	return(FALSE);
    if (!fast_ok)
	return(FALSE);
    sp = hwidb->devctl;
    if (!sp)
	return(FALSE);
    if (sp->sse_present && !is_smds(hwidb) && !is_ddr(hwidb))
	return(TRUE);
    return(FALSE);
}

/*
 * ssp_present
 *
 * Return TRUE if there's an SSP present.
 */

static boolean ssp_present (void)
{
    boolean treemem, rwmem;
    
    treemem = touch_device(SSE_TREE);
    rwmem = touch_device(SSE_REWRITE);
    return(treemem && rwmem);
}

/*
 * ssp_idb_bridging_capable
 *
 * Figure out if we can SSE bridge on this interface.  Return TRUE if we
 * can. 
 */

static boolean ssp_idb_bridging_capable (hwidbtype *hwidb)
{
    mcitype *cbus;

    if (!(hwidb->status & IDB_CBUS))
	return(FALSE);
    if (!(hwidb->status & (IDB_ETHER | IDB_FDDI)))
	return(FALSE);
    if ((hwidb->status & IDB_FDDI) && (hwidb->type != IDBTYPE_FDDIT))
	return(FALSE);
    cbus = hwidb->devctl;
    if (!cbus)
	return(FALSE);
    if (!cbus->sse_present)
	return(FALSE);
    return(TRUE);
}

/*
 * ssp_subsys_init
 *
 * Initialize the SSP subsystem.
 */

static void ssp_subsys_init (subsystype *subsys)
{
    reg_add_sse_idb_capable(ssp_idb_capable,
			    "ssp_idb_capable");
    reg_add_sse_present(ssp_present, "ssp_present");
    reg_add_sse_idb_bridging_capable(ssp_idb_bridging_capable,
				     "ssp_idb_bridging_capable");
    reg_add_sse_board_reset(sse_ssp_reset, "sse_ssp_reset");
    reg_add_sse_bus_reset(cbus_init, "cbus_init");

    ssp_memory_tested = FALSE;
}

/* 
 * SSE/SSP subsystem header
 */

#define SSP_MAJVERSION 1
#define SSP_MINVERSION 0
#define SSP_EDITVERSION 2

SUBSYS_HEADER(ssp, SSP_MAJVERSION, SSP_MINVERSION, SSP_EDITVERSION,
	      ssp_subsys_init, SUBSYS_CLASS_DRIVER,
	      "seq: sse",
	      "req: sse");
