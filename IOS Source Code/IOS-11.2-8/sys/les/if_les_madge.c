/* $Id: if_les_madge.c,v 3.3.62.7 1996/08/28 12:58:39 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_madge.c,v $
 *------------------------------------------------------------------
 * if_les_madge.c - low level code to deal with madge firware and tms380
 *
 * May 1993       - Dit Morse
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This code pulled from if_les_tms380.c (1.33), and if_px_tms380.c (1.34)
 * (one could say, hacked once again)
 * See that log for history earlier than recorded here.
 *------------------------------------------------------------------
 * $Log: if_les_madge.c,v $
 * Revision 3.3.62.7  1996/08/28  12:58:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.6  1996/08/26  15:10:20  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.3.62.5  1996/08/22  05:15:45  echeng
 * CSCdi62388:  Many bugs in the code handling TokenRing error counters
 * Branch: California_branch
 *
 * Revision 3.3.62.4  1996/08/13  02:20:27  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.3.62.3  1996/07/09  06:00:32  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.3.62.2  1996/05/17  11:26:32  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.48.2  1996/04/26  15:22:06  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.48.1  1996/04/03  19:59:28  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.62.1  1996/03/18  20:42:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:05:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/01/24  22:24:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  17:40:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:48  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:14:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/07/05  20:49:31  hampton
 * Consolidate common token ring variables into the idb, instead of having
 * them in the private data structure for each type of token ring
 * interface.  Remove overloading of the delay variable by token rings.
 * Eliminate direct references to the system clock.  [CSCdi36721]
 *
 * Revision 2.6  1995/06/26 22:49:56  yoko
 * CSCdi36061:  Low-end token ring doesnt check malloc return code
 *
 * Revision 2.5  1995/06/23  16:39:46  fred
 * CSCdi36295:  Commit Queue MIB to 11.0
 *
 * Revision 2.4  1995/06/21  08:55:33  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.3  1995/06/19  06:47:50  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:11:30  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:36:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * if_px_tms380.c  DosXX/IGS/MERLOT Network Tokenring interface driver
 *
 *      April 1992, Dit Morse
 *      Hacked again, from if_xx_tms380.c
 *
 *	October 1991, Marciano Pitargue
 *	Hacked from if_pan_tms380.c
 *	All rights reserved.
 *
 *	Extensive use made of code from David Hampton's SBE16 card monitor
 *	code in /csc/sbe16/madge, and of his system code in
 *	/csc/sys/if/if_sbe16.[ch] (Chris Shaker)
 *
 *	August 5, 1991 - Updated to agree with /csc/sbe16/madge version 1008,
 *	as of July 24, 1991.
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "logger.h"
#include "../if/msg_tr.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "../if/tring.h"
#include "../if/tring_private.h"
#include "../if/tring_common.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "if_les.h"
#include "if_les_tms380.h"
#include "../if/network.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"


/************************************************************************
 *
 * init380: do chip set mandated initialization
 *
 * returns TRUE if its cool.  FALSE otherwise and fail reason returned
 * back in where fail_p points.
 * 
 ************************************************************************/

boolean init380 (hwidbtype *idb,
		 ulong *fail_p)
{
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    rx_slot_ptr *rxptr;
    tx_slot_ptr *txptr;
    ushort intval, *ptr;
    short i, time;
    leveltype level;
    pooltype *tms380_buffer_pool = NULL;
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    
    /*
     * Initialize the first 22 bytes of the TI defined inititalization block.
     * See page 4-42 of the 1990 version of the TMS380 User's Guide:
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = INITBLK_ADDR;
    
    DBG2(DBG_MADGE, 
	 "TR(%d): INIT_OPTIONS = 0x%04x\n", idb->unit, INIT_OPT_VAL);
    
    sif_regs->datainc = INIT_OPT_VAL;   /* INIT_OPTIONS                     */
    sif_regs->datainc = INIT_VEC;       /* CMD_STATUS_VECTOR/TRANSMIT_VECT  */
    sif_regs->datainc = INIT_VEC;       /* RECEIVE_VECTOR/RING_STATUS_VECT  */
    sif_regs->datainc = INIT_VEC;       /* SCB_CLEAR_VECTOR/ADAPTER_CK_VECT */
    if (cookie->processor == PROCESSOR_BASS || 
	cookie->processor == PROCESSOR_CANCUN) {
	sif_regs->datainc = INIT_RX_BURST_BASS;  /* RECEIVE_BURST_SIZE      */
	sif_regs->datainc = INIT_TX_BURST_BASS;  /* TRANSMIT_BURST_SIZE     */
    }
    else {
	sif_regs->datainc = INIT_RX_BURST;  /* RECEIVE_BURST_SIZE           */
	sif_regs->datainc = INIT_TX_BURST;  /* TRANSMIT_BURST_SIZE          */
    }
    sif_regs->datainc = INIT_DMA_THRES; /* DMA_ABORT_THRESHOLD              */
    ptr =(ushort *) &ds->scb380->scb_cmd;     /* In IO memory               */
    sif_regs->datainc = (ulong) ptr >> 16;    /* SCB_ADDRESS (High)         */
    sif_regs->datainc = (ulong) ptr & 0xffff; /* SCB_ADDRESS (Low)          */
    ptr =(ushort *) &ds->scb380->ssb_cmd;     /* In IO memory               */
    sif_regs->datainc = (ulong) ptr >> 16;    /* SSB_ADDRESS (High)         */
    sif_regs->datainc = (ulong) ptr & 0xffff; /* SSB_ADDRESS (Low)          */
    
    /*
     * Poke the Madge Generic init block through the DIO window.
     * See 'struct SMTMAC_LOAD_PARMS', in the section, "BRING UP AND
     * INITIALIZATION", in "THE MADGE SMART SRB INTERFACE", page 9.
     */
    sif_regs->datainc = SMT_LEN_MAC;		  /* Length of load parms    */
    sif_regs->datainc = SMT_MOD_SIGNATURE_MAC;	  /* Generic MAC Signature   */
    sif_regs->datainc = INIT_UNUSED;		  /* Reserved                */
    sif_regs->datainc = VERNUM_SMTMAC_LOAD_PARMS; /* Version # for table     */
    
    sif_regs->datainc = INIT_UNUSED;		  /* debug_active            */
    sif_regs->datainc = *((ushort *)&idb->bia[0]); /* permanent_address      */
    sif_regs->datainc = *((ushort *)&idb->bia[2]);
    sif_regs->datainc = *((ushort *)&idb->bia[4]);
    sif_regs->datainc = INIT_UNUSED;              /* rxtx_buffer_size        */
    sif_regs->datainc = INIT_UNUSED;              /* dma_buffer_addr         */
    sif_regs->datainc = INIT_UNUSED;
    sif_regs->datainc = INIT_UNUSED;              /* dma_buffer_size         */
    sif_regs->datainc = INIT_UNUSED;              /* max_buffer_ram          */
    sif_regs->datainc = INIT_MIN_BUFFER_RAM;      /* min_buffer_ram          */

    /*
     * Set DMA burst size to 8 in case of BASS/CANCUN
     * board. 
     */
    if (cookie->processor == PROCESSOR_BASS || 
	cookie->processor == PROCESSOR_CANCUN) 
	sif_regs->datainc = INIT_TX_BURST_BASS;  /* sif_burst_size          */
    else 
	sif_regs->datainc = INIT_RX_TX_BURST;	 /* sif_burst_size          */
    
    /*
     * Poke the Madge FASTMAC Specific init block through the DIO window.
     * See "Initialization" in the Madge FASTMAC V3.0 Interface Specification.
     */
    sif_regs->datainc = SMT_LEN_FASTMAC;
    sif_regs->datainc = SMT_MOD_SIGNATURE_FASTMAC;
    sif_regs->datainc = INIT_UNUSED;
    sif_regs->datainc = VERNUM_FASTMAC_MODULE;
    
    sif_regs->datainc = FF_USED;		  /* Feature flags           */
    sif_regs->datainc = INT_USED;		  /* Interrupt flags         */
    sif_regs->datainc = OPEN_OPT_VAL;		  /* Open options            */
    sif_regs->datainc = *((ushort *)&ds->node_addr[0]); /* Node address      */
    sif_regs->datainc = *((ushort *)&ds->node_addr[2]);
    sif_regs->datainc = *((ushort *)&ds->node_addr[4]);
    sif_regs->datainc = *((ushort *)&zeromac[2]); /* Group address           */
    sif_regs->datainc = *((ushort *)&zeromac[4]);
    sif_regs->datainc = *((ushort *)&zeromac[2]); /* Functional address      */
    sif_regs->datainc = *((ushort *)&zeromac[4]);
    sif_regs->datainc = INIT_UNUSED;              /* rx_buf_physaddr         */
    sif_regs->datainc = INIT_UNUSED;              /* rx_buf_physaddr         */
    sif_regs->datainc = INIT_UNUSED;              /* rx_buf_size             */
    sif_regs->datainc = INIT_UNUSED;              /* rx_buf_space            */
    sif_regs->datainc = INIT_UNUSED;              /* tx_buf_physaddr         */
    sif_regs->datainc = INIT_UNUSED;              /* tx_buf_physaddr         */
    sif_regs->datainc = INIT_UNUSED;              /* tx_buf_size             */
    sif_regs->datainc = INIT_UNUSED;              /* tx_buf_space            */
    sif_regs->datainc = idb->max_buffer_size;     /* max frame size          */
    sif_regs->datainc = INIT_UNUSED;              /* size rxdesc queue       */
    sif_regs->datainc = INIT_UNUSED;              /* max rx dma              */
    sif_regs->datainc = INIT_UNUSED;              /* group root address      */
    sif_regs->datainc = TMS380_RX_BUFS;           /* rx bufs in eagle        */
    sif_regs->datainc = TMS380_TX_BUFS;           /* large tx bufs in eagle  */
    sif_regs->datainc = TMS380_MAX_RX_SLOTS;      /* rx slots in eagle       */
    sif_regs->datainc = TMS380_MAX_RING;          /* tx bufs in eagle        */
    sif_regs->datainc = INIT_UNUSED;               /* tx ahead */
    reset_interrupt_level(level);
    
    /*
     * Kick the processor. Note that we will get back TI interrupt status
     * codes here, as defined in "SIFCMD and SIFSTS Register Read-Bit
     * Assignments", on page 4-34.
     */
    sif_regs->intreg = TRINT_ADAPTER | TRINT_EXECUTE | TRINT_SYSINT;
    level = set_interrupt_level(ALL_ENABLE);
    process_sleep_for(135);		/* Initial delay */
    reset_interrupt_level(level);
    
    DBG3(DBG_MADGE,
	 "TR(%d): init380: woke up after initial delay: intval=%#x, actl=%#x\n",
	 idb->unit, sif_regs->intreg, sif_regs->actl);
    
    for (time = 0; time < TMS380_INIT_TIMEOUT; time++) {
	intval = sif_regs->intreg;
	
	DBG3(DBG_MADGE,
	     "TR(%d): init380: woke up, intval=%#x, actl=%#x\n",
	     idb->unit, intval, sif_regs->actl);
	
	if (!(intval & (TRINT_INIT | TRINT_TEST | TRINT_ERROR)))
	    break;
	if (intval & TRINT_ERROR) {
	    *fail_p = (ulong) intval;
	    
	    DBG2(DBG_MADGE, 
		 "TR(%d): init380: Error! *fail_p=%#x\n",
		 idb->unit, *fail_p);
	    
	    return(FALSE);
	}
	level = set_interrupt_level(ALL_ENABLE);
	process_sleep_for(500);		/* Wait 0.5 sec */
	reset_interrupt_level(level);
    }
    if (time > TMS380_INIT_TIMEOUT) {
	DBG2(DBG_MADGE,
	     "TR(%d): init380: selftest timed out, time=%d\n",
	     idb->unit, time);
    } else {
	DBG2(DBG_MADGE,
	     "TR(%d): init380: got a result, time=%d\n",
	     idb->unit, time);
    }
    
    /*
     * It's baaaaack.  What did it say.
     */
    if (intval & (TRINT_INIT | TRINT_TEST | TRINT_ERROR)) {
	/*
	 * Failure....  One of the bits INIT, TEST, or ERROR
	 * are set... They shouldn't be.
	 */
	*fail_p = (ulong) intval;
	
	DBG2(DBG_MADGE,
	     "TR(%d): init380: Error2! *fail_p=%#x\n",
	     idb->unit, *fail_p);
	return(FALSE);
    }
    
    /*
     * Seems to have worked.  Check for proper magic values in
     * the SCB and SSB.  This tells us that the TI code is O.K.
     */
    ptr = (ushort *)ds->scb380;
    if (ptr[0] != SCBINIT0 && ptr[1] != SCBINIT1 && ptr[2] != SCBINIT2 &&
	ptr[4] != SSBINIT0 && ptr[5] != SSBINIT1 &&
	ptr[6] != SSBINIT2 && ptr[7] != SSBINIT3) {
	*fail_p = (ulong) 0x157A;
	
	DBG2(DBG_MADGE,
	     "TR(%d): init380: Bad SCB and SSB, *fail_p=%#x\n",
	     idb->unit, *fail_p);
	return(FALSE);
    }
    
    /*
     * Is the Madge code O.K?  Check it now.
     */
    if ((intval & MADGE_SYSINT) && (intval & MADGE_CHECK)) {
	tms380_adapter_check(idb, intval, 4);
	return(FALSE);
    }
    
    /*
     * Read TI adapter addresses of interest.  These address can only be
     * read in the window between initialization and opening of the ring.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs->adrx        = ADDR_CHPT1;
    sif_regs->adr         = TI_INTERNAL_ADDR;
    ds->bia_addr          = sif_regs->datainc;
    ds->swlev_addr        = sif_regs->datainc;
    ds->address_addr      = sif_regs->datainc;
    ds->parm_addr         = sif_regs->datainc;
    ds->mac_buff          = sif_regs->datainc;
    ds->ti_ring_speed_ptr = sif_regs->datainc; /* Skip over LLC counter ptr */
    ds->ti_ring_speed_ptr = sif_regs->datainc;
    ds->adapter_ram_ptr   = sif_regs->datainc;
    
    /*
     * Now read the total amount of memory:
     */
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr = ds->adapter_ram_ptr;
    ds->adapter_ram = sif_regs->data;
    
    DBG2(DBG_MADGE,
	 "TR(%d): total RAM = %d Kbytes\n",
	 idb->unit, ds->adapter_ram);
    
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr = ds->ti_ring_speed_ptr;
    ds->ti_ring_speed = sif_regs->data;
    
    DBG2(DBG_MADGE,
	 "TR(%d): ti_ring_speed = %d\n",
	 idb->unit, ds->ti_ring_speed);
    
    /*
     * Read Madge adapter addresses of interest.  These address can only be
     * read in the window between initialization and opening of the ring.
     */
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = MADGE_INTERNAL_ADDR;
    ds->ssb_ptr  = sif_regs->datainc;
    ds->srb_ptr  = sif_regs->datainc;
    ds->arb_ptr  = sif_regs->datainc;
    ds->stb_ptr  = sif_regs->datainc;
    ds->ipb_ptr  = sif_regs->datainc;

    /*
     * set up the buffers for the rx slots
     *
     * get 1st slot
     */
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->stb_ptr + OFFSET_RX_SLOT_PTR;
 
    /*
     * poll every 1/2 second to see if madge is done initializing
     * itself wrt internal buffers.  wait 3 seconds max.
     */
    for (i = 0; i < TMS380_INIT_TIMEOUT; i++) {
        if (sif_regs->data != 0)
            break;
        process_sleep_for(ONESEC / 2);
    }

    ds->rx_size = make_ring_size(TMS380_MAX_RX_SLOTS);

    /*
     * Recreate the pool upon an MTU change.
     */
    if (idb->pool->size != idb->max_buffer_size) {

        /*
         * release the old pool here
         */
        pool_destroy(idb->pool);

        tms380_buffer_pool = pak_pool_create(idb->hw_namestring,
						 idb->pool_group,
						 idb->max_buffer_size,
						 POOL_SANITY,
						 NULL);
        if (!tms380_buffer_pool) {
	    crashdump(0);
        }
 
        idb->pool = tms380_buffer_pool;

        idb->pool->fallback = pak_pool_find_by_size(idb->max_buffer_size);

        pool_adjust(idb->pool, 0, TMS380_NUM_BUF(ds->rx_size), 
		    TMS380_NUM_BUF(ds->rx_size), TRUE);
        pak_pool_create_cache(idb->pool, TMS380_NUM_CACHE(ds->rx_size));
        pool_adjust_cache(idb->pool, TMS380_NUM_CACHE(ds->rx_size));
    }
 
    /* 
     * if there wasn't an mtu change, just recreate the cache, since  
     * it was destroyed on shutdown
     */

    if (!(idb->pool->flags & POOL_CACHE)) {
        pak_pool_create_cache(idb->pool, TMS380_NUM_CACHE(ds->rx_size));
	if (!(idb->pool->userflags & POOL_USER_PERM))
	    pool_adjust(idb->pool, 0, TMS380_NUM_BUF(ds->rx_size), 
			TMS380_NUM_BUF(ds->rx_size), TRUE);
	pool_adjust_cache(idb->pool, TMS380_NUM_CACHE(ds->rx_size));
    }

    ds->rx_p = malloc_fast(sizeof(rx_slot_ptr)*TMS380_MAX_RX_SLOTS);
    if (!ds->rx_p)
        return(FALSE);

    ds->tms380_rx_slot = 0;

    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->stb_ptr + OFFSET_RX_SLOT_PTR;
 
    for (i = 0; i < ds->rx_size; i++) {
 
        rxptr = &ds->rx_p[i];
        rxptr->fm_rx_space = sif_regs->data;

        sif_regs->adrx = ADDR_CHPT1;
        sif_regs->adr  = rxptr->fm_rx_space + FM_RX_NEXT_SLOT_OFFSET;
 
    }
    /*
     * associate a buffer w/ each rx slot
     */
    for (i = 0; i < ds->rx_size; i++) {
        paktype *pak;
        rxptr = &ds->rx_p[ds->tms380_rx_slot];
 
        if (!rxptr->pak) {
            /*
             * get a buffer to assign to this slot
             */
            pak = tms380_getbuf(idb, FALSE);
            if (pak == NULL) {
                errmsg(&msgsym(NOMEMORY, TR), idb->unit, "token ring rx ring");
                net_cstate(idb, IDBS_ADMINDOWN);
                reset_interrupt_level(level);
                return(FALSE);
            }
            rxptr->pak = pak;
 
        }
 
        pak = rxptr->pak;
 
        sif_regs->adrx    = ADDR_CHPT1;
        sif_regs->adr     = rxptr->fm_rx_space + FM_RX_MSW_BUFFER_ADDR;
        sif_regs->datainc = ((ulong)pak->datagramstart >> 16);    /* MSW */
        sif_regs->datainc = ((ulong)pak->datagramstart & 0xFFFF); /* LSW */
        sif_regs->adr  = rxptr->fm_rx_space;
        sif_regs->data = 0;
	ds->tms380_rx_slot++;
	if (ds->tms380_rx_slot == ds->rx_size)
		ds->tms380_rx_slot = 0;

    }
    /*
     * get address in dio space for the 1st tx slot
     */
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->stb_ptr + OFFSET_TX_SLOT_PTR;
 
    /*
     * poll every 1/2 second to see if madge is done initializing
     * itself wrt internal buffers.  wait 3 seconds max.
     */
    for (i = 0; i < TMS380_INIT_TIMEOUT; i++) {
        if (sif_regs->data != 0)
            break;
        process_sleep_for(ONESEC / 2);
    }
 
    ds->tx_size = make_ring_size(TMS380_MAX_RING);
    idb->mci_txcount = ds->tx_size;
    ds->tx_p = malloc_fast(sizeof(tx_slot_ptr)*TMS380_MAX_RING);
    if (!ds->tx_p)
        return(FALSE);

    ds->tms380_tx_slot = 0; 
    ds->tms380_tx_free_slot = 0; 

    for (i = 0; i < ds->tx_size; i++) {
 
        txptr = &ds->tx_p[i];
        /*
         * point to the madge's internal memory space
         */
        txptr->fm_tx_space = sif_regs->data;
 
        /*
         * get next slot
         */
 
        sif_regs->adrx = ADDR_CHPT1;
        sif_regs->adr  = txptr->fm_tx_space + FM_TX_NEXT_SLOT_OFFSET;
 
        /*
         * zero out tx ring (which holds the packet assigned
         * to this slot) just for yuks
         */
        txptr->pak = NULL;
    }
 
    /*
     * flag ready to receive by writing a zero to "tms380_rx_slot_start"
     * in the status block
     */
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->stb_ptr + OFFSET_RX_SLOT_PTR;
    sif_regs->data = 0;
    reset_interrupt_level(level);
    
    /*
     * Free the ARB Block.  It is initially owned by the system.
     */
    WRITE_SIF(&sif_regs->intreg, SIGNAL_ARB_FREE);
    
    /*
     * Grab the MAC software version string:
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr = ds->swlev_addr;
    ptr = (ushort *)&ds->frozen_stats.ti380_version;
    for (i = 0; i < MAC_VERS_LEN/2; i++)
	*ptr++ = sif_regs->datainc;
    reset_interrupt_level(level);
    
    /*
     * Bye.
     */
    *fail_p = 0;
    
    DBG2(DBG_MADGE,
	 "TR(%d): init380: success, intval=%#x\n",
	 idb->unit, intval);
    
    return(TRUE);
}


/************************************************************************
 *
 * tms380_panic_to_mom: make sure a panic message gets to mom
 *
 * This routine makes sure that debug info gets to mom.
 *
 * This routine is intended to be called prior to calling certain
 * death.  It's usefullness is in debugging infield roms when
 * something dies.
 *
 * p1 is a ulong that generally includes two fields.  The first field
 * (16 bits) denotes the major failure being indicated.  The 2nd field
 * (16 bits) denotes the sub-indicator of where the failure occured.
 * The other parameters are available for passing additional information
 * about why this failure occurred.  This is 3 longs.  Additional
 * calls to panic_to_mom can be made if additional information needs to
 * passed.
 * 
 ************************************************************************/

static void tms380_panic_to_mom (hwidbtype *idb,
				 int fail,
				 int fail_place,
				 ulong d0,
				 ulong d1)
{
    tms380_instance_t *ds;
    tokensb *tsb;
    
    ds = INSTANCE;
    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
    ds->if_state = IF_DOWN;
    ds->sca_fail = fail;
    ds->sca_fail_code = fail_place;
    ds->debug0 = d0;
    ds->debug1 = d1;
    ds->debug2 = ds->debug3 = ds->debug4 = 0;
    if ((fail != tsb->tr_last_panic) ||
	!SLEEPING(tsb->tr_last_panic_time)) {
	errmsg(&msgsym(PANICTYPE, TR), idb->unit,
	       tr_panic_types[fail]);
	TIMER_START(tsb->tr_last_panic_time, ONESEC);
	tsb->tr_last_panic = fail;
    }
    errmsg(&msgsym(PANIC, TR), idb->unit,
	   ((ulong) fail << 16) | fail_place, ds->debug0, ds->debug1);

    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
}


/**********************************************************************
 *
 * tms380_panic_info: make sure a panic info message gets out
 *
 * This routine makes sure some panic information gets out.  It is
 * used in liu of panic_to_mom and certain_death and does not cause
 * tramon to fall over and die.
 *
 * p1 is a ulong that generally includes two fields.  The first field
 * (16 bits) denotes the major failure being indicated.  The 2nd field
 * (16 bits) denotes the sub-indicator of where the failure occured.
 * The other parameters are available for passing additional information
 * about why this failure occurred.  p2 and all of the debug cells in the
 * sca can be used for this.  If additional information needs to be
 * passed this can be done by additional calls to panic_info.
 *
 ***********************************************************************/

static void tms380_panic_info (hwidbtype *idb,
			       int fail,
			       int fail_place,
			       ulong d0,
			       ulong d1,
			       ulong d2,
			       ulong d3,
			       ulong d4)
{
    tms380_instance_t *ds;
    tokensb *tsb;
    
    ds = INSTANCE;
    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
    
    ds->debug0 = d0;
    ds->debug1 = d1;
    ds->debug2 = d2;
    ds->debug3 = d3;
    ds->debug4 = d4;
    if ((fail != tsb->tr_last_panic) ||
	!SLEEPING(tsb->tr_last_panic_time)) {
	errmsg(&msgsym(PANICTYPE, TR), idb->unit, tr_panic_types[fail]);
	TIMER_START(tsb->tr_last_panic_time, ONESEC);
	tsb->tr_last_panic = fail;
    }
    errmsg(&msgsym(PANICINF, TR),
	   idb->unit, ((ulong) fail << 16) | fail_place, d0, d1, d2, d3, d4);

    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
}


/*
 * tms380_adapter_check
 *
 * The TMS380 processor software has detected an unrecoverable error.
 */

void tms380_adapter_check (hwidbtype *idb,
			   int intval,
			   int where)
{
    tms380_instance_t *ds;
    struct _sifreg_t *sif_regs;
    ushort a, b, c, d;
    leveltype level;

    SAVE_CALLER();
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    
    /*
     * Read Adapter Check information:
     */
    level = raise_interrupt_level(NETS_DISABLE);
    ds->intval = intval;
    ds->where_code = where;
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = CHECK_PTR;
    a = ds->adapter_check_status = sif_regs->datainc;
    b = ds->parm0 = sif_regs->datainc;
    c = ds->parm1 = sif_regs->datainc;
    d = ds->parm2 = sif_regs->datainc;
    reset_interrupt_level(level);
    WRITE_SIF(&sif_regs->intreg, CLEAR_INTERRUPT);
    
    ds->adapter_checks++;
    
    if (tokenevent_debug) {
        buginf("TR(%d): TMS380 adapter check caller = %x\n", 
	   idb->unit, caller());
    
        buginf("where=%d, intval=%#x, status=%#x, parm0=%#x, "
	       "parm1=%#x, parm2=%#x\n", 
	       where, intval, a, b, c, d);
    }
   
    tms380_pnt_ptrs (idb);  /* fetch and print ptrs */

    tms380_panic_info(idb, SCA_FAIL_380CHECK, 0, a, b, c, d, 0);
    tms380_certain_death(idb, SCA_FAIL_380CHECK, (ulong)intval);
}

/*
 * tms380_size_RAM
 * Size TMS380 RAM via DIO writes/reads, returning the RAM size in words.
 */

static int tms380_size_RAM (hwidbtype *idb)
{
    tms380_instance_t *ds;
    struct _sifreg_t *sif_regs;
    int size;
    ushort para, tmp;
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    
    /*
     * Find out how many paragraphs of memory we have:
     */
    for (para = 0; para < MAX_PARA; para++) {
	
	/*
	 * Write the paragraph number into TEST_LOC in each paragraph:
	 */
	sif_regs->adrx = para;
	sif_regs->adr = TEST_LOC;
	sif_regs->data = para;
	
	/*
	 * Check for wrap around onto paragraph 0:
	 */
	sif_regs->adrx = 0;
	sif_regs->adr = TEST_LOC;
	tmp = sif_regs->data;
	if (tmp != (ushort)0) {
	    sif_regs->data = 0;	/* Fix paragraph 0 back up */
	    break;
	}
    }
    
    
    /*
     * See if the paragraph numbers are still there:
     */
    for (para = 0; para < MAX_PARA; para++) {	
	
	/*
	 * Verify that the paragraph number is still in TEST_LOC
	 * in each paragraph:
	 */
	sif_regs->adrx = para;
	sif_regs->adr = TEST_LOC;
	tmp = sif_regs->data;
	
	if (tmp != para) {
	    break;
	}
    }
    ds->paragraphs = para;
    size = ds->paragraphs * BYTES_PER_PARA;
    
    /*
     * Do more extensive memory tests now:
     */
    
    return(size);
}

#define TMS380_REGISTERS	0x0100
#define TMS380_DATA_AREA	0x02fe

static void tms380_write_all_RAM (hwidbtype *idb)
{
    tms380_instance_t *ds;
    struct _sifreg_t *sif_regs;
    int para, word;
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    
    for (para = 0; para < ds->paragraphs; para++) {
	sif_regs->adrx = para;	/* Write paragraph register */
	if (para == 1) {
	    sif_regs->adr = 0;
	    for (word = 0; word < TMS380_REGISTERS; word += 2) {
		sif_regs->datainc = 0;
	    }
	    sif_regs->adr = TMS380_DATA_AREA;
	    for (word = TMS380_DATA_AREA; word < BYTES_PER_PARA; word += 2) {
		sif_regs->datainc = 0;
	    }
	} else {
	    sif_regs->adr = 0;
	    for (word = 0; word < BYTES_PER_PARA; word += 2) {
		sif_regs->datainc = 0;
	    }
	}
    }
}


/*
 * tms380_test_DIO_RAM
 * Size and test the DIO RAM. Use before downloading the TI and Madge
 * firmware. Return the size, in shorts, of the DIO RAM.
 */

static int tms380_test_DIO_RAM (hwidbtype *idb)
{
    tms380_instance_t *ds;
    int size;
    
    ds = INSTANCE;
    size = tms380_size_RAM(idb);
    
    tms380_write_all_RAM(idb);
    
    return(size);
}


/************************************************************************
 *
 * hardreset380: restart the TI 380.
 *
 * This routine will cause the TI chip set to perform a hard reset by
 * yanking on it's reset line.  It then downloads the TI software into
 * the adapter.  Last, it waits for proper completion of the diagnositics
 * that are built into the chip set.
 *
 * It will return TRUE if everything goes well.  FALSE otherwise
 * along with the failure reasons returned in fail_p.
 * 
 ************************************************************************/

boolean hardreset380 (hwidbtype *idb,
		      ulong *fail_p)
{
    tms380_instance_t *ds;
    struct _sifreg_t *sif_regs;
    ucodehdr *eagle;
    madge_fw         *fw;
    madge_fw_32data  *fw_data;
    madge_fw_32block *fw_block;
    ushort count, offset, intval, *sptr, fill;
    short time;
    leveltype level;
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    /*
     * Reset the TI processor.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    tms380_reset(idb);		/* Reset line (see page 4-38 User Guide) */
 
    /*
     * 2500 2R2T platform uses sif reg NSEL bits to set ring speed.
     * Download time is the ONLY time speed can be set via sif actl.
     * These bits are NoConnect on other platforms.
     */
    if (idb->tr_ring_speed == 4) {
        sif_regs->actl = (TRACTL_DOWNLOAD|RING_SPEED_4);
    } else {
        sif_regs->actl = (TRACTL_DOWNLOAD|RING_SPEED_16);
    }
    DELAY(1);			/* Wait at least 14 usec */
    
    ds->memory_size = tms380_test_DIO_RAM(idb);
    
    /*
     * Is the microcode present in our load image?
     */
    eagle = &timagic;
    if (eagle->magic != MADGE_UCODE_MAGIC) {
	errmsg(&msgsym(NOFIRM, TR), idb->unit, eagle, eagle->magic);
	reset_interrupt_level(level);
	return(FALSE);
    }
    
    /*
     * Download the microcode:
     */
    fw = (madge_fw *)eagle->data;
    for ( ; fw->length != 0; fw = (madge_fw *)((uchar *)fw + fw->length)) {
	switch (fw->type) {
	  case MADGE_DL_HEADER:
	    break;
	    
	  case MADGE_DL_32DATA:
	    fw_data = (madge_fw_32data *)fw;
	    sif_regs->adrx = fw_data->address_h;
	    sif_regs->adr  = fw_data->address_l;
	    for (count = fw_data->count, sptr = fw_data->data; count; count--)
		sif_regs->datainc = *sptr++;
	    break;
	    
	  case MADGE_DL_32BLOCK:
	    fw_block = (madge_fw_32block *)fw;
	    sif_regs->adrx = fw_block->address_h;
	    sif_regs->adr  = fw_block->address_l;
	    fill = fw_block->fill;
	    for (count = fw_block->count; count; count--)
		sif_regs->datainc = fill;
	    break;
	    
	  default:
	    errmsg(&msgsym(BADFIRMTYPE, TR), idb->unit, "download",
		   fw->type, fw);
	    reset_interrupt_level(level);
	    return(FALSE);
	}
    }
    /*
     * Read it back and verify it:
     */
    fw = (madge_fw *)eagle->data;
    for ( ; fw->length != 0; fw = (madge_fw *)((uchar *)fw + fw->length)) {
	switch (fw->type) {
	    
	  case MADGE_DL_HEADER:
	    break;
	    
	  case MADGE_DL_32DATA:
	    fw_data = (madge_fw_32data *)fw;
	    sif_regs->adrx = fw_data->address_h;
	    sif_regs->adr  = fw_data->address_l;
	    offset = 0;
	    for (count=fw_data->count, sptr=fw_data->data;
		 count;
		 count--, offset++) {
		ushort expected, got;
		
		expected = *sptr++;
		got = sif_regs->datainc;
		if (got != expected) {
		    *fail_p = (ulong)SCA_FAIL_TIUCODE;
		    reset_interrupt_level(level);
		    errmsg(&msgsym(BADFIRM, TR), idb->unit, got, expected,
			   fw_data->address_h, (fw_data->address_l + offset));
		    return(FALSE);
		}
	    }
	    break;
	    
	  case MADGE_DL_32BLOCK:
	    break;
	    
	  default:
	    errmsg(&msgsym(BADFIRMTYPE, TR), idb->unit, "verify",
		   fw->type, fw);
	    reset_interrupt_level(level);
	    return(FALSE);
	}
    }
    
    /*
     * It's there and verified.  Start the Bring Up Diagnostics (BUD):
     */
    sif_regs->actl = TRACTL_CONTINUE;
    idb_mark_unthrottled(idb);
 
    reset_interrupt_level(level);
    
    /*
     * TMS380 User's Guide says that we should try this up to three times:
     */
    for (count = 0; count < 3; count++) {
	
	/*
	 * Wait for the processor to perform diagnostics and interrupt:
	 */
	for (time = 0; time < TMS380_INIT_TIMEOUT; time++) {
	    level = set_interrupt_level(ALL_ENABLE);
	    process_sleep_for(500);		/* Wait 0.5 sec each time */
	    reset_interrupt_level(level);
	    intval = sif_regs->intreg;	/* Get status */
	    
	    /*
	     * We're done if we got a successfull completion,
	     * and the error bits are clear:
	     */
	    if ((intval & TRINT_INIT) &&
		(intval & (TRINT_TEST | TRINT_ERROR)) == 0) {
		*fail_p = 0;
		END_TIME_380("hardreset380");
		return(TRUE);
	    }
	}
	
	/*
	 * Do a soft reset to try the BUD again:
	 */
	sif_regs->intreg = SIGNAL_SOFTRESET;
    }
    
    /*
     * Nothing seemed to work or we aborted.
     */
    *fail_p = (ulong)intval;
#ifdef DEBUG
    switch(intval & 0xF) {
      case 0:
	  ttyprintf(CONTTY, "BUD: Initial Test Error\n");
	break;
      case 1:
	  ttyprintf(CONTTY, "BUD: Adapter Software Checksum Error\n");
	break;
      case 2:
	  ttyprintf(CONTTY, "BUD: Adapter RAM Error\n");
	break;
      case 3:
	  ttyprintf(CONTTY, "BUD: Instruction Test Error\n");
	break;
      case 4:
	  ttyprintf(CONTTY, "BUD: Context/Interrupt Test Error\n");
	break;
      case 5:
	  ttyprintf(CONTTY, "BUD: Protocol Handler/RI Hardware Error\n");
	break;
      case 6:
	  ttyprintf(CONTTY, "BUD: System Interface Register Error\n");
	break;
      default:
	  ttyprintf(CONTTY, "BUD: Unknown error code = %d\n", (intval & 0xF));
	break;
    }
    ttyprintf(CONTTY,
	      "TR(%d): hardreset380: BUD Test FAILURE: *fail_p=%#x\n",
	      idb->unit, *fail_p);
#endif
    return(FALSE);
}


/************************************************************************
 *
 * softreset380: restart the TI 380.
 *
 * This routine will reset the TI chip set and then waits for
 * proper completion of the diagnositics that are built into
 * the chip set.
 *
 * It will return TRUE if everything goes well.  FALSE other wise
 * along with the failure reasons returned in fail_p.
 * 
 ************************************************************************/

boolean softreset380 (hwidbtype *idb,
		      ulong *fail_p)
{
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    ushort intval;
    short time;
    leveltype level;
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    
    /*
     * Reset the TI processor, softly.
     */

    tms380_disable_host_interrupts(idb);

    sif_regs->intreg = SIGNAL_SOFTRESET;
    idb_mark_unthrottled(idb);
    level = set_interrupt_level(ALL_ENABLE);
    process_sleep_for(500);		/* Wait before polling TMS380 */
    reset_interrupt_level(level);
    
    /*
     * Wait for the processor to perform diagnostics and interrupt.
     */
    for (time = 0; time < TMS380_INIT_TIMEOUT; time++) {
	intval = sif_regs->intreg;
	if ((intval & TRINT_INIT) || ((intval & TRINT_TEST) &&
				      (intval & TRINT_ERROR)))
	    break;
	level = set_interrupt_level(ALL_ENABLE);
	process_sleep_for(500);		/* Wait 0.5 sec each time */
	reset_interrupt_level(level);
    }
    if ((intval & TRINT_INIT) && (intval & (TRINT_TEST | TRINT_ERROR)) == 0) {
	*fail_p = 0;
	tms380_enable_host_interrupts(idb);
	return(TRUE);
    }
    /*
     * Nothing seemed to work or we aborted.
     */
    *fail_p = (ulong) intval;
    tms380_enable_host_interrupts(idb);
    return(FALSE);
}


/*
 * Disable host interrupts from the TMS380:
 */

void tms380_disable_host_interrupts (hwidbtype *idb)
{
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    ushort old_actl, new_actl;
    
    MSG_TIME_380("disable_host_interrupts","");
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    
    /*
     * Disable host interrupts:
     */
    old_actl = sif_regs->actl;
    new_actl = old_actl & ~(TRACTL_SINTEN | TRACTL_PSDMAEN);
    sif_regs->actl = new_actl;
}

/*
 * Enable host interrupts from the TMS380:
 */

void tms380_enable_host_interrupts (hwidbtype *idb)
{
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    ushort new_actl, old_actl;
    
    MSG_TIME_380("enable_host_interrupts","");
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    
    /*
     * Enable TMS380 interrupts:
     */
    old_actl = sif_regs->actl;
    new_actl = old_actl | TRACTL_SINTEN; /* Enable TMS380 interrupts */
    sif_regs->actl = new_actl;
}

/*
 * Clear any existing interrupts in the TMS380.
 * Return false if we detect an adapter check:
 */

static boolean tms380_clear_all_interrupts (hwidbtype *idb)
{
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    volatile ushort *sifint_p;
    ushort intval;
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    sifint_p = &sif_regs->intreg;
    
    tms380_disable_host_interrupts(idb);
    
    /*
     * Now clear all interrupt causes in the TMS380:
     */
    intval = READ_SIF(sifint_p);
    while (intval & MADGE_SYSINT) {
	if ((intval & MADGE_SYSINT) && (intval & MADGE_CHECK)) {
	    tms380_adapter_check(idb, intval, 5);
	    return(FALSE);
	}
	if ((intval & MADGE_SRB_MASK) == MADGE_SRB_FREE) {
	    WRITE_SIF(sifint_p, SIGNAL_SRB_FREE);
	}
	if ((intval & MADGE_ARB_MASK) == MADGE_ARB_REQ) {
	    WRITE_SIF(sifint_p, SIGNAL_ARB_FREE);
	}
	if (!(intval & MADGE_ARB_MASK)) {
	    WRITE_SIF(sifint_p, CLEAR_INTERRUPT);
	}
	intval = READ_SIF(sifint_p);
    }
    return(TRUE);
}


/*
 * for spinning for very short periods of time like for
 * setting group and functional addresses and modify open.
 */
static void tms380_rapid_spin_wait (hwidbtype *idb)
{
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    volatile ushort *sifint_p;
    leveltype level;
    ushort intval;
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    sifint_p = &sif_regs->intreg;
    
    level = raise_interrupt_level(ALL_DISABLE);
    
    while(1) {
	intval = READ_SIF(sifint_p);
	if ((intval & MADGE_SRB_MASK) == MADGE_SRB_FREE) {
	    break;
	}
    }
    
    reset_interrupt_level(level);
}


/*
 * tms380_wait_for_completion
 * Wait for the TMS380 to complete the operation that we started.
 * Poll the TMS380 interrupt register poll_count times, waiting sleep_time
 * ms between each attempt. Return FALSE if the expected event never
 * happened, or if we get an adapter check. Return value of intval if we
 * got a Madge sysint completion interrupt.
 */

static boolean tms380_wait_for_completion (hwidbtype *idb,
					   int poll_count,
					   int sleep_time,
					   uint timeout_error_code,
					   ulong *fail_p)
{
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    volatile ushort *sifint_p;
    short time;
    ushort intval;
    
    ds = INSTANCE;
    
    sif_regs = SIF_REGS;
    sifint_p = &sif_regs->intreg;
    
    intval = 0;
    time = 0;
    while (time++ < poll_count) {
	if (ds->adapter_reset) {
	    *fail_p = RSLT_FAIL_BOARD_RESET;
	    return(FALSE);
	}
	intval = READ_SIF(sifint_p);
	if (!(intval & MADGE_SYSINT)) {
	    leveltype level;
	    
	    /*
	     * the spin wait is equivalent to the way hi-end dudes do it.
	     * see sbe16_immedcmd().  this should only be called
	     * when we receive a lan network manager frame to do
	     * lan manager thang.
	     */
	    
	    if (schedflag || onintstack()) {
		sys_timestamp timer;
		TIMER_START(timer, sleep_time);
		while (SLEEPING(timer))
		    ;
	    }  else {
		level = set_interrupt_level(ALL_ENABLE);
		process_sleep_for(sleep_time);	/* Let other processes run,
						   we wait */
		reset_interrupt_level(level);
	    }
	    continue;
	}
	if (intval & MADGE_CHECK) {
	    tms380_adapter_check(idb, intval, 6);
	    return(FALSE);
	}
	if ((intval & MADGE_SRB_MASK) == MADGE_SRB_FREE) {
	    break;
	}
	/* Not something we are interested in. Ignore it: */
	WRITE_SIF(sifint_p, CLEAR_INTERRUPT);
    }
    if (time > poll_count) {
	tms380_certain_death(idb, timeout_error_code, time);
	return(FALSE);
    }
    return(intval);
}


/************************************************************************
 *
 * open380: open and insert the interface into the ring.
 *
 * returns TRUE if all is cool  False if it didn't work.  fail_p
 * points at the cell updated with failure info.  Blk points at
 * the start block which contains amung other things the node_addr
 * we should use for this interface.  Flags contains information about
 * how we should open up.  The only thing it does is indicate if we
 * should put the interface in wrap mode.  This has to be done on the
 * open because chgmode380 can't do it via a MODIFY_OPEN_PARMS.
 *
 * We assume that all parameter checking has already been done.  This
 * means the only failures that may happen come from the chipset.
 *
 ************************************************************************/

boolean open380 (hwidbtype *idb,
		 ulong *fail_p)
{
    srb_open_adapter *open_blk;
    struct _sifreg_t *sif_regs;
    volatile ushort *sifint_p;
    tms380_instance_t *ds;
    boolean display;
    ushort result, *ptr;
    ushort phase, error;
    leveltype level;
    tokensb *tsb;
    
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    sifint_p = &sif_regs->intreg;
    
    /*
     * Clear any current interrupts. Return if adapter check.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    if (!tms380_clear_all_interrupts(idb)) {
	ds->if_state = IF_DOWN;
	net_cstate(idb, IDBS_DOWN);
	reset_interrupt_level(level);
	return(FALSE);
    }
    
    /*
     * Build the open block.
     */
    open_blk = malloc_iomem(SRB_OPEN_ADAPTER_SIZE);
    if (!open_blk) {
	*fail_p = RSLT_FAIL_NOMEMORY;
	ds->if_state = IF_DOWN;
	net_cstate(idb, IDBS_DOWN);
	reset_interrupt_level(level);
	return(FALSE);
    }
    open_blk->hdr.function = DIR_OPEN_ADAPTER;
    open_blk->open_options = OPEN_OPT_VAL;

    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
    
    /*
     * Turn on a subset of the open flags.  Turning on all things like
     * CAF can swamp the Eagle such that the open doesn't complete, but
     * times out instead.  Turn these on AFTER the open completes.
     */
    if (idb->tr_ring_mode & RINGMODE_LOOPBACK)
	open_blk->open_options |= OPN_WRAP;
    if (idb->tr_ring_mode & RINGMODE_CONTENDER)
	open_blk->open_options &= ~(OPN_CONTENDER);
    if (idb->tr_ring_mode & RINGMODE_ETR)
	open_blk->open_options |= (OPN_EARLY_RELEASE);
    if (idb->tr_ring_mode & RINGMODE_NO_RINGSTAT)
	open_blk->open_options |= (OPN_NOHARD | OPN_NOSOFT);
    ds->last_open_options = open_blk->open_options;
    ieee_copy(ds->node_addr, open_blk->address);
    open_blk->group_address = tsb->tr_group;
    open_blk->functional_address = tsb->tr_functional;
    
    /*
     * Check for the NULL address.  Ths means that we have to copy the
     * 'burned in' address from non-vol into the open block.  We can't
     * let the TI chipset do this because the it doesn't have any ROMs.
     */
    if (is_ieee_zero(open_blk->address)) {
	ieee_copy(idb->bia, ds->node_addr);
	ieee_copy(idb->bia, open_blk->address);
    }
    
    /*
     * Make sure we haven't been reset again:
     */
    if (ds->adapter_reset) {
	*fail_p = RSLT_FAIL_BOARD_RESET;
	ds->if_state = IF_DOWN;
	net_cstate(idb, IDBS_DOWN);
	reset_interrupt_level(level);
	free(open_blk);
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(FALSE);
    }
    
    /*
     * Send the open command.
     */
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->srb_ptr;
    for (ptr = (ushort *)open_blk; ptr < (ushort *)(open_blk+1); ptr++)
	sif_regs->datainc = *ptr;
    reset_interrupt_level(level);
    WRITE_SIF(sifint_p, SIGNAL_SRB_CMD);
    free(open_blk);
    
    /*
     * Wait for the Open response:
     */
    if (!tms380_wait_for_completion(idb, TMS380_OPEN_TIMEOUT, ONESEC,
				    SCA_FAIL_OPEN_TIMEOUT, fail_p)) {
	if (idb->state != IDBS_ADMINDOWN) {
	    ds->if_state = IF_DOWN;
	    net_cstate(idb, IDBS_DOWN);
	}
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(FALSE);
    }
    /*
     * Reject or success?
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->srb_ptr + OFFSET_OPEN_RESPONSE;
    result = sif_regs->data;
    if ((result >> 8) != E_00_SUCCESS) {
	sif_regs->adr  = ds->srb_ptr + OFFSET_OPEN_ERROR;
	reset_interrupt_level(level);
	/*
	 * fail code is 0xAABBCCCC
	 *
	 *   AA = Open command (0x03)
	 *   BB = Madge error code
	 * CCCC = TI error code
	 */
	*fail_p = (DIR_OPEN_ADAPTER << 24) |
	    ((result & 0xFF00) << 8) |
		sif_regs->data;
	WRITE_SIF(sifint_p, SIGNAL_SRB_FREE);
	display = TRUE;
	phase = ((*fail_p & OPN_PHASE_MASK) >> 4);
	error = (*fail_p & OPN_CODE_MASK);
	if (((*fail_p >> 24) == 0x03) && 
	    (phase < TR_OPN_PHASES) &&
	    (error < TR_OPN_ERRORS)) {
	    if (SLEEPING(tsb->tr_next_buginf_time))
		display = FALSE;
	    tsb->ringOpenStatus = error;

            /*SZ*/
            TR_OpenError(idb, error, phase, display);
	}
	TIMER_START(tsb->tr_next_buginf_time, TR_BUGINF_INTERVAL);
	if (idb->state != IDBS_ADMINDOWN) {
	    ds->if_state = IF_DOWN;
	    net_cstate(idb, IDBS_DOWN);
	}
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(FALSE);
    }
    *fail_p = 0;
    reset_interrupt_level(level);
    WRITE_SIF(sifint_p, SIGNAL_SRB_FREE);
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
    return(TRUE);
}


/************************************************************************
 *
 * close380: shut the TI 380 down.
 *
 * This routine tries to shut the Token Ring chip set down gracefully.
 *
 ************************************************************************/

void close380 (hwidbtype *idb)
{
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    volatile ushort   *sifint_p;
    leveltype level;
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    sifint_p = &sif_regs->intreg;
    
    /*
     * Clear any current interrupts. Return if adapter check.
     */
    if (!tms380_clear_all_interrupts(idb))
	return;
    
    /*
     * Send a close command.  Poke it directly since it's so small.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->srb_ptr;
    sif_regs->datainc = (DIR_CLOSE_ADAPTER << 8) | 0;
    sif_regs->datainc = 0;
    reset_interrupt_level(level);
    WRITE_SIF(sifint_p, SIGNAL_SRB_CMD);
    return;
}


/**********************************************************************
 *
 * chgmode380: change operational mode of the ring.
 *
 * This routine is responsible for arming any special modes that are
 * needed.  It does this only for the ti chip set.  Any system
 * implications is handled else where.  The behaviour is determined
 * by the current settings of ds->c_blk.ring_mode.  No provision is
 * made for backing out of a mode.
 *
 * returns true if successful.
 *
 * A brief description of what it is:
 *
 * RINGMODE_LOOPBACK		put the ring interface into internal loopback
 * 				this effects the chip set directly.  The only
 * 				way out is to reset the chip set but not to
 * 				worry because mother resets us completely.
 * 				This is done in Open380.
 *
 * RINGMODE_NO_RINGSTAT		A flag to the tms380_sif_handler to not pass
 * 				ring status messages up to Mom.  This does
 * 				not effect the ti chip set.  This is done
 * 				in Open380.
 *
 * RINGMODE_ALL_FRAMES		Receive all frames.  Put the chipset into
 * 				fully promiscous mode.  This is used when
 * 				the system is being used as a monitor.  This
 * 				will hand both LLC and MAC frames that are
 * 				seen upto Mom.
 *
 * RINGMODE_ALL_LLC		Only receive LLC packets.  This does the
 * 				same thing to the chip set as ALL_FRAMES
 * 				except only LLC frames are passed.
 * 				
 * RINGMODE_BRIDGE		Put the chip into the appropriate mode
 * 				for source bridging.  If a software only
 * 				bridge this put the system into ALL_LLC
 * 				mode.  If hardware it will load the
 * 				appropriate information into the hardware
 * 				registers that control this nonsense.
 *
 * RINGMODE_REM			Put the chipset into the appropriate state
 * 				so it can receive Ring Error Monitor
 * 				MAC packets.  This enables the REM func
 * 				address.
 *
 * RINGMODE_RPS			Ditto to the above only it Ring Parameter
 * 				Server.  The only parameter supplied
 * 				to the ring is ring number.  We do this
 * 				because IBM. does it.  This enables the
 * 				RPS functional address.
 *
 * RINGMODE_NETMAN		Ditto. But for Network Management.
 *
 *
 * If we need to do anything to the chip we sit on interrupts from it.
 * This may loose the first few ring status or similar messages.
 *
 * NOTE:  This routine is only additive.  It adds modes to whatever
 * open380 set the open modes to.
 *
 **********************************************************************/

boolean chgmode380 (hwidbtype *idb,
		    ulong *fail_p)
{
    volatile ushort *sifint_p;
    ushort result, newstate;
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    leveltype level;
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    sifint_p = &sif_regs->intreg;
    
    *fail_p = 0;

    /*
     * Check for NULL remif struct.  This is here to support tunneled
     * token ring to a UNIX router
     */
    if ((idb->remif == NULL) && ((ds->if_state != IF_OPENING) &&
				 (ds->if_state != IF_AUTOFIX) &&
				 (ds->if_state != IF_UP))) {
	*fail_p = (ulong) -1;
	return(FALSE);
    }

    tms380_disable_host_interrupts(idb);

    if (tokenevent_debug) {
	buginf("ring mode = %x\n",idb->tr_ring_mode);
    }

    /*
     * Recalculate the open options based on the new RINGSTAT value.
     * NOTE:  The hack at the end is required because of the way that
     * interpreted the Open Options in the TI specification.
     */
    newstate = OPEN_OPT_VAL;
    if (idb->tr_ring_mode & RINGMODE_CONTENDER)
	newstate &= ~(OPN_CONTENDER);

    if (idb->tr_ring_mode & RINGMODE_ETR)
	newstate |= (OPN_EARLY_RELEASE);

    if (idb->tr_ring_mode & RINGMODE_NO_RINGSTAT)
	newstate |= (OPN_NOHARD | OPN_NOSOFT);

    if (idb->tr_ring_mode & RINGMODE_RS)
	newstate |= (OPN_PASS_ADPT_MAC | OPN_PASS_ATTN_MAC | OPN_PASS_BEACON);

    if (idb->tr_ring_mode & (RINGMODE_REM))
	newstate |= (OPN_PASS_BEACON);

    if (idb->tr_ring_mode & (RINGMODE_ALL_LLC | RINGMODE_TBRIDGE))
	newstate |= (OPN_COPY_NON_MAC);

    if (idb->tr_ring_mode & RINGMODE_ALL_MAC)
	newstate |= (OPN_COPY_ALL_MAC);

    if (idb->tr_ring_mode & RINGMODE_ALL_FRAMES)
	newstate |= (OPN_PASS_ADPT_MAC | OPN_PASS_ATTN_MAC | OPN_PASS_BEACON |
		     OPN_COPY_ALL_MAC | OPN_COPY_NON_MAC);

    if (idb->tr_ring_mode & (RINGMODE_REM | RINGMODE_RPS | RINGMODE_NETMGR))
	newstate |= OPN_PASS_ADPT_MAC;

    ds->last_open_options = newstate;

    if (tokenevent_debug)
	buginf("\nTR%d: modified open w/ option %x\n",idb->unit, newstate);

    /*
     * Send a Modify Open Parameters command.  Poke it directly since
     * it's so small.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->srb_ptr;
    sif_regs->datainc = (DIR_MODIFY_OPEN_PARMS << 8) | 0;
    sif_regs->datainc = 0;
    sif_regs->datainc = newstate;
    reset_interrupt_level(level);
    WRITE_SIF(sifint_p, SIGNAL_SRB_CMD);
    
    DBG2(DBG_SRB, "TR(%d): chgmode380: newstate=%#x\n",
	 idb->unit, newstate);
    
    /*
     * Wait for the change mode response.
     */
    tms380_rapid_spin_wait(idb);
    
    /*
     * Reject or success?
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->srb_ptr + OFFSET_MODIFY_RESPONSE;
    result = sif_regs->data;
    reset_interrupt_level(level);
    if ((result >> 8) != E_00_SUCCESS) {
	*fail_p = (ulong) ((result << 16) | 0);
	WRITE_SIF(sifint_p, SIGNAL_SRB_FREE);
	tms380_enable_host_interrupts(idb);
	return(FALSE);
    }
    WRITE_SIF(sifint_p, SIGNAL_SRB_FREE);
    tms380_enable_host_interrupts(idb);
    return(TRUE);
}


/**********************************************************************
 *
 * cfgbridge380
 *
 * Configure the Source Routing Accelerator chip with the current
 * bridging parameters.  These are the local ring, bridge number, remote
 * ring, and whether this is part of the spanning tree.  This routine
 * provides the size of the bridge number partition which is, by
 * consensus, four bits wide.
 *
 *
 * *** NOTE *** The only way to turn off the SRA chip is to mis-configure
 * it.  We use the enable flag to tell us whether or not to deliberately
 * misconfigure the SRA chip and ignore the resulting error message.
 *
 **********************************************************************/

boolean cfgbridge380 (hwidbtype *idb,
		      boolean enable,
		      ulong *fail_p)
{
    idbtype *swidb;
    volatile ushort *sifint_p;
    srb_set_ti_bridge_parms *bridge_blk;
    srb_triplet_t srb_triplet;
    srb_vector_table_t *srbV_p;
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    ushort result, *ptr;
    leveltype level;
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    sifint_p = &sif_regs->intreg;
    
    *fail_p = 0;
    
    tms380_disable_host_interrupts(idb);
    
    /*
     * Build a Set Bridge Parameters command
     */
    bridge_blk = malloc_iomem(SRB_SET_TI_BRIDGE_PARMS_SIZE);
    if (!bridge_blk) {
	*fail_p = RSLT_FAIL_NOMEMORY;
	return(FALSE);
    }
    bridge_blk->hdr.function = DIR_SET_BRIDGE_PARMS;
    bridge_blk->maxrif       = ds->max_rd * 2 + 2;
    bridge_blk->partlen      = 4;

    swidb = firstsw_or_null(idb);

    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        srbV_p->srb_sb_get_triplet(swidb, &srb_triplet);
        bridge_blk->this_ring   = srb_triplet.localRing;
        bridge_blk->bridge_num  = (enable ? srb_triplet.bridgeNum 
                                          : SRB_INVALID_BRIDGE_NUM);
        bridge_blk->that_ring   = srb_triplet.remoteRing;
        bridge_blk->sngrtebcast = srbV_p->srb_sb_get_spanning_explorer(swidb);
    } else {
        bridge_blk->this_ring   = SRB_INVALID_RING_NUM;
        bridge_blk->bridge_num  = SRB_INVALID_BRIDGE_NUM;
        bridge_blk->that_ring   = SRB_INVALID_RING_NUM;
        bridge_blk->sngrtebcast = FALSE;

    }
    
    /*
     * Send the configure bridge command.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->srb_ptr;
    for (ptr = (ushort *)bridge_blk; ptr < (ushort *)(bridge_blk+1); ptr++)
	sif_regs->datainc = *ptr;
    reset_interrupt_level(level);
    WRITE_SIF(sifint_p, SIGNAL_SRB_CMD);
    free(bridge_blk);
    
    DBG3(DBG_SRB,
	 "TR(%d): cfgbridge380: bridge parms r=%d, tr=%d \n",
	  idb->unit, srb_triplet.localRing, srb_triplet.targetRing);
    
    /*
     * Wait for the configure bridge response.
     */
    
    tms380_rapid_spin_wait(idb);
    
    /*
     * Reject or success?
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->srb_ptr + OFFSET_SETBRPARAM_RESPONSE;
    result = sif_regs->data;
    reset_interrupt_level(level);
    if (enable && ((result >> 8) != E_00_SUCCESS)) {
	*fail_p = (result << 16) | 0;
	WRITE_SIF(sifint_p, SIGNAL_SRB_FREE);
	DBG2(DBG_SRB, 
	     "TR(%d): cfgbridge380: Bad result code=%#x\n",
	     idb->unit, result);
	tms380_enable_host_interrupts(idb);
	return(FALSE);
    }
    
    *fail_p = 0;
    WRITE_SIF(sifint_p, SIGNAL_SRB_FREE);
    tms380_enable_host_interrupts(idb);
    return(TRUE);
}


/**********************************************************************
 *
 * set_address_380
 *
 * Change the group address that the adapter is accepting.
 *
 **********************************************************************/

boolean set_address_380 (hwidbtype *idb,
			 ushort command,
			 ulong address,
			 ulong *fail_p)
{
    volatile ushort *sifint_p;
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    ushort result;
    leveltype level;
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    sifint_p = &sif_regs->intreg;
    
    if (tokenevent_debug) {
	if (command == DIR_SET_FUNCTIONAL_ADDRESS)
	    buginf("setting functional address w/ %x\n",address);
	else
	    buginf("setting group address w/ %x\n",address);
    }
    
    *fail_p = 0;

    tms380_disable_host_interrupts(idb);

    /*
     * Send a Set Group/Functional Address command.  Poke it directly
     * since it's so small.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->srb_ptr;
    sif_regs->datainc = (command << 8) | 0;       /* fn, corr */
    sif_regs->datainc = 0;			  /* return, resvd */
    sif_regs->datainc = 0;			  /* reserved */
    sif_regs->datainc = address >> 16;            /* address  */
    sif_regs->datainc = address & 0xffff;
    reset_interrupt_level(level);
    WRITE_SIF(sifint_p, SIGNAL_SRB_CMD);
    
    DBG3(DBG_SRB, 
	 "TR(%d): set_address_380: cmd = %#x, address = %#x\n",
	 idb->unit, command, address);
    /*
     * Wait for the set group/functional response.
     */
    tms380_rapid_spin_wait(idb);
    
    /*
     * Reject or success?
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->srb_ptr + OFFSET_SET_ADDRESS_RESPONSE;
    result = sif_regs->data;
    reset_interrupt_level(level);
    if ((result >> 8) != E_00_SUCCESS) {
	*fail_p = (result << 16) | 0;
	DBG2(DBG_SRB, 
	     "TR(%d): set_address_380: Bad result code=%#x\n",
	     idb->unit, result);
	return(FALSE);
    }
    
    WRITE_SIF(sifint_p, SIGNAL_SRB_FREE);
    *fail_p = 0;
    return(TRUE);
}


/*
 * tms380_get_stats: get internal token ring statistics.
 *
 * This routine checks to see if the interface is alive and
 * if so gets an update of its dynamic cells.
 */

boolean tms380_get_stats (hwidbtype *idb)
{
    tms380_instance_t *ds;
    leveltype level;
    
    ds = INSTANCE;
    if (!ds)
	return(FALSE);
    
    level = raise_interrupt_level(NETS_DISABLE);
    bcopy((uchar *)&ds->active_counts, (uchar *)&ds->frozen_counts,
	  sizeof(sbe_counters_t));
    memset(&ds->active_counts, 0, sizeof(sbe_counters_t));
    memset(&ds->active_errors, 0, sizeof(madge_smt_errors_t));
    reset_interrupt_level(level);
    return(TRUE);
}

/*
 * for invoking from tms380_show_controller() only
 * the same as tms380_get_stats() but it zero out nothing
 */
boolean tms380_get_stats_copy (hwidbtype *idb)
{
    tms380_instance_t *ds;
    leveltype level;
   
    ds = INSTANCE;
    if (!ds)
        return(FALSE);

    level = raise_interrupt_level(NETS_DISABLE);
    bcopy((uchar *)&ds->active_counts, (uchar *)&ds->frozen_counts,
          sizeof(sbe_counters_t));
    reset_interrupt_level(level);
    return(TRUE);
}

/**********************************************************************
 *
 * grab_380_smt_stats
 *
 * Read the current SMT values from the TI chipset.
 *
 **********************************************************************/

void grab_380_smt_stats (hwidbtype *idb)
{
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    leveltype level;
    ushort *ptr;
    int i;
    
    ds = INSTANCE;
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs = SIF_REGS;
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr = ds->address_addr;
    
    ptr = (ushort *)&ds->frozen_stats.mac;
    for (i = 0; i < SMT_TI_ADDRESS/2; i++)
	*ptr++ = sif_regs->datainc;
    
    sif_regs->adr = ds->parm_addr;
    ptr = (ushort *)&ds->frozen_stats.physdrop;
    for (i = 0; i < SMT_TI_PARMS/2; i++)
	*ptr++ = sif_regs->datainc;
    
    reset_interrupt_level(level);
}


/**********************************************************************
 *
 * read_380_error_log
 *
 * Read the current error counters from the TI chipset.
 *
 **********************************************************************/

static boolean read_380_error_log (hwidbtype *idb)
{
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    srb_dir_read_log log;
    volatile ushort *sifint_p;
    ushort result, *ptr, *end;
    leveltype level;
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    sifint_p = &sif_regs->intreg;
    
    trebuginf("\nTR%d: reading error log", idb->unit);
    
    level = raise_interrupt_level(NETS_DISABLE);
    tms380_disable_host_interrupts(idb);

    /*
     * Send a Read Error Log command.  Poke it directly
     * since it's so small.
     */
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->srb_ptr;
    sif_regs->datainc = (DIR_READ_LOG << 8) | 0;  /* fn, corr */
    sif_regs->datainc = -1;			  /* return, resvd */
    ds->error_log_counter++;
    WRITE_SIF(sifint_p, SIGNAL_SRB_CMD);
    reset_interrupt_level(level);
    
    /*
     * Wait for the read error log response.
     */
    tms380_rapid_spin_wait(idb);
    
    /*
     * Reject or success?
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->srb_ptr + OFFSET_READ_LOG_RESPONSE;
    result = sif_regs->data;
    WRITE_SIF(sifint_p, SIGNAL_SRB_FREE);
    reset_interrupt_level(level);

    if ((result >> 8) != E_00_SUCCESS) {
        ds->error_log_failures++;
	trebuginf("failed, bad return code %#x", result);
	tms380_panic_to_mom(idb, SCA_FAIL_BADSRBCMD, 1,
			    (ulong)READ_SIF(sifint_p), (ulong)result);
	tms380_enable_host_interrupts(idb);
	return(FALSE);
    }
    
    /*
     * Copy out the data.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = ds->srb_ptr;
    end = (ushort *)((uchar *)&log + SRB_DIR_READ_LOG_SIZE);
    for (ptr = (ushort *)&log; ptr < end; ptr++)
	*ptr = sif_regs->datainc;
    reset_interrupt_level(level);
    
    /*
     * Record it.
     * Record to ds->log380_errors instead of ds->active_errors
     */
    ds->log380_errors.err_line          = log.line_errors;
    ds->log380_errors.err_internal      = log.internal_errors;
    ds->log380_errors.err_burst         = log.burst_errors;
    ds->log380_errors.err_arifci        = log.a_c_error;
    ds->log380_errors.err_abort         = log.abort_delimiter;

    ds->log380_errors.err_lostframe     = log.lost_frame;
    ds->log380_errors.err_rcvcongestion = log.rx_congestion;
    ds->log380_errors.err_copy          = log.fr_copied_errors;
    ds->log380_errors.err_freq          = log.freq_errors;
    ds->log380_errors.err_token         = log.token_errors;
    /*
     * Add it to ds->active_errors (count since last show)
     */
    ds->active_errors.err_line          += log.line_errors;
    ds->active_errors.err_internal      += log.internal_errors;
    ds->active_errors.err_burst         += log.burst_errors;
    ds->active_errors.err_arifci        += log.a_c_error;
    ds->active_errors.err_abort         += log.abort_delimiter;
    
    ds->active_errors.err_lostframe     += log.lost_frame;
    ds->active_errors.err_rcvcongestion += log.rx_congestion;
    ds->active_errors.err_copy          += log.fr_copied_errors;
    ds->active_errors.err_freq          += log.freq_errors;
    ds->active_errors.err_token         += log.token_errors;

    tms380_add_counters(idb);

    tms380_enable_host_interrupts(idb);
    return(TRUE);
}


/************************************************************************
 * routines for sif_handler
 ************************************************************************/

void arb_req (hwidbtype *idb,
	      tms380_instance_t *ds,
	      struct _sifreg_t *sif_regs,
	      volatile ushort *sifint_p,
	      ushort intval)
{
    ushort command;
    ushort ring_stat;

    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr = ds->arb_ptr;
    command = sif_regs->data; /* Get the ARB request code */
    sif_regs->datainc = 0;	/* commands always succeed */

    DBG2(DBG_MADGE, "TR(%d): MADGE_ARB_REQ, command=0x%x\n", 
	 idb->unit, command);
    
    switch (command >> 8) {
	
      case ARB_CMD_RING_STATUS:
	sif_regs->adr = ds->arb_ptr + OFFSET_ARB_RING_STATUS;
	ring_stat = sif_regs->data; /* The ring status */
	
	/* Signal that we got the status information */
	sif_regs->adr = ds->arb_ptr + OFFSET_RETURN_CODE;
	sif_regs->data = 0; /* Return code = success */
	WRITE_SIF(sifint_p, SIGNAL_ARB_FREE);
	
	DBG3(DBG_ARB, 
	     "TR(%d): ring status=%#x, intval=%#x\n",
	     idb->unit, ring_stat, intval);
	
	/* Did someone remove us from the ring? */
	if (ring_stat & RNG_AUTOFIX) {
		    
	    /* Right now the interface is ugly and its mother
	     * dresses it funny */
	    ds->if_state = IF_AUTOFIX;
		    
	    DBG1(DBG_ARB,
	      "TR(%d): Removed from ring, setting ds->state to IF_AUTOFIX\n",
	       idb->unit);
		    
	    /* Possibly display a message for this ring status? */
	    tms380_possibly_display_ringstat(idb, ring_stat);
	    return;
	}
	
	/* Did the TMS380's stats overflow?
	 * If so, arrange for mk5025_periodic to read the error log */
	if (ring_stat & RNG_CNT_OVRFLW)
	    read_380_error_log(idb);
	
	/*
	 * Are we interested in hearing these?
	 */
	if ((idb->tr_ring_mode & RINGMODE_NO_RINGSTAT) == 0) {
	    tms380_possibly_display_ringstat(idb, ring_stat);
	}
	break;
	
      default:
	tms380_certain_death(idb, SCA_FAIL_BADARBCMD,
			     (ulong) command);
	return;
    }
}
