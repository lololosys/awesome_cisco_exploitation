/* $Id: if_ags_c7000_trip.c,v 3.3.60.7 1996/08/28 12:44:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_ags_c7000_trip.c,v $
 *------------------------------------------------------------------
 * ags_rp1_trip.c -- driver routines cBus Token Ring Interface
 *  Processor for the AGS and RP1 (C/7000)
 *
 * August 1991 - Jack Jenney
 * 
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_ags_c7000_trip.c,v $
 * Revision 3.3.60.7  1996/08/28  12:44:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.60.6  1996/08/26  15:06:59  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.3.60.5  1996/08/13  02:17:26  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.3.60.4  1996/07/23  13:22:53  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.60.3  1996/07/19  19:13:14  ppearce
 * CSCdi62738:  rsrb over tcp connection fails on ags+ and 7k
 * Branch: California_branch
 *   Ensure pak->rxtype is initialized before reg_invoke_srt_srb_forward(
 *
 * Revision 3.3.60.2  1996/07/18  20:28:18  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.3.60.1  1996/03/18  19:41:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/14  00:56:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.26.1  1996/01/24  21:46:03  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  09:17:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/25  08:33:45  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.3  1995/08/07  05:33:57  richl
 * CSCdi38315:  makefile rework
 *              move code to tring_inline and to rif_util in libutil.a
 *
 * Revision 2.2  1995/07/20  17:55:30  psandova
 * CSCdi36539:  CTR token ring rql goes to 0, keeps initializing
 *
 * Reselect the input interface before flushing the packet.
 *
 * Revision 2.1  1995/06/07  20:40:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../srt/srt_registry.h"
#include "../srt/srb_core.h"
#include "interface_private.h"
#include "logger.h"
#include "../if/tring.h"
#include "../if/tring_private.h"
#include "if_hes_common.h"
#include "if_fci.h"
#include "if_cmd.h"
#include "if_trip.h"
#include "if_trip_inline.h"
#include "if_mci.h"
#include "../hes/if_ags_c7000_trip.h"
#include "../hes/dbus.h"
#include "../if/rif_util.h"
#include "msg_cbus.c"			/* Not a typo, see logger.h */



/**********************************************************************
 *
 *  Functions based on the inlines in if_trip.h
 *
 **********************************************************************/
void cbus_command_a (mciregtype *regptr, ushort command, ushort argument)
{
    cbus_command_a_inline(regptr, command, argument);
}

void cbus_command_la (mciregtype *regptr, ushort command, ulong argument)
{
    regptr->largreg = argument;
    regptr->cmdreg = command;
}

void ctr_memb_setup (hwidbtype *idb, ushort address)
{
    ctr_memb_setup_inline(idb, address);
}


/**********************************************************************
 *
 *		       Receive Support Routines
 *
 **********************************************************************/

/*
 * ctr_srb_frame
 *
 * Copy in a frame that is to be source routed by the system.  The FCS is
 * present in the Cbus buffer immediately after the frame, but is not
 * counted in the size passed up by the Cbus Controller card.  Remember
 * to copy it in and set the appropriate flag so that all SRB and RSRB
 * paths will work correctly.  The CTR output routine knows to ignore
 * these four bytes since the CTR currently cannot be told not to
 * regenerate the FCS on a frame.
 */
static char ctr_rif_string[SRB_MAXRIF*2+SRB_MAXRIF/2];

void ctr_srb_frame (
    hwidbtype *idb,
    mciregtype *regptr,
    ushort srbinfo,
    int dgsize)
{
    tring_hdr *trh = (tring_hdr *)NULL;
    int riflen = (srbinfo & FCI_RXSRB_LENGTH)*2; 
    paktype *bpak;
    uchar *llc1, buffer[TR_MAXENCSIZE];

    if (tokenring_debug) {
        trh = (tring_hdr *)buffer; /* Grab header to print debug info. */
        cbus_command_a_inline(regptr,MCI_CMD_RX_SELECT,CTR_RECEIVE_OFFSET);
        mci2buffer(&regptr->readlong, buffer, TR_MAXENCSIZE);

	llc1 = (uchar *)trh->data + (srbinfo & FCI_RXSRB_LENGTH)*2;
	if (tr_print_packet(idb," br:", trh, (ulong *)llc1, regptr->rxstatus,
			    dgsize))
	    buginf("\nTR%d:  br:      riflen %d, rd_offset %d, llc_offset %d",
		   idb->unit, (srbinfo & FCI_RXSRB_LENGTH)*2,
		   ((srbinfo & FCI_RXSRB_RDOFFSET) >> 8)*2,
		   (uchar *)llc1 - (uchar *)trh);
    }

    if (idb->tr_bridge_idb == NULL && idb->tr_vring_blk == NULL) {
	if (srb_debug || srbevent_debug) {
	    if(!trh) {  /* Grab the header to print debug info. */
                trh = (tring_hdr *)buffer;
                cbus_command_a_inline(regptr, MCI_CMD_RX_SELECT,
				      CTR_RECEIVE_OFFSET);
                mci2buffer(&regptr->readlong, buffer, TR_MAXENCSIZE);
	    }

    	    buginf("\nSRB%d: no path, s: %e d: %e rif: %s",
		   idb->unit, trh->saddr, trh->daddr,
		   ((trh->saddr[0] & TR_RII) ?
		    get_rif_str(trh->data, ctr_rif_string) : ""));
	}
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
        srbV.srb_sb_incr_rx_drops(idb->firstsw);
	return;
    }

        bpak = input_getbuffer(dgsize + TR_FCS_SIZE, idb);

    if (!bpak) {
	if (srbevent_debug) {
	    if(!trh) {  /* Grab the header to print debug info. */
                trh = (tring_hdr *)buffer;
                cbus_command_a_inline(regptr,MCI_CMD_RX_SELECT,
				      CTR_RECEIVE_OFFSET);
                mci2buffer(&regptr->readlong, buffer, TR_MAXENCSIZE);
	    }
	    buginf("\nSRB%d: no buffer, s: %e d: %e rif: %s",
		   idb->unit, trh->saddr, trh->daddr,
	((trh->saddr[0] & TR_RII) ?
		get_rif_str(trh->data, ctr_rif_string) : ""));
	}
        cbus_mci_idb_throttle(idb);					
        srbV.srb_sb_incr_rx_drops(idb->firstsw);
	return;
    }
	    
    bpak->datagramstart = bpak->network_start;
    bpak->datagramsize = dgsize;	/* don't count the FCS */
    bpak->enctype = ET_NULL;
    bpak->flags |= PAK_SRB_IN;
    bpak->rxtype = idb->rxtype;

    cbus_command_a_inline(regptr, MCI_CMD_RX_SELECT, CTR_RECEIVE_OFFSET);
    mci2buffer(&regptr->readlong,bpak->datagramstart,dgsize + TR_FCS_SIZE);

    reg_invoke_srt_srb_forward(bpak, idb, riflen);

    /* Reselect the input interface before flushing it's packet. */

    regptr->argreg = idb->mci_index;
    regptr->cmdreg = MCI_CMD_SELECT;
    regptr->cmdreg = MCI_CMD_RX_FLUSH;
}


/**********************************************************************
 *
 *		   Calls from the system level code
 *
 **********************************************************************/

/*****************************************************************************
 * Name: trip_restart_init
 *
 * Description: Post TRIP restart reinitialization
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface.
 *
 * Return Value: None
 *
 *****************************************************************************/
void trip_restart_init (hwidbtype *hwidb)
{
    /*
     * Re-initialize the specified interface.
     * Assign the interface to a buffer pool, set max datagramsize,
     * set receive offset and love note pool and buffer parking 
     */
    send_assign_poolindex_cmd(hwidb,hwidb->buffer_pool);
    send_rxbytes_cmd(hwidb,hwidb->max_buffer_size);
    send_rx_offset_cmd(hwidb,hwidb->rxoffset);
    send_rx_error_cmd(hwidb,FCI_RX_ERROR_POOL);
    send_cbus_buffer_park_cmd(hwidb, hwidb->cbus_burst_count);
}

/*****************************************************************************
 * Name: platform_trip_anybody_out_there
 *
 * Description: Determine if the TRIP is still alive by sending it
 *              a simple VERSION command.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface.
 *
 * Return Value: the status from the version command.
 *
 *****************************************************************************/
ushort platform_trip_anybody_out_there (hwidbtype *hwidb)
{
    mciregtype *regptr;

    regptr = hwidb->mci_regptr;
    cbus_command_a(regptr, FCI_CTRLR_ARGUMENT, FCI_SW_VERSION);
    cbus_command_a(regptr, FCI_CTRLR_CMD, FCI_CMD_VERSION);
    return(regptr->cmdreg);
}


/*****************************************************************************
 * Name: trip_platform_init
 *
 * Description: platform-specific TRIP init.  Since there are no
 *              special requirements on the ags/rp1, this is a stub.
 *
 * Input: 
 *   subsys -- pointer to the sybsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void trip_platform_init (subsystype *subsys)
{
}


/*
 * ctr_polling_failed
 * Print the debug info in ICBs of the TRIP, then reset the cbus
 * There are six words of debug info for each interface.
 * The 1st word is stored in the 6th word of the ICB of the interface.
 * The other five words are stored in the ICB (not used) which is indexed 
 * by adding four to the index of the interface.
 */
void ctr_polling_failed (hwidbtype *idb)
{
    mcitype *cbus = idb->devctl;
    mciregtype *regptr;
    int i, max_i, cbus_icb_address;
    leveltype level;
    ushort icb_data[6];
#ifdef C7000
    dgtype *dbus;
#endif

#ifdef C7000
    dbus = pslot_to_dbus(idb->cbus_ctrlr_index);
    if (!touch_device(dbus)) {
	if (tokenevent_debug)
	    buginf("\n%s: Ignoring polling failure, no dbus response",
		   idb->hw_namestring);
	return;
    }
#endif    

    if (cbus->flags & (CBUSII|CXBUS)) {
        i = (idb->mci_index / CBUS2_MAX_CTRLR_INTERFACES)*CBUS2_MAX_CTRLR_INTERFACES;
        max_i = i + 4;
        errmsg(&msgsym(POLLFAIL, CBUS), idb->hw_namestring);
        for ( ; i < max_i; i++) {
            level = raise_interrupt_level(NETS_DISABLE);
            regptr = idb->mci_regptr;
            cbus_icb_address = CBUS2_ICB_BASE + (i * CBUS2_ICB_SIZE) + 5;
            regptr->argreg = cbus_icb_address;
            regptr->cmdreg = FCI_CMD_MEMD_SELECT;
            icb_data[0] = regptr->readshort;
    
            cbus_icb_address = CBUS2_ICB_BASE + ((i+4) * CBUS2_ICB_SIZE) + 1;
            regptr->argreg = cbus_icb_address;
            regptr->cmdreg = FCI_CMD_MEMD_SELECT;
            icb_data[1] = regptr->readshort;
            icb_data[2] = regptr->readshort;
            icb_data[3] = regptr->readshort;
            icb_data[4] = regptr->readshort;
            icb_data[5] = regptr->readshort;
            reset_interrupt_level(level);

            errmsg(&msgsym(POLLFAILDAT, CBUS), i, icb_data[0], icb_data[1], 
		   icb_data[2], icb_data[3], icb_data[4], icb_data[5]);
        }
    } else
        buginf("\nTR%d: polling failed.  No debug information,", idb->unit);

    (void)cbus_init(cbus, cbus->critical);
    cbus_restarts_error++;
}
