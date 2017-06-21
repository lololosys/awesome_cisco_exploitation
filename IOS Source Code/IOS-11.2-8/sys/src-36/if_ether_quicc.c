/* $Id: if_ether_quicc.c,v 3.5.28.6 1996/08/28 13:10:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-36/if_ether_quicc.c,v $
 *------------------------------------------------------------------
 * if_ether_quicc.c  68360 QUICC chip ethernet driver for low end software
 *
 * November 1994, Ashwin Baindur
 * Based on "if_les_lance.c", December 1989,  Chris Shaker
 * Based on "if_lance.c", June 1988,  Bill Westfield
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_ether_quicc.c,v $
 * Revision 3.5.28.6  1996/08/28  13:10:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.28.5  1996/08/13  02:22:18  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.5.28.4  1996/07/20  03:20:10  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.5.28.3  1996/06/12  19:56:31  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.5.28.2  1996/04/17  13:47:22  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.5.28.1  1996/03/18  21:56:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/13  02:00:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.2  1996/03/07  10:42:53  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/01/24  22:49:03  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1996/01/20  00:25:04  jng
 * CSCdi42687:  C1003 QUICC vers 3 confused by MAC similar addresses
 * Individual address need not be added to the filter
 *
 * Revision 3.4  1995/11/20  22:27:50  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  18:39:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:26:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/08/10  18:58:11  gstovall
 * CSCdi38481:  Sapphire wont unthrottle
 * Convert a few IDBTYPE_LANCE to IDBTYPE_QUICC_ETHER that were missed.
 * Bad programmer, no pizza.
 *
 * Revision 2.7  1995/08/09  19:32:45  sgannu
 * CSCdi38309:  catwoman msg causes modular build failure
 *
 * Revision 2.6  1995/07/25  21:48:55  abaindur
 * CSCdi37628:  LOSTCARR msgs too frequent when c1000 ethernet cable
 * unplugged
 * Rate limit message. Also fix handling of line protocol state during
 * carrier loss, and counting of deferred transmits. Remove a bogus
 * check for idb->status in ethernet_enable.
 *
 * Revision 2.5  1995/06/28  09:30:34  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.4  1995/06/21  09:00:40  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.3  1995/06/21  03:14:43  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/20  07:18:09  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:51:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "logger.h"
#include "../src-36-c1000/msg_c1000_ether.c" /* Not a typo, see logger.h */

#include "fastswitch_registry.h"
#include "media_registry.h"
#include "subsys.h"

#include "../os/pool.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"

#include "../src-36/quicc.h"
#include "../src-36/quicc_lib.h"
#include "../les/if_les.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/ether_private.h"
#include "../if/ether_inline.h"
#include "../src-36/if_ether_quicc.h"
#include "../les/les_driver_inline.h"
#include "../les/if_les_ether.h"

#include "../ip/ip.h"
#include "../les/if_les_ether_inline.h"
#include "sys_registry.h"
#include "../if/traffic_shape.h"


#ifdef DEBUG

static inline ethernet_debug_foo1 (hwidbtype *idb, 
                                   ether_instance_t *ds,
                                   char *fn_name)
{
    ttyprintf(CONTTY, "QUICC(%d): %s: idb = 0x%x\n", 
	      idb->unit, fn_name, idb);
    if (!ds) {
        ttyprintf(CONTTY, "QUICC(%d): ### ds is NULL!\n", idb->unit);
        return;
    }

    if (!(idb->type == IDBTYPE_QUICC_ETHER)) {
	ttyprintf(CONTTY, "\n%s: Not an ethernet idb! 0x%x\n",
		  fn_name, idb);
	return;
    }
}

static inline void ethernet_debug_foo2 (hwidbtype *idb, 
                                        ether_instance_t *ds,
                                        char *fn_name)
{
    buginf("%s(%d)-%s: SCCE=%#x, SCCM=%#x, CICR=%#x, CIMR=%#x,"
           " CIPR=%#x, CISR=%#x\n",
           idb->hw_namestring, idb->unit, fn_name,
           *ds->scce, *ds->sccm, *ds->cicr, *ds->cimr,
           *ds->cipr, *ds->cisr);
}
#endif
/*---------------------------------------------------------------------*/
/*
 * Stop the quicc ethernet SCC. 
 * Must be called with network interfaces disabled.
 */
static void 
quicc_ethernet_stop (hwidbtype *idb)
{
    ether_instance_t *ds = INSTANCE;

    /*
     * Bring the ethernet scc to a screeeeching halt.
     * Both Rx and Tx must stop, interrupts disabled, etc
     */
    quicc_scc_stop(ds->dprbase, ds->qinfo, RX_DISABLED, TX_DISABLED);
}

/*
 * Throttle the controller to give process level code a chance to run
 */
static void 
quicc_ethernet_throttle (hwidbtype *idb)
{
    ether_instance_t *ds = INSTANCE;

    idb_mark_throttled(idb);

    /*
     * Disable the receiver
     */
    quicc_scc_disable_rx(ds->dprbase, ds->qinfo);
}

/*
 * Enable ethernet interface, most likely shut down for input congestion.
 * Please note; this function must be called with NETS_DISABLED, to
 * work correctly.
 */
static void 
quicc_ethernet_enable (hwidbtype *idb)
{
    ether_instance_t *ds = INSTANCE;

    quicc_scc_enable_rx(ds->dprbase, ds->qinfo);
}

/* 
 * Start the transmitter. If interface is not
 * throttled, start the receiver too.
 */
static void 
quicc_ethernet_start (hwidbtype *idb)
{
    ether_instance_t *ds;
    
    ds = INSTANCE;

    /*
     * Kick the CP with an INIT_RX_TX_PARAMS command.
     * Enable ethernet interrupts.
     * Set ENT and ENR in GSMR.
     */
    if (quicc_scc_start(ds->dprbase, ds->qinfo, 
                        idb_is_throttled(idb) ? 0 : RX_ENABLED,
                        TX_ENABLED)) {
        idb->ethernet_lineup = TRUE;
	net_cstate(idb, IDBS_UP);
	if (ether_debug)
	    buginf("\n%s: Interface is alive, phys. addr %e",
		   idb->hw_namestring, idb->hardware);
	return;
    }
    quicc_ethernet_stop(idb);
    idb->ethernet_lineup = FALSE;
    errmsg(&msgsym(INITFAIL, QUICC_ETHER), idb->unit, 0, ds->scce);
    net_cstate(idb, IDBS_DOWN);
}

/*
 * Check if SCC should be running; if so, ensure that it is
 * running and has a normal value. Otherwise, reset it.
 */
static void quicc_ethernet_health_check (hwidbtype *idb)
{
    ether_instance_t *ds;
    leveltype SR;
    boolean sts_ok;
    
    ds = INSTANCE;
    if (ds) {
	SR = raise_interrupt_level(ETHER_DISABLE);
        sts_ok = quicc_scc_status(ds->dprbase, ds->port_no,
				  idb_is_throttled(idb) ? 0 : RX_ENABLED,
                                  TX_ENABLED);
	reset_interrupt_level(SR);

        if (!sts_ok) {
            if (ether_debug)
                buginf("\nQUICC(%d): Rcvr and/or Txmtr in bad state\n",
                       idb->unit);
	    if (idb->reset)
                (*idb->reset)(idb);
	}
    }
}

static void 
quicc_ethernet_mode (hwidbtype *idb, int mode)
{
    ether_instance_t *ds = INSTANCE;

    switch (mode) {
    case MODE_PROM:
        quicc_scc_write_psmr(ds->dprbase, ds->port_no, 
                             QUICC_ETHERNET_MODE_PRO);
        break;

    case MODE_INDIV_FILTER:
        quicc_scc_write_psmr(ds->dprbase, ds->port_no, 
                             QUICC_ETHERNET_MODE_IAM);
        break;

      case MODE_REJECT_BROADCAST:
        quicc_scc_write_psmr(ds->dprbase, ds->port_no, 
                             QUICC_ETHERNET_MODE_BRO);
        break;

    default:  /* do nothing */
        break; 
    }
}

/*
 * Add another ethernet hardware address to the logical address filter
 */
static void 
quicc_ethernet_add_listen_address (hwidbtype *idb, uchar *address)
{
    /*
     * Format of address on the wire:
     *
     *   47                                                   2   1   0
     *  +---+-----------------------------------------------+---+---+---+
     *  |   |                                               |   | L | M |
     *  +---+-----------------------------------------------+---+---+---+
     *		L = locally or globally administered bit
     *		M = Multicast or unicast bit
     *
     * Format of address in 68020 memory:
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
     *
     * For the QUICC, the address in memory needs to be
     * converted to the network byte order before asking
     * the QUICC to convert it to an address filter bit.
     */

    ether_instance_t *ds;
    leveltype status;

    status = raise_interrupt_level(ETHER_DISABLE);
    ds = INSTANCE;

    quicc_scc_set_listen_addr(ds->dprbase, address);

    reset_interrupt_level(status);
}

static void 
quicc_ethernet_add_listen_range (hwidbtype *idb, uchar *address, uchar *mask)
{
    ether_instance_t *ds;
    quicc_ether_param_t *ether;
    leveltype status;

    status = raise_interrupt_level(ETHER_DISABLE);
    ds = INSTANCE;

    /*
     * First make one call with the first group address
     * so as to issue the SET_GROUP_ADDRESS command to the
     * CP, then go and set all bits in the filter.
     * This function is called for multicast routing.
     * The range of multicast addresses cause all bits in 
     * the filter to be set.
     */

    quicc_ethernet_add_listen_address(idb, address);

    /*
     * Now set all bits in the group address filter.
     */
    ether = &ds->dprbase->scc1.ether;
    ether->gaddr1 = 0xFFFF;
    ether->gaddr2 = 0xFFFF;
    ether->gaddr3 = 0xFFFF;
    ether->gaddr4 = 0xFFFF;

    reset_interrupt_level(status);
}

/*
 * Check the given output ring entry for transmit errors.
 * Take the correct action for each error:
 */
static void quicc_ethernet_check_tx_errors (
    register hwidbtype *idb,
    ushort status)
{
    ether_instance_t		*ds;
    ushort			retry_count;
    ethersb			*esb;

    ds = INSTANCE;
    esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);

    if (status & QUICC_ETHERNET_TX_ERR) {    
	idb->counters.output_error++;

        if (status & QUICC_BDSTAT_TX_RLIM) {
            /*
             * Retry limit exceeded error. FATAL. TXE set in SCCE.
             */
       	    esb->output_excessive_collision++;
       	    ds->fatal_tx_err = TRUE;
       	    errmsg(&msgsym(COLL, QUICC_ETHER), idb->unit, 
                   ((quicc_ether_param_t *)ds->parmbase)->ret_lim);
        }
        if (status & QUICC_BDSTAT_TX_UNRRUN) {
            /*
             * Underrun error. FATAL. TXE set in SCCE.
             */
       	    idb->counters.output_underrun++;
       	    ds->fatal_tx_err = TRUE;
       	    errmsg(&msgsym(UNDERFLO, QUICC_ETHER), idb->unit);
        }
        if (status & QUICC_BDSTAT_TX_LCOL) {
            /*
             * Late collision error. FATAL. TXE set in SCCE.
             */
       	    esb->output_late_collision++;
       	    ds->fatal_tx_err = TRUE;
       	    errmsg(&msgsym(LATECOLL, QUICC_ETHER), idb->unit);
        }
        if (status & QUICC_BDSTAT_TX_HBEAT) {
            /*
             * Heartbeat check failure. TXE bit set in SCCE.
             */
            ds->tx_hbeat_loss++;
            errmsg(&msgsym(HBEAT, QUICC_ETHER), idb->unit);
        }
        if (status & QUICC_BDSTAT_TX_CSLOS) {
            /*
             * Carrier sense loss error. Set only if collision NOT
             * detected. Buffer transmitted normally.
             */
       	    esb->output_lost_carrier++;
       	    if (!idb->nokeepalive) {  /* Keepalives active */
       	        if (idb->ethernet_lineup == TRUE) {
       	            idb->ethernet_lineup = FALSE; /* Lost carrier, not up */
       	            idb->reason = "Transmit failed - carrier loss";
       	            reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
       	        }    
    	    }    
   	    errmsg(&msgsym(LOSTCARR, QUICC_ETHER), idb->unit);
        } 
    }  /* end if-error */

    /*
     * No TX error. Is our line status DOWN ?
     */
    else {
        if (ds->line_up_fn)
            (*ds->line_up_fn)(idb);
    }

    if (status & QUICC_BDSTAT_TX_DEFIND) {
        /*
         * Count number of deferred transmits.
         */
        esb->output_deferred++;
    }

    retry_count = QUICC_BDSTAT_TX_RCNT(status);
    if (retry_count == 1)
        esb->output_one_collision++;
    else
        esb->output_more_collision += retry_count;
    
    idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
} 

/*
 ******************* Interrupt routines ****************************
 */
static void 
quicc_ethernet_check_rx_errors (hwidbtype *idb, ushort bd_status)
{

    /*
     * We encountered an error while receiving this frame.
     */
    if (bd_status & QUICC_ETHERNET_RX_ERR) {
        idb->counters.input_error++;

        if (bd_status & QUICC_BDSTAT_RX_FLV) {
            idb->counters.giants++; /* 2 */
        }
        if (bd_status & QUICC_BDSTAT_RX_NOFA) {
            idb->counters.input_frame++;
        }
        if (bd_status & QUICC_BDSTAT_RX_SHORT) {
            idb->counters.runts++;
        }
        if (bd_status & QUICC_BDSTAT_RX_CRCERR) {
            idb->counters.input_crc++;
        }
        if (bd_status & QUICC_BDSTAT_RX_OVRRUN) {
            ether_instance_t *ds = INSTANCE;

       	    idb->counters.input_overrun++;
            ds->overflow_err++;
        }
        if (bd_status & QUICC_BDSTAT_RX_COLLIS) {
    	    idb->counters.input_abort++;
        }
    }
}

/*
 * quicc_ethernet_fastsend
 * Transmit this packet immediately or drop it
 * Called from interrupt level.
 */
static void quicc_ethernet_fastsend (hwidbtype *idb, paktype *pak)
{
    TXBD_T *tmd;
    ether_instance_t *ds;
    uchar *start;
    ushort bytes;
    ulong acct_proto;
    paktype **txp;
    traffic_shape_t *shape;

    if (idb->state != IDBS_UP) {
        idb->counters.output_total_drops++;
        datagram_done(pak);
        return;
    }

    acct_proto = (ulong)pak->acct_proto;
    ds = INSTANCE;
    
    bytes = pak->datagramsize;
    if (bytes < ds->min_frame_len) {
	bytes = ds->min_frame_len;
	pak->datagramsize = bytes;
    }

    /*
     * determine whether there is window available to send the message with
     * in the rate control bucket
     */
    shape = traffic_shape_required_inline(pak->if_output->traffic_shape, pak);
    if (shape) {
	traffic_shape_defer_sending(shape, pak);
	return;
    }

    /*
     * If no packets are queued for output on this interface,
     * try to immediately transmit the packet:
     */
    if (!idb->output_qcount) {

	/*
	 * Get pointer to ring entry and packet pointer at tx_head.
	 */ 
        txp = ds->head_txp;
	tmd = ds->tx_head;
	start = pak->datagramstart;
	
	/*
	 * Is there room for another packet in the output queue?
	 */
	if (ds->tx_count < ds->tx_size) {  

            /* Room in TX ring */

	    if ((tmd->status & QUICC_BDSTAT_TX_RDY) == 0) {
		
		/*
		 * Transmit our packet:
		 */
		if (*start & 1)
		    idb->counters.output_broad++; /* count broadcasts */
		
		/*
		 * Give the quicc ownership of the buffer now to reduce latency:
		 */
                tmd->buf_ptr = start;
		tmd->length = bytes;
		tmd->status = QUICC_BDSTAT_TX_DEFAULT | 
                              (tmd->status & QUICC_BDSTAT_TX_WRAP);

		/*
		 * Force the quicc to poll the TX ring and see this packet:
		 */
                if (ds->tx_count == 0)
		    *ds->tx_todr = QUICC_TX_KICK;
		ds->tx_count++;
		
		/*
		 * Update the head pointer for the next packet we want to TX:
		 */
                *txp++ = pak;
		if (++ds->tx_head > ds->tx_max) { /* End of ring? */
		    ds->tx_head = ds->tx_ra;
                    txp = &ds->tx_p[0];
		}
                ds->head_txp = txp;

		/*
		 * Do packet accounting after starting the quicc:
		 */
		idb->counters.tx_cumbytes_fast[acct_proto] += bytes;
		idb->counters.outputs_fast[acct_proto]++;
		TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */

	    } else {  /* Uh oh. Problem. We don't own the buffer. */
		
		retbuffer(pak);
		idb->counters.output_total_drops++;
	    }

	    return;

	} else {  /* TX ring full */
	    
	    /*
	     * We have a full TX ring. Is the first packet in the queue done?
	     * If so, we can service the TX interrupt on this packet and use
	     * the ring entry for our new packet:
	     */
	    if ((tmd->status & QUICC_BDSTAT_TX_RDY) == 0) {
		/*
                 * This packet has been transmitted. Does the BD
		 * need cleaning up ?
		 */
		if (*txp) {

		    /*
                     * BD needs cleaning up.
		     * Check for TX  errors, then release packet buffer.
		     */
	 	    (*ds->TX_errors)(idb, tmd->status);

		    /*
		     * Do TX packet accounting and clean up:
		     */
		    datagram_done(*txp); /* Release output packet*/
		}

		/*
		 * Now, we can transmit our new packet:
		 */
		if (*start & 1)
		    idb->counters.output_broad++;	/* count broadcasts */
		
		/*
		 * Give the quicc ownership of the buffer.
		 */
                tmd->buf_ptr = start;
		tmd->length = bytes;
		tmd->status = QUICC_BDSTAT_TX_DEFAULT | 
                              (tmd->status & QUICC_BDSTAT_TX_WRAP);

		/*
		 * Update the head pointer for the next packet we want to TX:
		 */
                *txp++ = pak;
		if (++ds->tx_head > ds->tx_max) {
		    ds->tx_head = ds->tx_ra;
                    txp = &ds->tx_p[0];
		}
                ds->head_txp = txp;
		
		/*
		 * Have the next TX interrupt bypass this packet
		 * since we just took care of it:
		 */
                ds->tail_txp++;
		if (++ds->tx_tail > ds->tx_max) {
		    ds->tx_tail = ds->tx_ra;
                    ds->tail_txp = &ds->tx_p[0];
		}

		/*
		 * Do packet accounting after starting the quicc:
		 */

		/* Don't change ds->tx_count, it should stay the same */
		idb->counters.tx_cumbytes_fast[acct_proto] += bytes;
		idb->counters.outputs_fast[acct_proto]++;
		GET_TIMESTAMP(idb->lastoutput);
		TIMER_START(idb->xmittimer, TIMEOUT); /* set timeout timer */
		
	    } else {  /* BD still in use by QUICC */
		
		/*
		 * We have a full TX queue, so queue the packet in the output
		 * queue. Note that holq_enqueue requires the if_output and
		 * linktype fields to be initialized:
		 */
		holdq_enqueue_fast(idb, pak);
		(*idb->soutput)(idb);
	    }

	    return;
	}

    } else {  /* Output hold queue not empty */

	/*
	 * There are already packets on the output queue,
	 * so add this one to the end of the queue to minimize
	 * out of order situations:
	 */
	holdq_enqueue_fast(idb, pak);

	/*
         * If the software thinks the TX ring is full, go check with
         * the controller (and in particular, the ownership bits, and see
         * if we can clear some space before pulling frames off of
         * the holdq by faking a TX interrupt. The TX interrupt
         * routine will call (*idb->soutput)() for us.
         *
         * This causes a smoother saturation roll-off
         */
        if (ds->tx_count == ds->tx_size)
            quicc_scc_tx_intr(idb);
        else
            (*idb->soutput)(idb);
    }
}

/*
 ******************* Display routines *****************************
 */
/*
 * Show quicc ethernet errors:
 */
static void 
quicc_ethernet_show_errors (hwidbtype *idb)
{
    ether_instance_t *ds = INSTANCE;

    printf("QUICC SCC specific errors:\n");
    printf("%d buffer errors, %d overflow errors\n",
	   ds->buffer_err, ds->overflow_err);
    printf("%d input aborts on late collisions\n", idb->counters.input_abort);

    printf("%d heartbeat failures, %d cumulative deferred \n",
           ds->tx_hbeat_loss, ds->tx_def_count);
    printf("%d throttles, %d enables\n", idb->counters.throttle_count,
	   ds->enabled);

}

/*
 * Show quicc ethernet status. Return true if we quit the display.
 */
static boolean 
quicc_ethernet_show_controller (hwidbtype *idb, parseinfo *csb)
{
    ether_instance_t *ds = INSTANCE;
    uchar eth_addr[IEEEBYTES];
    
#ifdef DEBUG
    ethernet_debug_foo1(idb, ds, "ethernet_show_controller");
#endif
    
    printf("%s unit %d ", idb->typestring, idb->unit);
    
    if (ds) { 
        printf("using SCC%d, Microcode ver %d\n", 
	       ds->port_no+1, ds->dprbase->misc.rev_num);
	
        /*
         * Compute the current station address and default station address:
         */
        reg_invoke_get_default_hw_address(idb, eth_addr);
        printf("Current station address %e, default address %e\n",
               idb->hardware, eth_addr);
	
        /*
         * Display scc general registers
         */
	quicc_scc_show_regs(idb, TRUE);

        /*
         * Display SCC generic stuff and
         * ethernet SCC specific stuff.
         */
        quicc_scc_show_ether_param(ds->parmbase);

        /*
         * Allocate memory for the RX and TX rings.
         * Disable network interrupts and take a snapshot of the 
         * RX and TX ring states.
         * Enable network interrupts.
         * Display the Rx and Tx rings.
         */
        if (!show_bdrings(idb, sizeof(RXBD_T))) {
            return(TRUE);
        }
	
        /*
         * Display generic error counts maintained in the idb.
         */
        les_ethernet_show_errors(idb);

        /*
         * Display controller specific errors.
         */
        quicc_ethernet_show_errors(idb);

	printf("\n\n");

    } else
        printf(not_initialized);

    return(FALSE);
}

static void 
quicc_ethernet_init (hwidbtype *idb, boolean maxdgram_change)
{
    ether_instance_t *ds;

    ds = INSTANCE;

    /*
     * Initialize the quicc SCC
     * - initialize the RX and TX BD rings
     * - program the GSMR (except ENT and ENR bits)
     * - program the PSMR
     * - program the DSR
     * - initialize common parameter RAM
     * - initialize ethernet parameter RAM
     * - clear SCCE events
     * - initialize CICR, CIPR, CIMR
     */
    /*
     * Clean up RX and TX BD rings
     */
    quicc_scc_bd_ring_init(idb, maxdgram_change);

    /*
     * Initialize GSMR, PSMR, DSR
     */
    quicc_scc_reg_init(ds->dprbase, ds->port_no);

    /*
     * Initialize SCC general parameter RAM
     * Initialize SCC ethernet parameter RAM, SCCE, CICR, CIPR
     */
    quicc_scc_ethernet_param_init(ds->dprbase, ds->port_no,
                                  idb->hardware);
}

/*
 * Quicc or controller related IDB initialization routine:
 */
static void quicc_ethernet_idb_init (hwidbtype *idb)
{

    /*
     * To minimize changes to other non-sapphire code, continue
     * using IDB_LANCE to represent the QUICC based ethernet on
     * sapphire. This should ultimately be cleaned up.
     */
    idb->type = IDBTYPE_QUICC_ETHER;
    idb->typestring = "QUICC Ethernet";
    
    /*
     * Set up function calls
     */
    idb->soutput = quicc_tx_start_output;
    idb->fastsend = quicc_ethernet_fastsend;
    idb->show_controller = quicc_ethernet_show_controller;
}

/*
 * Set up driver data structure.
     * Malloc driver data structure and initialize it.
     * - initialize port number for SCC
     * - get RX and TX BD ring base addresses
     * - initialize RX and TX pak pointer arrays
     * - initialize register values, error counts, pointers, etc
     *
     * Set up the ethernet SCC in the quicc.
     * - configure the parallel i-o port pins
     *   - PAPAR, PADIR, PAODR
     *   - PCPAR, PCDIR, PCSO 
     *   - program SICR
     * - initialize the registers
     *   - GSMR (except ENT and ENR bits)
     *   - PSMR, DSR, CICR, CIPR, CIMR
     *   - clear SCCE events
     * - initialize common parameter RAM
     * - initialize ethernet parameter RAM
 */
static void 
quicc_ethernet_driver_setup (hwidbtype *idb, long membase, long port)
{
    ether_instance_t *ds;
    void             *ds_base;
    int rx_ring_size, tx_ring_size;

    /*
     * QUICC specific idb init.
     */
    quicc_ethernet_idb_init(idb);

    /*
     * Allocate the driver data structure:
     */
    ds_base = malloc_fast(QUICC_ETHER_INSTANCE_SIZE);
    if (!ds_base) {
	crashdump(0);
    } else {
	ds = (ether_instance_t *)ds_base;
	INSTANCE = ds;
	
	ds->qinfo = malloc_fast(sizeof(quicc_info_t));
	if (!ds->qinfo) 
	    crashdump(0);
	
        ds->qinfo->port_no = ds->port_no = port;
	
        ds->dprbase          = (quicc_dpr_t *)membase;
        ds->parmbase         = (void *)&ds->dprbase->scc1.ether;

	/*
	 * Find the ring sizes
	 */
	select_ring_size(&rx_ring_size, &tx_ring_size, idb->max_buffer_size);
	
        ds->max_rx_ring_size = min(QUICC_RXBD_NUM_SCC1, rx_ring_size);
        ds->max_tx_ring_size = min(QUICC_TXBD_NUM_SCC1, tx_ring_size);

        idb->start_offset    = (ENCAPBYTES - ETHER_ARPA_ENCAPBYTES) & ~0x3;
        ds->rx_bd_mask       = QUICC_ETHERNET_RX_MASK;
        ds->rx_bd_good_mask  = QUICC_BDSTAT_RX_PKT;
        ds->tx_bd_mask       = QUICC_ETHERNET_TX_MASK;
        ds->tx_bd_good_mask  = QUICC_BDSTAT_TX_PKT;
	
	/*
	 * Add scce and sccm mask.  These mask will be use to enable
	 * and disable interrupt as well as clear events within the
	 * routines used to enable and disable ports.
	 */
	ds->qinfo->rx_enable_sccm_mask   = QUICC_RX_ETHER_ENABLE_SCCM_MASK;
	ds->qinfo->tx_enable_sccm_mask   = QUICC_TX_ETHER_ENABLE_SCCM_MASK;
	
	ds->qinfo->rx_clear_scce_event   = QUICC_RX_ETHER_CLEAR_SCCE_EVENT;
	ds->qinfo->tx_clear_scce_event   = QUICC_TX_ETHER_CLEAR_SCCE_EVENT;

	ds->qinfo->rx_disable_sccm_mask  = QUICC_RX_ETHER_DISABLE_SCCM_MASK;
	ds->qinfo->tx_disable_sccm_mask  = QUICC_TX_ETHER_DISABLE_SCCM_MASK;
	
	
        ds->crc_length       = QUICC_ETHER_CRC_SIZE;
        ds->min_frame_len    = MINETHERSIZE;
        ds->max_frame_len    = QUICC_ETHER_MAX_FRAME;

        ds->hw_stop          = quicc_ethernet_stop;
        ds->hw_start         = quicc_ethernet_start;
        ds->hw_init          = quicc_ethernet_init;
        ds->hw_health        = quicc_ethernet_health_check;
        ds->hw_mode          = quicc_ethernet_mode;
        ds->hw_throttle      = quicc_ethernet_throttle;
        ds->hw_enable        = quicc_ethernet_enable;
	ds->line_up_fn       = les_ethernet_line_up;
	ds->line_down_fn     = c1000_ethernet_line_down;
        ds->TX_errors        = quicc_ethernet_check_tx_errors;
        ds->RX_errors        = quicc_ethernet_check_rx_errors;
        ds->rx_handler       = les_ethernet_RX_interrupt;
        ds->tx_handler       = NULL;
        ds->misc_handler     = NULL;

        /*
         * Initialize TX error statistics 
         */
        ds->tx_hbeat_loss   = 0;
        ds->tx_def_count    = 0;

        /*
         * Set up driver stuff common to SCCs.
         */
        quicc_scc_driver_setup(idb);

        /*
         * Set up i-o port pins
         */
        quicc_set_ethernet_pins(ds->dprbase);

        /*
         * Install RX, TX, and error interrupt handlers
         */
        quicc_scc_handler_install(idb, ds->port_no);

        /*
         * Initialize GSMR, PSMR, DSR, SCCE, SCCM, CICR, CIPR
         */
        quicc_scc_reg_init(ds->dprbase, ds->port_no);

        /*
         * Initialize SCC general parameter RAM
         * Initialize SCC ethernet parameter RAM, SCCE, CICR, CIPR
         */
        quicc_scc_ethernet_param_init(ds->dprbase, ds->port_no, 
                                      idb->hardware);
    }
}

static void quicc_ethernet_subsys_init (subsystype *subsys)
{
    reg_add_ethernet_driver_setup(QUICC_DRIVER, 
                                  quicc_ethernet_driver_setup,
                                  "ethernet_driver_setup");
    reg_add_ethernet_hw_listen(quicc_ethernet_add_listen_address,
                               "ethernet_hw_listen");
    reg_add_ethernet_hw_listen_range(quicc_ethernet_add_listen_range,
                               "ethernet_hw_listen_range");
}
/*
 * QUICC ethernet subsystem header
 */
#define QUICC_ETHERNET_MAJVERSION   1
#define QUICC_ETHERNET_MINVERSION   0
#define QUICC_ETHERNET_EDITVERSION  1

SUBSYS_HEADER(quicc_ethernet, QUICC_ETHERNET_MAJVERSION, 
              QUICC_ETHERNET_MINVERSION, QUICC_ETHERNET_EDITVERSION,
	      quicc_ethernet_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);

      
