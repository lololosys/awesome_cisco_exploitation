/* $Id: if_les_biga.c,v 3.2.62.2 1996/08/28 13:16:25 thille Exp $
 * $Source: /release/112/cvs/Xsys/syn-biga/if_les_biga.c,v $
 *------------------------------------------------------------------
 * if_les_biga.c - Synergy In-band I/F (BIGA) driver.
 *
 * May 26 1995, Jin-Jang Chen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_biga.c,v $
 * Revision 3.2.62.2  1996/08/28  13:16:25  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/05/09  14:46:59  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:26:41  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.1  1996/04/08  02:09:55  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.2  1996/04/02  04:59:53  rlowe
 * Fix up more include file paths.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.68.1  1996/03/22  09:44:10  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.1  1996/03/03  21:25:03  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.50.1  1996/03/02  01:21:19  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.34.1  1996/02/29  07:50:24  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  18:57:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:32:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:02:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "exception.h"
#include "../if/ether.h"
#include "../srt/span.h"
#include "config.h"
#include "../wan/serial.h"  
#include "ttysrv.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "sched.h"

#include "../snmp/snmp_api.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../snmp/snmp_registry.h"
#include "../les/if_les.h"
#include "../syn-biga/if_les_biga.h"  

#include "../if/network.h"
#include "../ip/ip.h"
#include "../dev/monitor1.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../if/tring.h"

#include "../if/ether_inline.h"

#include "../ip/remif_proto.h"

#include "../../wbu/cat5k/alc_cominc/basic_types.h"
#include "../../wbu/cat5k/alc_include/acp_basic_types.h"
#include "../../wbu/cat5k/alc_cominc/sar_if.h"
#include "../../wbu/cat5k/alc_cominc/atmdrv_if.h"

/* MAXETHERSIZE */


/*
 * BIGA subsystem header
 */

#define BIGA_MAJVERSION   1
#define BIGA_MINVERSION   0
#define BIGA_EDITVERSION  1

SUBSYS_HEADER(biga, BIGA_MAJVERSION, BIGA_MINVERSION, BIGA_EDITVERSION,
	      biga_subsys_init, SUBSYS_CLASS_DRIVER,
	      "seq: atmSyn",
	      "req: atmSyn");


/*
 * biga_linestate
 * Return current line protocol status
 */

static boolean biga_linestate (hwidbtype *hwidb)
{
    /*
     * Synergy backplane biga I/F is always ON.
     */
    return(TRUE);
}

/*
 * Common IDB initialization routine:
 */

void
init_biga_idb_common (hwidbtype *idb)
{
    idbtype *swidb = idb->firstsw;

#ifdef BIGA_PARSER
    static first_time = TRUE;
    if (first_time) {
	biga_parser_init();
	first_time = FALSE;
    }
#endif

    idb->status = IDB_ETHER | IDB_BCAST | IDB_BRIDGE; /* IDB_BIGA */
#ifdef	SYN2FIX		/* Fix this ? */
    idb->type = IDBTYPE_BIGA;
#else
    idb->type = IDBTYPE_LANCE ;		/* Fake for now. */
#endif
    idb->typestring = "BIGA";
    idb->state = IDBS_INIT;
    idb->enctype = ET_ARPA;
    idb->error_threshold = 5000;
    idb->error_count = 0;
    idb->fast_switch_flag = 0;
    idb->rxload = 1;
    idb->span_encapsize = 0;	/* Transparent bridging */

    swidb->tbridge_media = TBR_ETHER;
    
    /*
     * Set up function calls
     */
    idb->soutput = biga_tx_start;
#ifdef	SYN2FIX		/* Fix/delete this ? */
    idb->hardinit = (hardinit_t) return_nothing;
#endif
    idb->reset = biga_restart;
#ifdef	SYN2FIX		/* Broken, Ark-talisker */
    idb->cstate = net_cstate;
#endif
    idb->linestate = biga_linestate;
    idb->shutdown = biga_shutdown;
    idb->periodic = biga_periodic;
    idb->listen = biga_add_multicast_address;
    idb->listen_range = biga_add_multicast_range;
    idb->enable = biga_enable;
#ifdef	SYN2FIX
    idb->setup_mac = (setup_mac_t) NULL;
    idb->setup_ip = (setup_ip_t) NULL;
#endif
    idb->setup_bridging = biga_setup_bridging;
    swidb->span_disabled = FALSE;
    idb->txqlength = biga_txqlength;
    idb->show_controller = biga_show_controller;
    idb->fastsend = biga_fastsend;

    ether_idb_init(idb);
    
    idb_enqueue(idb);

}

/*
 * Initialize the BIGA interface:
 */

void
biga_if_init (hwidbtype *idb, boolean maxdgram_change)
{
    boolean filter = FALSE;	/* Stupid compiler... */
    leveltype SR;

    idbtype *swidb;
#ifdef DEBUG
    volatile ulong call_pc = caller();
#endif

    
    SR = raise_interrupt_level(ETHER_DISABLE);
    
    /*
     * stop the Lance and count the reset:
     */
    
    biga_stop();
    idb->counters.resets++;
    if (idb->state != IDBS_ADMINDOWN) {
	/*
	 * Show that we are being reset:
	 */
#ifdef	SYN2FIX		/* Broken, Ark-talisker */
	(*idb->cstate)(idb, IDBS_RESET);
#endif
    }

#ifdef DEBUG
    buginf("\nBIGA(%d): Reset from 0x%x", idb->unit, call_pc);
#endif

    /*
     * Flush the hold queue:
     */
    holdq_reset(idb);

    /*
     * If interface is administratively down, don't re-enable it.
     * Note that we do this check after shutting down the Lance
     */
    if (idb->state == IDBS_ADMINDOWN) {
	reset_interrupt_level(SR);
	return;
    }

    biga_stop();
    biga_if_reset(idb);

    /*
     * initialize the hardware addresses
     * If our station address has not already been set, default it to the
     * address given in the MCI cookie:
     */
    biga_get_default_hw_address(idb, idb->bia);
    if (is_ieee_zero(idb->hardware))
	biga_get_default_hw_address(idb, idb->hardware); 
    /*
     * Initialize the buffer cache pool pointer
     */
    idb->pool = 0; /* pool_find_by_size(largest_maxdgram);*/

    /* 
     * Call anyone expecting to know about a new MAC address
     */
#ifdef	SYN2FIX
    if (idb->setup_mac)
	(*idb->setup_mac)(idb);
#endif

    /*
     * Initialize bridging only if we are active according to the spanning
     * tree, bridging is enabled, and we are not boot loading:
     */
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	filter = RUNNING_IF(LINK_BRIDGE,swidb);
	if (filter && bridge_enable && !(system_loading | bootstrap_enable)) {
	    /*
	     * Enable promiscuous mode so that we see every packet:
	     */
	    
	    /*
	     * Initialize bridge filter here:
	     */
#ifdef	SYN2FIX		/* Fix this ? */
	    reg_invoke_bridge_myaddress(swidb, idb->hardware);
#endif
	}

	/*
	 * If we are not boot loading, add other multicast addresses:
	 */
	if (!(system_loading | bootstrap_enable))
	    reg_invoke_media_interesting_addresses(swidb);
    }
    
    /*	
     * right place to be up ? Actual biga/txsar initialization is somewhere else.
     */
    idb->state = IDBS_UP;

    reset_interrupt_level(SR);		/* restore interrupt level */
    return;
}

/*
 * Stop the BIGA and reinitialize it.
 * Must be called with network interfaces disabled.
 */
void biga_stop (void)
{
    /*
     * How ?
     */
}

/*
 * Wrapper routine for idb->reset vector:
 */

void
biga_restart (hwidbtype *idb)
{
    leveltype SR;

    SR = raise_interrupt_level(NETS_DISABLE);
#ifdef	SYN2FIX
    if (idb->pakout) {
	datagram_done(idb->pakout);
	idb->pakout = NULL;
    }
#endif
    holdq_reset(idb);		/* Flush input and output queues */
	
    if (idb->state != IDBS_ADMINDOWN)
	biga_if_init(idb, FALSE);	/* Reset interface */
    reset_interrupt_level(SR);
}

/*
 * biga_check
 * If it should be running, ensure that it is
 * running and has a normal value. Otherwise, reset it.
 */

void
biga_check (hwidbtype *idb)
{
    
    /*
     * What to check ?
     */
    if (0)
        biga_if_init(idb, FALSE);
}

/*
 * biga_shutdown
 * Shut down BIGA interface
 */

void
biga_shutdown (hwidbtype *idb, int newstate)
{
    leveltype SR;

#ifdef	SYN2FIX		/* Broken, Ark-talisker */
    (*idb->cstate)(idb, newstate);
#endif

    SR = raise_interrupt_level(ETHER_DISABLE);
    biga_stop();
    biga_if_reset(idb);
    reset_interrupt_level(SR);
}

/*
 * biga_add_multicast_address
 * Add another ethernet hardware address to the logical address filter
 */

void
biga_add_multicast_address (idbtype *swidb, uchar *multicast_address,
			     uchar fsm)
{
    /*
     * Format of multicast_address on the wire:
     *
     *   47                                                   2   1   0
     *  +---+-----------------------------------------------+---+---+---+
     *  |   |                                               |   | L | M |
     *  +---+-----------------------------------------------+---+---+---+
     *		L = locally administered bit
     *		M = Multicast bit
     *
     * Format of multicast_address in 68020 memory:
     *
     *                                   L    M
     *	+----+----+----+----+----+----+----+----+
     *	| 07 | 06 | 05 | 04 | 03 | 02 | 01 | 00 |  byte 0
     *	+----+----+----+----+----+----+----+----+
     * 
     *	+----+----+----+----+----+----+----+----+
     *	| 15 | 14 | 13 | 12 | 11 | 10 | 09 | 08 |  byte 1
     *	+----+----+----+----+----+----+----+----+
     *			...
     *	+----+----+----+----+----+----+----+----+
     *	| 47 | 46 | 45 | 44 | 43 | 42 | 41 | 40 |  byte 5
     *	+----+----+----+----+----+----+----+----+
     */

    leveltype status;

    status = raise_interrupt_level(ETHER_DISABLE);
    reset_interrupt_level(status);
}

/*
 * biga_add_multicast_range
 * Add multicast address range to the logical address filter. 
 *
 * NOTE: Turns out that enabling just 256 IP multicast addresses
 * (0100.5e00.0000 - 0100.5e00.00ff) enables all filter bits. Therefore,
 * just enable all multicasts to support IP multicast routing.
 */
void
biga_add_multicast_range (idbtype *swidb, uchar *address, uchar *mask)
{

}

/*
 * Set up for bridging on this interface:
 */

void
biga_setup_bridging (hwidbtype *idb)
{
    biga_if_init(idb, FALSE);	/* Reset code will set it up */
    if (idb->counters.resets > 0)
	idb->counters.resets--;
}

/*
 * biga_input_error
 * Handle an input error
 */

void
biga_input_error (hwidbtype *idb)
{
    idb->counters.input_error++;
}

/*
 * biga_buffer_ring_adjust
 * Adjust buffer ring after MTU pool cache change
 */

void biga_buffer_ring_adjust (hwidbtype *idb)
{
    biga_if_init(idb, TRUE);
}

/*
 * lance_fastsend
 * Transmit this packet immediately or drop it
 * Called from interrupt level.
 */

void
biga_fastsend (hwidbtype *idb, paktype *pak)
{
    uchar * start;
    ushort bytes;
    ulong acct_proto;

    if (idb->state != IDBS_UP) {
        idb->counters.output_total_drops++;
        datagram_done(pak);
        return;
    }

    bytes = pak->datagramsize;
    acct_proto = (ulong)pak->acct_proto;
    
#ifdef DEBUG
    fprintf(CONTTY, "LANCE(%d): lance_fastsend: idb = 0x%x\n", idb->unit, idb);
    if (!ds) {
        fprintf(CONTTY, "LANCE(%d): ### ds is NULL!\n", idb->unit);
        return;
    }
#endif
     
#ifdef DEBUG
    if (bytes >= MAX_LANCE) {
	idb->outputdrops++;
	ds->tx_fsw_pk2big++;
	retbuffer(pak);
	errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, bytes);
	return;
    }
    if (bytes < MINETHERSIZE) {
	ds->tx_fsw_pk2sml++;
	pak->datagramsize = MINETHERSIZE;
	bytes = MINETHERSIZE;
    }
#endif


    /*
     * If no packets are queued for output on this interface,
     * immediately transmit the packet:
     */
    if (!idb->output_qcount) {

	/*
	 */ 
	start = (uchar *)pak->datagramstart;
	
	/*
	 * Is there room for another packet in the output queue?
	 */
	if (aalsap_nmp_send(0, bytes, start) != 0) {
	    /*
	     * Error, we don't own the buffer:
	     */
	    retbuffer(pak);
	    idb->counters.output_total_drops++;
	    return;
	}
		
	/*
	 * Transmit our packet:
	 */
	if (*start & 1)
	    idb->counters.output_broad++;	/* count broadcasts */
		
	/*
	 * Do packet accounting after starting the Lance:
	 */
	idb->counters.tx_cumbytes_fast[acct_proto] += bytes;
	idb->counters.outputs_fast[acct_proto]++;
	TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */

    } else {

	/*
	 * There are already packets on the output queue.
	 */
	pak->if_output = idb->firstsw;
	holdq_enqueue_fast(idb, pak);

        biga_tx_start(idb);
    }
}

/*
 * Start or restart output.
 */

void
biga_tx_start (hwidbtype *idb)
{
    uchar * start;
    paktype *pak;
    leveltype SR;
    ushort bytes;
    

    SR = raise_interrupt_level(ETHER_DISABLE);

    /*
     * While there is another packet to output :
     */
    while ((pak = (*idb->oqueue_dequeue)(idb))) {

	/*
	 * Is the packet too big to transmit?
	 */
	bytes = pak->datagramsize;

	if (bytes > MAX_BIGA) { /* MAX_BIGA */
	    idb->counters.output_total_drops++;
	    errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, bytes);
	    datagram_done(pak);
	    continue;
	}
	if (bytes < MINETHERSIZE) {
	    pak->datagramsize = MINETHERSIZE;
	    bytes = MINETHERSIZE;
	}
	/*
	 * Send packet:
	 */
	start = (uchar *)pak->datagramstart;

	if (aalsap_nmp_send(0, bytes, start) != 0) {
	    
	    pak->if_output = idb->firstsw;
/*
 * This message not defined anywhere:
 *
 *	    errmsg(&msgsym(OWNERR, BIGA), idb->unit, pak);
 */
	    pak_requeue_head(idb, pak);
	    reset_interrupt_level(SR);
	    return;
	}

	if (*start & 1)
	    idb->counters.output_broad++;

	/*
	 * Do packet accounting after starting the BIGA:
	 */
	idb->counters.tx_cumbytes += bytes;
	TIMER_START(idb->xmittimer, TIMEOUT); /* set timeout timer */
	idb->counters.outputs++;

	datagram_done(pak);
    }

    reset_interrupt_level(SR);
}
/*
 * biga_enable
 * Enable a BIGA interface, most likely shut down for input congestion.
 * Please note; this function must be called with NETS_DISABLED, to 
 * work correctly.
 */
void
biga_enable (hwidbtype *idb)
{
}


/*
 * biga_txqlength
 * Returns the number of active packets in the transmit queue.
 */

int
biga_txqlength (hwidbtype *idb)
{
    return (0);
}
/*
 * biga_periodic
 */
void
biga_periodic  (hwidbtype *idb)
{
}

/*
 * biga_show_controller 
 */
boolean biga_show_controller (hwidbtype *idb, parseinfo *csb)
{
    return(TRUE);
}

      
