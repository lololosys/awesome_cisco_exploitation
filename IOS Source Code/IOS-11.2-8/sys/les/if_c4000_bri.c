/* $Id: if_c4000_bri.c,v 3.8.12.13 1996/08/28 12:57:41 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_bri.c,v $
 *------------------------------------------------------------------
 * if_c4000_bri.c -- The BRI driver code for the cisco 4xxx.
 *
 * 10 December 1993, Michael Hubenthal and Manoj Leelanivas
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_bri.c,v $
 * Revision 3.8.12.13  1996/08/28  12:57:41  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.12.12  1996/08/13  02:19:56  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.8.12.11  1996/07/23  13:25:47  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.8.12.10  1996/06/18  01:48:15  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.8.12.9  1996/06/17  17:33:17  wmay
 * CSCdi60601:  show controller bri crashes box - only allow for d-chans
 * Branch: California_branch
 *
 * Revision 3.8.12.8  1996/06/16  21:15:06  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.8.12.7  1996/06/12  16:50:44  wmay
 * CSCdi39956:  BRI: show int stat and show int accounting only show D
 * channel info - put b channel idbs in the idb list.  This removes a
 * lot of special initialization code for BRI.  Also, make the names
 * of the b-channels more consisted with PRI (ie: BRI0:1 and BRI0:2).
 * Branch: California_branch
 *
 * Revision 3.8.12.6  1996/05/29  00:00:42  asastry
 * CSCdi56473:  Fix perfomance degradation with custom queueing on 4500
 * MBRI.
 * Branch: California_branch
 *
 * Revision 3.8.12.5  1996/05/23  23:46:36  hrobison
 * CSCdi50628:  BRI interface stops sending frames on B or D channels on
 * 4x00
 * Branch: California_branch
 * Under unknown circumstances, a 4x00 BRI can stop transmitting.  Appears
 * to be related to CSCdi37200, so choose the lesser of two evils, either
 * have it hang, or have it require a ton of memory just to change the
 * speed of the interface.  Back out CSCdi37200 until we can figure out
 * what is happening.
 *
 * Revision 3.8.12.4  1996/05/17  11:26:16  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.2.3  1996/05/05  23:31:29  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.8.2.2  1996/04/26  07:56:17  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.8.2.1  1996/04/03  19:58:11  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.8.12.3  1996/05/10  01:34:16  wmay
 * CPP and ppp half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.8.12.2  1996/04/22  21:51:43  gtaylor
 * CSCdi54554:  Bridging over isdn leased-line broken for HDLC
 * encapsulation.
 * Branch: California_branch
 *
 * Revision 3.8.12.1  1996/03/18  20:41:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.5  1996/03/16  07:11:55  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.4  1996/03/13  01:49:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.3  1996/03/07  10:05:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.2  1996/02/20  01:08:23  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/15  18:02:22  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.7  1996/02/08  20:12:25  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.6.2.1  1996/01/24  22:23:04  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1996/01/19  23:09:45  hrobison
 * CSCdi37200:  MBRI requires significant memory to set b channel speed
 * The MBRI driver no longer frees and reallocates ring buffers and
 * descriptors on every dialer call that changes bandwidth.  Set the
 * timeslot bitrate in the M32 on every call.
 *
 * Fix problem (software workaround for probable HW bug) with the M32
 * chip where the device RX descriptor pointer either does not increment
 * or jumps to the start of the ring with an RX interrupt
 *
 * Revision 3.5  1996/01/19  22:15:40  wmay
 * CSCdi45344:  ACL: input access lists are ignored on SMDS subinterfaces
 * Add extra include file so serial_inline is happy
 *
 * Revision 3.4  1996/01/05  01:02:07  sluong
 * CSCdi42391:  Configuring a protocol to be routed brings down B-channel
 *
 * Revision 3.3  1995/11/17  17:38:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:58  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:14:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/08/25  11:44:31  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.7  1995/07/18 06:47:19  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.6  1995/07/12 22:31:25  mleelani
 * CSCdi36990:  Occassional corruption of interface state for BRI
 * Interface state gets illegal value.
 *
 * Revision 2.5  1995/06/21  03:01:03  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.4  1995/06/19  06:47:10  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/16  19:33:10  mleelani
 * CSCdi35977:  Test interface does not work for BRI
 * Make sure we process the echo packets, as we receive them.
 *
 * Revision 2.2  1995/06/09  13:11:21  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:33:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "subsys.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "packet.h"
#include "../ui/debug.h"
#include "../wan/serial.h"
#include "../wan/ppp.h"
#include "../if/ether.h"
#include "../wan/smds.h"
#include "ttysrv.h"
#include "logger.h"
#include "../wan/isdn_debug.h"
#include "if_les.h"
#include "isdn_bri.h"
#include "if_c4000_mcn.h"
#include "if_c4000_m32.h"
#include "if_les_bri.h"
#include "if_c4000_mb.h"
#include "if_c4000_bri.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../wan/atm_dxi.h"
#include "media_registry.h"
#include "../if/serial_inline.h"
#include "../ui/command.h"
#include "../wan/serial_parser.h"
#include "../if/priority_private.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "msg_bri.c"			/* Not a typo, see logger.h */
#include "../ui/common_strings.h"


/*
 * global defines
 */
static queuetype test_bri_loopbackQ; /* Queue for to store packets which were
					looped back on the BRI under test */
static queuetype test_bri_echoQ; /* ICMP echo queue used when testing bri
				    interface */


/*
 * =====================================================================
 * test_bri_init() 
 *
 * Description:
 * Initialize the loopback queue and ICMP Echo queue used for testing a
 * bri interface. Also reset the loopback flag in isdn_bri structure.
 *
 * Parameters: 
 * dchan_idb  - the pointer to d channel idb 
 *
 * Returns:   
 * nothing
 *
 * Notes: called by bri_init
 * =====================================================================
 */
static void test_bri_init (hwidbtype *dchan_idb)
{
    queue_init(&test_bri_loopbackQ, 2);
    queue_init(&test_bri_echoQ, 2);
    dchan_idb->isdn_info->loopback_mode = LOOPBACK_DISABLED;
    dchan_idb->isdn_info->b1_chan_idb->isdn_info->loopback_mode =
	LOOPBACK_DISABLED;
    dchan_idb->isdn_info->b2_chan_idb->isdn_info->loopback_mode =
	LOOPBACK_DISABLED;
}
     
/*
 * =====================================================================
 * bri_reset()
 *
 * Description:
 * Deactivate the tp3420 chip for an interface.
 *
 * Parameters:
 * dchan_idb - the idb for the bri D channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_reset (hwidbtype *idb) 
{
    hwidbtype *dchan_idb;

    dchan_idb = idb->isdn_info->d_chan_idb;	/* make sure it is dchannel*/

    /*
     * put the chip in a known state and inform the process
     * level code that things are closing down
     */
    mb_tp3420_deactivate_line(dchan_idb); 
}

/*
 * =====================================================================
 * bri_disable()
 *
 * Description:
 * Disable the tp3420 chip for an interface.
 *
 * Parameters:
 * dchan_idb - the idb for the bri D channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_disable (hwidbtype *idb) 
{
    hwidbtype *dchan_idb;

    dchan_idb = idb->isdn_info->d_chan_idb;	/* make sure it is dchannel*/

    /*
     * put the chip in a known state and inform the process
     * level code that things are closing down
     */
    mb_tp3420_deactivate_line(dchan_idb); 
    /*
     * shut down the chip, requires an enable to start 
     * it up again
     */
    mb_tp3420_disable_line(dchan_idb); 
}


/*
 * =====================================================================
 * bri_drop_pak()
 *
 * Description:
 * Clear flags and up counters for a dropped packet.
 *
 * Parameters:
 * idb - the idb for the bri channel 
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_drop_pak (hwidbtype *idb, paktype *pak)
{
    clear_if_input(pak);
    idb->counters.input_drops++;
    MAYBE_DROPPED(idb);
    datagram_done(pak);
}


/*
 * =====================================================================
 * bri_m32_map_channel()
 *
 * Description:
 * Map a D or B channel of a BRI interface to a munich32 channel.
 *
 * Parameters:
 * idb - the idb for the bri channel to map
 *
 * Returns:
 * munich32 channel that corresponds to the bri channel
 * =====================================================================
 */
static ulong bri_m32_map_channel (hwidbtype *idb)
{
    ulong m32_channel_value = BRI_M32_D_CHAN_VALUE,
          m32_channel;

    switch (idb->isdn_info->chantype)  {
    case ISDN_Dchan:
        m32_channel_value = BRI_M32_D_CHAN_VALUE;
        break;
    case B1:
        m32_channel_value = BRI_M32_B1_CHAN_VALUE;
        break;
    case B2:
        m32_channel_value = BRI_M32_B2_CHAN_VALUE;
        break;
    default:
	break;
    }

    m32_channel = (idb->subunit * BRI_MAX_CHAN) + m32_channel_value;

    return (m32_channel);
}


/*
 * =====================================================================
 * bri_down_channels()
 *
 * Description:
 * Map each BRI channel to a Munich32 channel and down the channel.
 *
 * Parameters:
 * idb - the idb for the bri D channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_down_channels (hwidbtype *idb)
{
    m32_instance_t *m32_ds;
    ulong channel;
    
    m32_ds = M32_INSTANCE(idb->slot, idb->subcont);
 
    channel = bri_m32_map_channel(idb);
    m32_down_channel(m32_ds, channel);
    channel = bri_m32_map_channel(idb->isdn_info->b1_chan_idb);
    m32_down_channel(m32_ds, channel);
    channel = bri_m32_map_channel(idb->isdn_info->b2_chan_idb);
    m32_down_channel(m32_ds, channel);

}


/*
 * =====================================================================
 * bri_up_channels()
 *
 * Description:
 * Map each BRI channel to a Munich32 channel and up the channel.
 *
 * Parameters:
 * idb - the idb for the bri D channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_up_channels (hwidbtype *idb)
{
    m32_instance_t *m32_ds;
    ulong channel;
    enum m32_error_t status = M32_NOERR;

    m32_ds = M32_INSTANCE(idb->slot, idb->subcont);
   
    channel = bri_m32_map_channel(idb);
    if ((status = m32_up_channel(m32_ds, channel)) == M32_NOERR) {
	channel = bri_m32_map_channel(idb->isdn_info->b1_chan_idb);
	if ((status = m32_up_channel(m32_ds, channel)) == M32_NOERR) {
	    channel = bri_m32_map_channel(idb->isdn_info->b2_chan_idb);
	    status = m32_up_channel(m32_ds, channel);
	}
    }
    if (status != M32_NOERR)
	errmsg(&msgsym(INITFAIL, BRI), idb->unit,
	       "M32 channel configuration.");
}

/*
 * =====================================================================
 * bri_shutdown()
 *
 * Description:
 * Shut a BRI interface down by downing the munich32
 * channels and disabling the tp3420.
 *
 * Parameters:
 * dchan_idb - the idb for the bri D channel
 * newstate - the state this interface is being put into
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_shutdown (hwidbtype *idb, int newstate)
{
    leveltype int_level;
    hwidbtype *dchan_idb;

    dchan_idb = idb->isdn_info->d_chan_idb;	/* make sure it is dchannel*/
    int_level = raise_interrupt_level(HIRQ_INTLEVEL);

    dchan_idb->state = newstate;
    GET_TIMESTAMP(dchan_idb->state_time);
    /*
     * deactivate the bri interface chip
     */
    bri_reset(dchan_idb);
    /*
     * disable the bri interrupts for this interface
     */
    mb_tp3420_disable_line(dchan_idb);
    /*
     * down the channels at the hdlc controller level
     */
    bri_down_channels(dchan_idb);

    reset_interrupt_level(int_level);
}



/*
 * =====================================================================
 * bri_queue_for_process()
 *
 * Function to make the test interface command work with
 * serial_queue_for_process
 *
 * =====================================================================
 */
static void bri_queue_for_process (hwidbtype *idb, paktype *pak)
{
   echotype *ep;

   /*
    * See if this bri interface is under system loopback(internal) or plug 
    * loopback test(external). If so, queue the packet in loopback queue. 
    */
   if (idb->isdn_info->loopback_mode) {
       ep = dequeue(&test_bri_echoQ); 
       if (ep != NULL) {
           ep->active = FALSE;
           requeue(&test_bri_echoQ, ep);
           pak_enqueue(&test_bri_loopbackQ, pak);
           idb->counters.rx_cumbytes += pak->datagramsize;
           idb->counters.inputs++;
       }
       return;
   }
   serial_queue_for_process(idb, pak);
}
  

/*
 * =====================================================================
 * bri_RX_interrupt()
 *
 * Description:
 * Take a bri packet and pass it up to process level. If the packet
 * is for a B channel, set the encapsulation type in the pak struct.
 * Bridge the packet if it is a transit packet.
 * The bri interrupt routines are called from the Munich32 interrupt
 * handler. The Munich32 isr handles all ring and buffering issues.
 * The bri interrupt handlers need only handle bri related issues.
 *
 * Parameters:
 * idb - the idb for the bri channel 
 * pak - pointer to the received packet
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_RX_interrupt (hwidbtype *idb, paktype *pak)
{
    idbtype *dchan_swidb;
    ushort rxtype;

    if(ISBRILEASED(idb))
        dchan_swidb = idb->firstsw;
    else
        dchan_swidb = idb->isdn_info->d_chan_idb->firstsw;

    if (idb->isdn_info->chantype == ISDN_Dchan
		&& (!ISBRILEASED(idb))) {
        /*
         * D channel packet, send it straight to process level
         */
        bri_queue_for_process(idb, pak);
    } else { /* B channel packet */
        switch (idb->enctype) {
        case ET_HDLC:
            rxtype = hdlc_decode_encapsulation_inline(pak, 
                                          (vhdlctype *)(pak->datagramstart));
	    if (reg_invoke_hdlc_fs(rxtype, pak)) {
		return;
	    }
            break;
        case ET_PPP:
            rxtype = ppp_decode_encapsulation_inline(pak,
                                          (ppp_hdr *)(pak->datagramstart));
	    if (reg_invoke_ppp_fs(rxtype, pak)) {
		return;
	    }
            break;
        default:
            rxtype = RXTYPE_UNKNOWN;
        }

        if ((rxtype == RXTYPE_TRANSIT) && (dchan_swidb->tbridge_on_swidb)) {
            pak->enctype = ET_BRIDGE;
            if (!change_if_input(pak, dchan_swidb)) {
                bri_drop_pak(idb, pak);
		return;
            }
            if (reg_invoke_tbridge_forward(pak))
		return;
        }
        bri_queue_for_process(idb, pak);
    }
}


/*
 * =====================================================================
 * bri_TX_interrupt()
 *
 * Description:
 * Handle a just transmitted packet notification.
 * The bri interrupt routines are called from the Munich32 interrupt
 * handler. The Munich32 isr handles all ring and buffering issues.
 * The bri interrupt handlers need only handle bri related issues.
 *
 * Parameters:
 * idb - the idb of the bri channel 
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_TX_interrupt (hwidbtype *idb, paktype *pak)
{
    GET_TIMESTAMP(idb->lastoutput);
    m32_tx_start(idb);
}


/*
 * =====================================================================
 * bri_m32_setup_init_struct()
 *
 * Description:
 * Setup a munich32 channel for a BRI channel so that
 * it can be initialized.
 *
 * Parameters:
 * idb - the idb for the bri channel 
 * channel_init - ptr to channel initialization struct
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_m32_setup_init_struct (hwidbtype *idb, 
				       m32_channel_init_info_t *channel_init)
{

    channel_init->channel_num = idb->m32_channel;
    channel_init->channel_idb = idb;

    /*
     * Figure out what encapsulation we are configured for and 
     * save the datagramstart offset for bri_getbuf
     */
    switch (idb->enctype) {
       case ET_HDLC:
       case ET_FRAME_RELAY:
       case ET_PPP:
       case ET_SMDS:
       case ET_SDLCP:
       case ET_SDLCS:
       case ET_SDLC:
       case ET_STUN:
       case ET_BSTUN:
       case ET_LAPB:
       case ET_X25:
       case ET_V120:
       case ET_CPP:
          channel_init->data_offset = ENCAPBYTES - idb->encsize;
          break;
       default:
          errmsg(&msgsym(BOGUSENCAP, LINK), idb->hw_namestring, idb->enctype);
          channel_init->data_offset = ENCAPBYTES - HDLC_ENCAPBYTES;
    }

    /*
     * setup the bitrate and the ring sizes for this channel
     */
    if (idb->isdn_info->chantype == ISDN_Dchan) {
        /*
         * D channel bitrate is always 16 kbit and uses the
         * most significant bits in the timeslot.
         */
        channel_init->bitrate = 16;
        channel_init->bitrate_which_bits = M32_TIMESLOT_USE_MSB;
        /*
         * don't allow the D channel to be throttled. 
         */
        channel_init->throttle_channel = FALSE;
        /*
         * D channel rings sizes are a fixed small size.
         */
        channel_init->rx_ring_size = BRI_D_CHAN_RX_SIZE;
	if (hwidb_use_short_tx_q_inline(idb))
	    channel_init->tx_ring_size = make_ring_size(2);
	else
	    channel_init->tx_ring_size = BRI_D_CHAN_TX_SIZE;
        /*
         * out of the 64 long words of Munich32 internal transmit
         * buffer, assign some to the D channel.
         */
        channel_init->internal_buf_size = BRI_DCHAN_M32_XMIT_BUF;
    } else { /* b channel */
        if (idb->isdn_info->speed == 56) {
            channel_init->bitrate = 56;
        } else {
            channel_init->bitrate = 64;
        }
        channel_init->bitrate_which_bits = M32_TIMESLOT_USE_MSB;
        /*
         * B channels should be throttled when required.
         */
        channel_init->throttle_channel = TRUE;
        /*
         * adjust the B channels ring size depending on its MTU
         */
	select_ring_size((int *)&channel_init->rx_ring_size,
			 (int *)&channel_init->tx_ring_size,
			 idb->max_buffer_size);
 	channel_init->rx_ring_size =
 	    channel_init->rx_ring_size / BRI_BCHAN_BUF_REDUCTION_FACTOR;
	if (hwidb_use_short_tx_q_inline(idb))
	    channel_init->tx_ring_size = make_ring_size(2);
	else
	    channel_init->tx_ring_size =
 	        channel_init->tx_ring_size / BRI_BCHAN_BUF_REDUCTION_FACTOR;
	
        /*
         * out of the 64 long words of Munich32 internal transmit
         * buffer, assign some to the B channel.
         */
        channel_init->internal_buf_size = BRI_BCHAN_M32_XMIT_BUF;
    }

    /*
     * setup the vectors for disabling and enabling the tp3420.
     */
    channel_init->disable_adaptor = 
        (m32_disable_adaptor_t)bri_disable;
    channel_init->enable_adaptor = 
        (m32_enable_adaptor_t)mb_tp3420_enable_line;
    channel_init->down_connection = 
        (m32_down_connection_t)mb_tp3420_deactivate_line;

    /*
     * channels and timeslots always have a 1 to 1
     * correspondance for BRI. for example, channel 4
     * always uses timeslot 4. set the appropriate bit 
     * in the timeslot bitfield for this channel. 
     * Munich32 code expects a bitfield since other
     * port adaptors may have multiple timeslots per channel.
     */
    channel_init->timeslot_bitfield = 1 << channel_init->channel_num;
    channel_init->loopback = M32_NO_LOOPBACK;

    /*
     * setup the receive and transmit interrupt handlers
     */
    channel_init->recv_handler = bri_RX_interrupt;
    channel_init->xmit_handler = bri_TX_interrupt;

    /*
     * start all BRI channels in the UP state. disabling
     * and enabling channels is handled by the interface chip.
     */
    channel_init->initial_state = M32_CHANNEL_UP;
}


/*
 * =====================================================================
 * bri_init_channel()
 *
 * Description:
 * Initialize a BRI channel at the Munich32 level.
 * If this channel has never been initialized or if
 * there is a configuration change for this channel
 * do a full initialization. Otherwise, just reset
 * the channel.
 *
 * Parameters:
 * dchan_idb - the idb for the bri D channel
 * channel_config_change - whether or not this init requires 
 *                         changing the channel's configuration
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static enum m32_error_t bri_init_channel (hwidbtype *idb, 
					  boolean channel_config_change)
{
    m32_channel_init_info_t channel_init;
    m32_instance_t *m32_ds;
    enum m32_channel_state_t state;
    enum m32_error_t status = M32_NOERR;
    pooltype *old_pool;

    m32_ds = M32_INSTANCE(idb->slot, idb->subcont);
    state = m32_get_channel_state_inline(m32_ds, idb->m32_channel);
    
    if (state == M32_CHANNEL_UNUSED || channel_config_change) {
        /*
         * uninitialized or a channel config change,
         * do a full initialization
         */
        bri_m32_setup_init_struct(idb, &channel_init);

	/*
	 * If a private buffer pool for this interface exists AND there
	 * has been an MTU change, discard the pool with the old-sized
	 * buffers. 
	 */

	if (idb->pool && (idb->pool->size != idb->max_buffer_size)) {
	  old_pool = idb->pool;
	  pool_destroy(old_pool);
	  idb->pool = NULL;
	}
    
	/* 
	 * If the private buffer pool has not been allocated yet, or if
	 * it was destroyed due to an MTU change above, allocate it now.
	 * Populate the pool, initialize a private buffer cache,
	 * and populate the buffer cache with buffers from the newly
	 * created private pool.  Also set up the fallback public buffer
	 * pool. 
	 */

	if (!idb->pool) {
	    if (idb->pool_group == POOL_GROUP_INVALID)
		idb->pool_group = pool_create_group();

	    if (!idb->hw_namestring)
		idb_init_names(idb->firstsw, FALSE);

	    idb->pool = pak_pool_create(idb->hw_namestring,
					idb->pool_group,
					idb->max_buffer_size, POOL_SANITY,
					NULL); 
	    if (!idb->pool) {
		crashdump(0);
	    }
	    idb->pool->fallback = pak_pool_find_by_size(idb->max_buffer_size);
	    pool_adjust(idb->pool, 0, PRIV_NUM_BUF(channel_init.rx_ring_size),
			PRIV_NUM_BUF(channel_init.rx_ring_size), TRUE); 
	    pak_pool_create_cache(idb->pool,
				  PRIV_NUM_CACHE(channel_init.rx_ring_size));
	    pool_adjust_cache(idb->pool,
			      PRIV_NUM_CACHE(channel_init.rx_ring_size)); 
	}

	status = m32_init_channel(m32_ds, &channel_init);

    } else if (state == M32_CHANNEL_DOWN) {
        /*
         * channel already initialized but down, up the channel
         */
        status = m32_up_channel(m32_ds, idb->m32_channel);
    } else if (state == M32_CHANNEL_UP || state == M32_CHANNEL_STANDBY) {
        /*
         * channel already initialized and up, just reset the channel
         */
        status = m32_reset_channel(m32_ds, idb->m32_channel);
    } else {
        status = M32_ERR_BAD_STATE;
    }

    holdq_reset(idb);

    return(status);
}


/*
 * =====================================================================
 * c4000_set_bri_bandwidth()
 *
 * Description:
 * Set the bandwidth of a BRI B channel by setting a field
 * in the isdn_info struct and re-initializing the channel.
 *
 * Parameters:
 * idb - the idb for the bri channel 
 * bandwidth - new bandwidth of the channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void c4000_set_bri_bandwidth (hwidbtype *idb, uint requested_bw)
{
    enum Chan channel_type;
    ulong current_bw;

    channel_type = idb->isdn_info->chantype;

    /* 
     * only the B channels can have their bandwidths changed
     */
    if (channel_type != ISDN_Dchan) {
        /* 
         * get the current bandwidth of this channel
         */
        current_bw = 
            m32_get_channel_bw_inline(M32_INSTANCE(idb->slot, idb->subcont),
                                               idb->m32_channel);
        if (requested_bw == 56 && current_bw != 56) {
	    set_default_interface_bandwidth(idb->firstsw,
					    VISIBLE_BRI_BANDWIDTH_56);
            /* 
             * re-initialize the channel. this will cause 
             * the chip to change the channel's bandwidth.
             */
             if (bri_init_channel(idb, TRUE) != M32_NOERR)
 		errmsg(&msgsym(INITFAIL, BRI), idb->unit,
 		       "M32 channel configuration.");

            if (isdn_debug)
                buginf("\nBRI %d %s: Changing bandwidth from %dKb to %dKb\n",
                    idb->unit, (channel_type == B1) ? "B1" : "B2", 
                    current_bw, requested_bw);
        } else if (requested_bw == 64 && current_bw != 64) {
	    set_default_interface_bandwidth(idb->firstsw,
					    VISIBLE_BRI_BANDWIDTH_64);
            /* 
             * re-initialize the channel. this will cause 
             * the chip to change the channel's bandwidth.
             */
             if (bri_init_channel(idb, TRUE) != M32_NOERR)
 	        errmsg(&msgsym(INITFAIL, BRI), idb->unit,
 		       "M32 channel configuration.");
 
            if (isdn_debug)
                buginf("\nBRI %d %s: Changing bandwidth from %dKb to %dKb\n",
                    idb->unit, (channel_type == B1) ? "B1" : "B2", 
                    current_bw, requested_bw);
        }
    }
}


/*
 * =====================================================================
 * bri_restart()
 *
 * Description:
 * Restart a BRI interface by re-initializing the interface
 * and enabling the tp3420.
 *
 * Parameters:
 * idb - the idb for the bri channel 
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void bri_restart (hwidbtype *idb)
{
    hwidbtype *dchan_idb;

    dchan_idb = idb->isdn_info->d_chan_idb;	/* make sure it is dchannel*/

    /*
     * Make sure the L2 knows we are down.
     */
    bri_disable(dchan_idb);

    /*
     * re-initialize the interface
     */
    bri_init(dchan_idb, TRUE);

    /*
     * enable the bri interface chip if we are up or we are a leased line. 
     */
    if (ISBRILEASED(dchan_idb) || dchan_idb->state != IDBS_ADMINDOWN) {
        mb_tp3420_enable_line(dchan_idb);
	mb_isdn_activate_line(dchan_idb);
    }
}


/*
 * =====================================================================
 * bri_enable()
 *
 * Description:
 * routine to clear throttling on a bri channel. calls the
 * Munich32 enable routine since throttling is handled at 
 * the Munich32 level. This is an idb vector.
 *
 * Parameters:
 * idb
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_enable (hwidbtype *idb)
{
    m32_instance_t *m32_ds;

    m32_ds = M32_INSTANCE(idb->slot, idb->subcont);

    if (idb->type == IDBTYPE_BRI) {
        m32_enable(m32_ds, idb->m32_channel, M32_CHANNEL_UP);
    }
}


/*
 * =====================================================================
 * bri_setup_bridging()
 *
 * Description:
 * Setup for bridging by re-initializing the interface.
 * This is an idb vector.
 *
 * Parameters:
 * idb
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void bri_setup_bridging (hwidbtype *idb)
{
    bri_init(idb, FALSE);
    if (idb->counters.resets > 0)
        idb->counters.resets--;
}


/*
 * =====================================================================
 * bri_show_errors()
 *
 * Description:
 * Print BRI errors.
 *
 * Parameters:
 * idb - the idb for the bri channel 
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_show_errors (hwidbtype *idb)
{
    mb_instance_t *mb_ds;
    m32_instance_t *m32_instance;

    mb_ds = MB_INSTANCE(idb->slot, idb->subcont);
    m32_instance = M32_INSTANCE(idb->slot, idb->subcont);

    printf("  %d missed datagrams, %d overruns\n",
      idb->counters.input_resource, idb->counters.input_overrun);
    printf("  %d bad datagram encapsulations, %d memory errors\n",
      idb->unknown_encaps, idb->dma_memory_error);
    printf("  %d transmitter underruns,",idb->counters.output_underrun);
    printf(" %d throttles, %d enables, %d bad interrupt elements\n",
      m32_instance->channel_params[idb->m32_channel].throttle_count,
      m32_instance->channel_params[idb->m32_channel].enable_count,
      m32_instance->bad_intq_element_count);
}


/*
 * =====================================================================
 * bri_show_controller()
 *
 * Description:
 * Print controller stats for a BRI interface.
 *
 * Parameters:
 * idb - idb of the bri channel 
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static boolean bri_show_controller (hwidbtype *in_idb, parseinfo *csb)
{
    hwidbtype *dchan_idb;
    hwidbtype *idb = NULL;
    mb_instance_t *mb_instance;
    mcn_instance_t *mcn_instance;
    m32_instance_t *m32_instance;
    ulong bri_channel,
          m32_channel;
    uchar master, i;

    dchan_idb = in_idb->isdn_info->d_chan_idb;	/* make sure it is dchannel*/
    if (in_idb != dchan_idb)
	return(FALSE);
    
    mcn_instance = MCN_INSTANCE(dchan_idb->slot);
    mb_instance = MB_INSTANCE(dchan_idb->slot, dchan_idb->subcont);
    m32_instance = M32_INSTANCE(dchan_idb->slot, dchan_idb->subcont);

    printf("BRI slot %d unit %d subunit %d",
      dchan_idb->slot, dchan_idb->unit, dchan_idb->subunit);

    /*
     * Print the layer 1 status.
     */
    switch (mb_instance->current_state[dchan_idb->subunit]) {
      case l1_state_F1:
      case l1_state_F2:
      case l1_state_F3:
	printf("\nLayer 1 is DEACTIVATED. (ISDN L1 State F%d)\n",
	       mb_instance->current_state[dchan_idb->subunit]);
	break;
      case l1_state_F4:
      case l1_state_F5:
      case l1_state_F6:
	printf("\nLayer 1 is PENDING ACTIVATION. (ISDN L1 State F%d)\n",
	       mb_instance->current_state[dchan_idb->subunit]);
	break;
      case l1_state_F7:
      case l1_state_F8:
	printf("\nLayer 1 is ACTIVATED. (ISDN L1 State F%d)\n",
	       mb_instance->current_state[dchan_idb->subunit]);
	break;
      default:
	printf("\nLayer 1 status is Unknown. (State F%d)\n",
	       mb_instance->current_state[dchan_idb->subunit]);
    }

    /*
     * Print out who is the master for the slot.
     */
    master = (uchar) mb_instance->master_clock_bitmap;
    if (master) {
	for (i = 0; i < mb_instance->no_of_subunits_present; i++) {
	    if (master & 0x01)
		break;
	    master >>= 1;
	}
	/*
	 * make sure it is activated before printing out the info.
	 */
	if (mb_instance->current_state[i] >= l1_state_F7)
	    printf("Master clock for slot %d is bri subunit %d. \n",
		   dchan_idb->slot, i);
    }

    /*
     * Print out the number of chip config success and failures
     */
    printf("Total chip configuration successes: %d, failures: %d, timeouts: %d\n",
      m32_instance->ar_success, 
      m32_instance->ar_failure, 
      m32_instance->ar_timeout);

    /* 
     * loop thru all 3 BRI channels and list the controller
     * information for each one.
     */
    for (bri_channel=0; bri_channel<BRI_MAX_CHAN; bri_channel++) {
        /* 
         * get the idb of the BRI channel.
         */
        switch (bri_channel) {
        case BRI_D_CHAN:
            idb = dchan_idb->isdn_info->d_chan_idb;
            printf("D Channel Information:\n");
            break;
        case BRI_B1_CHAN:
            idb = dchan_idb->isdn_info->b1_chan_idb;
            printf("B1 Channel Information:\n");
            break;
        case BRI_B2_CHAN:
            idb = dchan_idb->isdn_info->b2_chan_idb;
            printf("B2 Channel Information:\n");
            break;
        }

        m32_channel = bri_m32_map_channel(idb);
        m32_print_rings(m32_instance, m32_channel);
        
        bri_show_errors(idb);
    } /* end of for loop */

    printf("\n");
    /*
     * always return false becuase this is what the 
     * calling function is expecting.
     */
    return(FALSE);
}


/*
 * =====================================================================
 * bri_m32_get_idb()
 *
 * Description:
 * given the slot and sub-unit of a bri interface, return
 * the idb of the interface.
 *
 * Parameters:
 * idb - the idb for the bri channel 
 * subcont - the subcontroller number
 * sub_unit - which interface on the nim
 *
 * Returns:
 * nothing
 * =====================================================================
 */
hwidbtype *bri_m32_get_idb (uint slot, uint subcont, uint sub_unit)
{
    hwidbtype *idb = NULL;
    m32_instance_t *m32_ds;
    ulong d_channel;

    m32_ds = M32_INSTANCE(slot, subcont);

    d_channel = (sub_unit * BRI_MAX_CHAN) + BRI_M32_D_CHAN_VALUE;
    idb = m32_get_channel_idb_inline(m32_ds, d_channel);

    return (idb);
}


/*
 * =====================================================================
 * bri_txqlength()
 *
 * Description:
 * Return the number of frames on the transmit ring that are
 * waiting to be transmitted.
 *
 * Parameters:
 * idb 
 *
 * Returns:
 * count of frames on the transmit ring
 * =====================================================================
 */
static int bri_txqlength (hwidbtype *idb)
{
    m32_instance_t *m32_ds;
    int frame_count;
    
    m32_ds = M32_INSTANCE(idb->slot, idb->subcont);
    frame_count = 
      m32_ds->channel_params[idb->m32_channel].tx_ring_use_count;

    return(frame_count);
}


/*
 * =====================================================================
 * test_bri_echoBLOCK
 *
 * Description:
 * Wait for 
 *  - an ICMP Echo returned by loopback, or 
 *  - timeout, or
 *  - an escape sequence.
 *
 * Parameters:
 * data - data structure for ICMP type.          
 *
 * Returns:
 * TRUE or FALSE: whether the process will be kept blocking or not.
 *
 * =====================================================================
 */
static boolean test_bri_echoBLOCK (register echotype *data)
{
    if (!data->active)                    /* if block flag reset, wakeup */
        return(FALSE);
    if (psipending2((tt_soc *)data->tty)) /* interrupted? */
        return(FALSE);
    if (SLEEPING(data->timeout))          /* keep blocking if timer ticking */
        return(TRUE);
    return(FALSE);
}


/*
 * =====================================================================
 * test_bri_loopback_datagram_out
 *
 * Description:
 * Queue bri loopback-test output packet for the selected net.
 * We also timeout hung transfers.
 *
 * Parameters:
 * pak - pointer to the packet to be sent 
 *
 * Returns:
 * nothing 
 *
 * =====================================================================
 */
static void test_bri_loopback_datagram_out (paktype *pak)
{
    hwidbtype *idb = pak->if_output->hwptr;	/* addr of associated idb */
    unsigned acct_proto;		/* accounting protocol type */
    leveltype status;			/* lock out interrupts */
    boolean success;
    ulong acct_bytes;
    
    /*
     * Assume the data will be enqueued and subsequently delivered.
     */
    acct_proto = pak->acct_proto = ACCT_PROTO_IP;
    idb->counters.tx_frames[acct_proto]++;
    acct_bytes = pak->datagramsize;
    idb->counters.tx_bytes[acct_proto] += acct_bytes;
    status = raise_interrupt_level(NETS_DISABLE);
    
    /*
     *  We also have to maintain the other set of counters 
     */
    if (idb->soutput == (soutput_t)return_nothing) {
        /*
         * That means that we don't have "soutput" routine which should
         * take care of "output" counter
         */
        idb->counters.outputs++;
    }
    
    success = (*idb->oqueue)(idb, pak, TAIL);	/* add to output queue */
    reset_interrupt_level(status);
    
    if (!success) {
        /*
         * Output queue has filled up, or idb is set to an encapsulation
         * that performs its own idb->soutput, or idb is a virtual idb.
         * If output queue has really filled up, kick the soutput routine as
         * self protection against deadlock.
         */
        if (idb->oqueue == holdq_enqueue ||
            idb->oqueue == priority_enqueue) {
            idb->counters.tx_frames[acct_proto]--;
            idb->counters.tx_bytes[acct_proto] -= acct_bytes;
	    /* We also have to maintain the other set of counters */
            if (idb->soutput == (soutput_t)return_nothing) {
	        /* That means that we don't have "soutput" routine which 
		 * should take care of "output" counter
		 */
                idb->counters.outputs--;
            }
            (*(idb->soutput))(idb);
        }
    } else {
	(*idb->soutput)(idb);	/* start another output if pending */
    }
}


/*
 * =====================================================================
 * ping_test_for_bri
 *
 * Description:
 * Send a number of ICMP Echo Requests out an BRI interface and return
 * count of failures.  Prints a 'B1', 'B2', or 'D' on stdio on success of
 * B1, B2, and D channel loopback, respectively.
 *
 * Parameters:
 * idb - pointer to d channel idb                     
 *
 * Returns:
 * drops - count of failures
 *
 * Notes:
 * Will run until the repeat count reached or is aborted by the user.   
 * =====================================================================
 */
static int ping_test_for_bri (hwidbtype *idb)
{
    idbtype *swidb;
    echotype data;
    ulong size, repeat, seconds, maxsize;
    ulong hits, ix, jx;
    boolean hits_flag;
    ulong bri_channel; 
    int databytes;
    ushort *dataptr;
    
    isdn_bri_t *isdn_bri;
    
    iphdrtype *ip;
    icmptype *icmp;
    ipparamtype params;
    ushort pattern = 0xAA55;
    paktype *pak;                       /* packet to send */
    paktype *pak_copy;                  /* save a copy of the packet sent */   
    paktype *pak_in;                    /* paket received */
    int data_offset = ENCAPBYTES;
    int drops = 0; 
    
    /*
     * Set up pointers    
     */
    isdn_bri = idb->isdn_info;
    
    /*
     * Set default parameters.
     */
    size = BRI_TEST_DATA_SIZE;           /* default data size */
    repeat = BRI_TEST_DATA_COUNT;        /* default repeat count */
    seconds = BRI_TEST_DATA_GAP;         /* pause between packets in seconds */
    /*
     * For now, set default max datagram size to  be 1500.
     * It can be larger because the test
     * packet (ICMP echo request) will never get
     * fragmented with the current implementation
     * of loopback code.
     */
    maxsize = BRI_TEST_DATA_MAX_SIZE;
    
    /*
     * Ask for repeat count, size, and pause interval
     */
    if (!ping_common(&repeat, &size, (IPHEADERBYTES(NOPT) +
				      sizeof(icmptype)), maxsize, &seconds)) {
        printf("\nCan't get repeat count, size, or pause interval. Test aborted.");
	
        return(drops);
    }
    
    /*
     * Go for it.
     */
    printf(type_escape_seq_abort_msg);
    printf("\nSending %d, %d-byte ICMP Echos, timeout is %d seconds:",
	   repeat, size, seconds);
    printf("\n `B1' means `one loopback test passed on B1 channel'");
    printf("\n `B2' means `one loopback test passed on B2 channel'");
    printf("\n `D' means `one loopback test passed on D channel'");
    printf("\n `.' means `transmitted packet NOT equivalent to received packet on B1, B2, or D channel'");
    printf("\n `T' means `one timeout on receiving test packet on B1,B2, or D channel");
    printf("\nTest in progress. Please be patient.\n");
    process_sleep_for(100);	/* let output out */
    psion(stdio);
    
    for (ix = 0, hits = 0; (ix < repeat) && (psipending() == FALSE); ix++) {
        hits_flag = TRUE; 
	for (bri_channel=1; bri_channel<=BRI_MAX_CHAN; bri_channel++) {
            data.sequence = 0;
            data.tos = 0;
            data.options = NULL;
            
            if (!(pak = getbuffer(ENCAPBYTES + size))) {
                if (ix) {
                    ix--;
                } 
                goto test_done;
            }
	    
            pak->datagramsize = size; 
            pak->datagramstart = pak->data_area + data_offset;
	    
            data.id = ix % (MAXUSHORT + 1);
            ip = (iphdrtype *)ipheadstart(pak);
            /*
             * We initialize the packet buffer by zeroing the IP
	     * and ICMP header
             * and putting whatever the desired pattern
	     * in the rest of the packet.
             */
            databytes = size - IPHEADERBYTES(data.options) - sizeof(icmptype);
            if (databytes < 0)
                databytes = 0;
            icmp = (icmptype *)(ipheadstart(pak) +
				IPHEADERBYTES(data.options));
            dataptr = (ushort *)(icmp + 1);
            for (jx = 0; jx < btow(databytes) - 1; jx++)
                *dataptr++ = ~(pattern);
            memset(pak_center(pak), 0, size - databytes);
	    
            /*
             * Fill in the ICMP and IP portion of the datagram
             */
            icmp->type = ICMP_ECHO;
            icmp->code = 0;
            icmp->checksum = 0;
            icmp->identifier = data.id; 
            icmp->sequence = data.sequence; 
            icmp->checksum = ipcrc((ushort *)icmp,(size - IPHEADERBYTES(data.options)));
	    
            params = ipdefaultparams;           /* struct copy */

	    /*
	     * use for unique id
	     */
            params.ident = (short)icmp_traffic.echosent;
	    
            /*
             * Build the header.
             */
            ip->version = IPVERSION;        /* IP version number */
            ip->tos = params.tos;           /* type of service */
            ip->tl = size;                  /* length of packet */
 
            ip->id = params.ident;          /* packet ID */
            ip->fo = 0;                     /* no fragment offset */
            ip->ipreserved = 0;             /* reserved field */
            ip->dontfragment = params.dontfragment; /* may fragment? */
            ip->morefragments = FALSE;      /* no more fragments */
            ip->prot = ICMP_PROT;           /* protocol type */
            ip->ihl = btol(IPHEADERBYTES(data.options)); /* header length */
	    
            swidb = idb->firstsw;
            ip->srcadr = swidb->ip_address;     /* source address */
            ip->dstadr = swidb->ip_address;     /* destination address */
	    
            pak->if_output = swidb;
            pak->if_input = swidb;
            switch (bri_channel) {
	      case 1:
		pak->if_output->hwptr = isdn_bri->b1_chan_idb;
		break;
	      case 2:
		pak->if_output->hwptr = isdn_bri->b2_chan_idb;
		break;
	      case 3:
		pak->if_output->hwptr = isdn_bri->d_chan_idb;
		break;
	      default: 
		printf("\nWrong BRI channel. Test aborted.");
		datagram_done(pak);
		return(++drops);
            }
	    
            switch (pak->if_output->hwptr->enctype) {
	      case ET_HDLC:
		pak->datagramsize += HDLC_ENCAPBYTES;
		pak->datagramstart -= HDLC_ENCAPBYTES;
		if (pak->datagramstart < pak->data_area) {
		    printf("\nInvalid datagram start address. Test aborted.");
		    datagram_done(pak);
		    return(++drops);
		}
		pak->enctype = ET_HDLC;
		pak->refcount = 1;       
		break;
	      case ET_PPP:
		pak->datagramsize += PPP_ENCAPBYTES;
		pak->datagramstart -= PPP_ENCAPBYTES;
		if (pak->datagramstart < pak->data_area) {
		    printf("\nInvalid datagram start address. Test aborted.");
		    datagram_done(pak);
		    return(++drops);
		}
		pak->enctype = ET_PPP;
		pak->refcount = 1;       
		break;
	      default:
		printf("\nInvalid encapsulation. Test aborted.");
		datagram_done(pak);
		return(++drops);
            }
	    
            pak_copy = pak_duplicate(pak);
            if (!pak_copy) {
                datagram_done(pak);
                drops++;
                if (ix) {
                    ix--;
                }
                goto test_done;
            }
            icmp_traffic.echosent++;
	    
            data.next = NULL;
            data.active = TRUE;
            data.tty = stdio;
            TIMER_START(data.timeout, seconds * ONESEC);
	    
            /*
	     * send ICMP echo packet out of the BRI interface under test
             */
            test_bri_loopback_datagram_out(pak);
	    
            /*
             * go to sleep and wait for ICMP Echo returned via loopback 
             */
            enqueue(&test_bri_echoQ, &data);
            edisms((blockproc *)test_bri_echoBLOCK, (ulong)&data);
            unqueue(&test_bri_echoQ, &data);
            if (!data.active) {
                /*
                 * It's either Echo packet or escape sequence has
		 * been received.
                 */
                if ((pak_in = pak_dequeue(&test_bri_loopbackQ))) {
                    /*
                     * Echo packet has been looped back and received
                     */
                    ip_traffic.inputs++;
                    icmp_traffic.echorcv++; /* echo received via loopback */
                    swidb = pak_in->if_input;
                    if (swidb) {
                        swidb->hwptr->counters.rx_frames[ACCT_PROTO_IP]++;
                        swidb->hwptr->counters.rx_bytes[ACCT_PROTO_IP]
			    += pak->datagramsize;
                    }
                    /*
                     * compare transmitted packet and received packet. If they
                     * are the same, mark test successful.
                     */
                    if (bcmp(pak_copy->datagramstart, pak_in->datagramstart, 
			     pak_copy->datagramsize) == 0) {
                        hits_flag &= TRUE;      
                        printf("%s", (bri_channel == 1 ? "B1" :
				      (bri_channel == 2 ? "B2" : "D")));
                    } else {
                        hits_flag &= FALSE;      
                        drops++;
                        printf(".");
                    }
                    datagram_done(pak_in);
                    datagram_done(pak_copy);
                } else {
                    /*
                     * escape sequence received 
                     */
                    datagram_done(pak_copy);
                    ix--;  
                    goto test_done;
                } 
            } else {
                /*
                 * timeout on receiving
                 */
                hits_flag &= FALSE;      
                drops++;
	        printf("T");
                datagram_done(pak_copy);
            } 
        }   /* end of B1, B2, D loop */
        if (hits_flag == TRUE) {
            hits++;
        } 
    }   /* end of test loop */ 
  test_done:    
    psioff(stdio);
    if (isdn_bri->loopback_mode == SYSTEM_LOOPBACK_ENABLED) {
        printf("\nSuccess rate of internal loopback test is %d percent (%u/%u)", hits > MAXULONG/100 ? hits / (ix/100) : (hits*100) / ix, hits, ix);
    } else {
        printf("\nSuccess rate of external loopback test is %d percent (%u/%u)", hits > MAXULONG/100 ? hits / (ix/100) : (hits*100) / ix, hits, ix);
    }    
    /*
     * To mark test sucessful, we allows maximumally 2 drops which may be 
     * caused by
     * 1) aborting the test using escape sequence,
     * 2) real timeout on receiving a test packet, or
     * 3) unequivalent transmitted and received packets.
     */
    if (drops < 3) {
        drops = 0;
    }
    return(drops); 
}   /* end of ping_test_for_bri */


/*
 * =====================================================================
 * test_bri_interface_loopback -
 *
 * Description:
 * This function tests a bri interface by enabling loopback on the
 * TP3420 chip (system loopback), and/or by enabling the external loopback
 * (plug loopback), which requires a loopback plug.
 *
 * Parameters:
 * idb - hwidbtype ptr to the bri interface 
 * bri_skippred - flag that indicates whether one or both of the loopback 
 *                tests (chip or plug) is skipped.
 *
 * Returns:
 * drops - number of failures 
 *
 * Notes:
 *
 * =====================================================================
 */
static int test_bri_interface_loopback (hwidbtype *idb, ushort *bri_skipped)
{
    int drops = 0; 
    ulong bri_channel; 
    hwidbtype *dchan_idb,
              *b1chan_idb,
              *b2chan_idb;
    isdn_bri_t *isdn_bri;
    enum Loopback looptest;
    boolean test_aborted = FALSE;
    
    /*
     * Set all idb variables
     */
    isdn_bri = idb->isdn_info;
    dchan_idb = isdn_bri->d_chan_idb;     /* make sure it is d channel*/
    b1chan_idb = isdn_bri->b1_chan_idb;
    b2chan_idb = isdn_bri->b2_chan_idb;
    
    /*
     * check to see if remote loopback is enabled.
     * Note that remote loopback was implemented before and use 'loopback'
     * field in idb structure, while system loopback and chip loopback
     * that are implemented now use 'loopback_mode' in isdn_info structure.
     * For the purpose of integrity, putting all these fields in one structure
     * is recommended.
     */
    if (dchan_idb->loopback) { 
        printf("\nMust remove remote loopback before testing");
        return(drops);
    }
    
    if (isdn_bri->loopback_mode == LOOPBACK_DISABLED) {
        for (looptest = SYSTEM_LOOPBACK_ENABLED;looptest !=
	     LOOPBACK_DISABLED;) {
            /*
             * no loopback test has currently being conducted. So go ahead.
             */
            clearinput(stdio);
            printf("\n%s loopback test [y/n] ?",
		   (looptest == SYSTEM_LOOPBACK_ENABLED ? "internal" :
		    "external"));
            if (doit()) {
                if (looptest == PLUG_LOOPBACK_ENABLED) {
                    printf("\nUnplug %s cable and insert the loopback plug.",
			   idb->hw_namestring);
                    printf("\nThe procedure to build such a plug:");
                    printf("\n1. connect pin 3 and 4, and pin 5 and 6, respectively, of a RJ45 plug.");
                    printf("\n2. put a 50 Ohm terminating resistor between pin 4 and 5.");
                    printf("\nWarning: executing a plug loopback test without unplugging the existing %s cable and plugging a loopback plug is not a valid test and may cause SERIOUS problems.", idb->hw_namestring);
                    printf("\n**** Make sure you unplug %s cable and insert the loopback plug. ****\n", idb->hw_namestring); 
                    printf("\n external loopback test [y/n] ?");
                    if (!doit()) {
                        (*bri_skipped)++;
                        looptest = LOOPBACK_DISABLED;
                        break;   
                    }
                }
                /*
                 * - set loopback flag for interface under test.
                 * - enable loopback
                 */
                isdn_bri->loopback_mode =
		    b1chan_idb->isdn_info->loopback_mode =
			b2chan_idb->isdn_info->loopback_mode = looptest;
                mb_tp3420_init(dchan_idb);
                mb_tp3420_enable_line(dchan_idb);
                mb_tp3420_enable_loopback(dchan_idb, looptest);
                /*
                 * do a full initialization on the associated Munich32 channel
		 */
		for (bri_channel = 1; bri_channel <= BRI_MAX_CHAN;
		     bri_channel++) {
		    switch (bri_channel) {
		      case 1:
			idb = isdn_bri->b1_chan_idb;
			break;
		      case 2:
			idb = isdn_bri->b2_chan_idb;
			break;
		      case 3:
			
			idb = isdn_bri->d_chan_idb;
			break;
		      default:
			printf("\nWrong BRI channel. Test aborted.");
			drops++;
			test_aborted = TRUE;
			break;
		    }
		    if (bri_init_channel(idb, TRUE) != M32_NOERR) {
			printf("\nMunich32 channel initialization problem. Test aborted");
			drops++;
			test_aborted = TRUE;
			
			break;  
		    }
		}
		if (!test_aborted) {
		    test_aborted = FALSE;
		    bri_up_channels(dchan_idb);
		    drops += ping_test_for_bri(dchan_idb);
		}
		/*
		 * remove loopback setup when test is done.
		 */                           
                isdn_bri->loopback_mode = LOOPBACK_DISABLED;
                b1chan_idb->isdn_info->loopback_mode = LOOPBACK_DISABLED;
                b2chan_idb->isdn_info->loopback_mode = LOOPBACK_DISABLED;
                mb_tp3420_enable_loopback(dchan_idb, LOOPBACK_DISABLED);
                /*
                 * deactivate the bri interface chip and disable
                 * its interrupts
                 */
                bri_disable(dchan_idb);
                bri_down_channels(dchan_idb);
                switch (looptest) {
		  case SYSTEM_LOOPBACK_ENABLED:
		    looptest = PLUG_LOOPBACK_ENABLED; 
		    break;
		  default:
		    looptest = LOOPBACK_DISABLED; 
		    break;
                }
            } else {
                (*bri_skipped)++; 
                switch (looptest) {
		  case SYSTEM_LOOPBACK_ENABLED:
		    looptest = PLUG_LOOPBACK_ENABLED;
		    break;
		  default:
		    looptest = LOOPBACK_DISABLED;
		    break;
                }
            }
        }   /* end of looptest loop */
        return(drops);
    } else if ((isdn_bri->loopback_mode == SYSTEM_LOOPBACK_ENABLED) ||
               (isdn_bri->loopback_mode == PLUG_LOOPBACK_ENABLED)) {
        printf("\n%s is being tested. Try later.", idb->hw_namestring);
        *bri_skipped = 2; /*skip both chip and plug loopback test */
        return(drops);
    } 
    printf("\nTesting BRI interface is not yet supported on this platform.");
    return(drops);
}


/*
 * =====================================================================
 * test_bri_interface -
 *
 * Description:
 * This function is invoked by the test_interface registry for BRI.
 * The BRI test interface function is different from other interfaces.
 * It does an internal loopback and an external loopback test.
 *
 * Parameters:
 * hwidbtype *,
 * int *failed cases
 * int *passed cases
 * int *skipped cases
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
static void test_bri_interface (hwidbtype *idb, int *nfailed, int *npassed,
				int *nskipped)
{
    int drops = 0, ierrors, oerrors;
    isdn_bri_t *isdn_bri;
    ushort test_bri_skipped = 0;
    char *reason = NULL;
    
    if (idb->state != IDBS_ADMINDOWN) {
	printf("SHUTDOWN required when testing BRI.\nWarning: make sure there is no existing traffic on %s.\nSkipping...", idb->hw_namestring);
	(*nskipped)++;
	return;                          
    } else {
	isdn_bri = idb->isdn_info;
	ierrors = idb->counters.input_error + 
	    isdn_bri->b1_chan_idb->counters.input_error +
		isdn_bri->b2_chan_idb->counters.input_error;
	oerrors = idb->counters.output_error + 
	    isdn_bri->b1_chan_idb->counters.output_error +
		isdn_bri->b2_chan_idb->counters.output_error;
	
	/*
	 * Perform loopback tests and get the drops count
	 */
	drops = test_bri_interface_loopback(idb, &test_bri_skipped);
	
	/*
	 * check to see if loopback test is skipped and adjust
	 * the ierrors value accordingly. This is because 
	 * after setting up the loopback (chip or plug), one input 
	 * error for each channel (B1, B2, and D) on the interface 
	 * under is always detected by Munich 32 chip, which may be 
	 * the limitation of the chip. We need to adjust
	 * the value of ierrors so that when it is used later
	 * to compare with the input errors after the test (
	 * aftertest_ierrors), the true errors detected during the
	 * test can be reflected.
	 */
	if (test_bri_skipped == BRI_NO_LOOP_SKIPPED) {
	    /* neither chip nor plug loopback test skipped */
	    ierrors += BRI_MAX_CHAN * 2; /* chip and loop; B1, B2, and D */ 
	} else if (test_bri_skipped == BRI_ONE_LOOP_SKIPPED) {
	    /* either chip or plug loopback test skipped */
	    ierrors += BRI_MAX_CHAN; /* chip or loop; B1, B2, and D */ 
	} else if (test_bri_skipped == BRI_BOTH_LOOP_SKIPPED) { 
	    /* both chip and plug loopback test skipped */
	    (*nskipped)++;
	    return;  
	} else {
	    /*
	     * Should never comes here. Should it do,
	     * abort the test.
	     */
	    printf("\nInvalid value of `test_bri_skipped'. Test aborted.\n");
	    return; 
	}
	
	if (drops == 0) {
	    int aftertest_ierrors, aftertest_oerrors;
	    
	    isdn_bri = idb->isdn_info;
            aftertest_ierrors = idb->counters.input_error +
		isdn_bri->b1_chan_idb->counters.input_error +
		    isdn_bri->b2_chan_idb->counters.input_error;
            aftertest_oerrors = idb->counters.output_error +
		isdn_bri->b1_chan_idb->counters.output_error +
		    isdn_bri->b2_chan_idb->counters.output_error;
	    /*
             * tolerate one input error on each channel (B1, B2, and D) which 
             * occurs when setting up loopback.
             */
            if (aftertest_ierrors > ierrors + 1) {
                reason = "reception";
            } else if (aftertest_oerrors > oerrors + 1) {
                reason = "transmission";
            } else {
                reason = NULL;
            }
	} else {
	    reason = "timeout OR `transmitted vs received packet mismatch'";
	}  /* drops not equal to 0 */
	
	if (reason) {
	    printf("\nTest result for %s: Failed - %s problem.", 
		   idb->hw_namestring, reason);
	    (*nfailed)++;
	    if (reason == "reception") {
		printf(" Input errors on %s.", idb->hw_namestring);
	    } else if (reason == "transmission") {
		printf(" Output errors on %s.", idb->hw_namestring);
	    }
	} else {
	    printf("\nTest result for %s: Passed", idb->hw_namestring);
	    (*npassed)++;
	}
    }
}


/*
 * =====================================================================
 * bri_init -
 * 
 * Description:
 * Initailize a BRI interface. this includes the munich32
 * channels and the tp3420 chip.
 *
 * Parameters:
 * hwidbtype *dchan_idb
 * maxdgram_change :	boolean to indicate if max dgram size changed.
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
void bri_init (hwidbtype *idb, boolean maxdgram_change)
{
    hwidbtype *dchan_idb;
    leveltype SR;
    uint bri_channel;
    enum m32_error_t status = M32_NOERR;
	isdn_bri_t *isdn_info;

    /*
     * Do not allow the whole interface to be reset when leased line is
     * configured and bri_init is called with a B channel idb
     */
    if(ISBRILEASED(idb) && (idb->isdn_info->chantype != ISDN_Dchan))
        return;

	dchan_idb = idb->isdn_info->d_chan_idb;	/* make sure it is dchannel*/
	isdn_info = dchan_idb->isdn_info;

    /*
     * If we are administratively down, dont re-enable.
     */
    if (!ISBRILEASED(dchan_idb) && (dchan_idb->state == IDBS_ADMINDOWN) &&
        !maxdgram_change)
        return;

    SR = raise_interrupt_level(HIRQ_INTLEVEL);	/* disable interrupts */
    dchan_idb->counters.resets++;	/* bump count */

    /*
     * Put the SID in a known down state.
     */
    bri_reset(dchan_idb);
    for (bri_channel = 0; bri_channel < BRI_MAX_CHAN; bri_channel++) {
	/* Set up IDB channel pointers */
	switch (bri_channel) {
	  case BRI_D_CHAN:
	    idb = dchan_idb;
	    break;
	    
	  case BRI_B1_CHAN:
	    idb = dchan_idb->isdn_info->b1_chan_idb;
	    break;
	    
	  case BRI_B2_CHAN:
	    idb = dchan_idb->isdn_info->b2_chan_idb;
	    break;
	}

	/*
	 * Init and reset the controller channels and the holdQ.
	 * Note that this should clear all the ring variables and
	 * such.
	 */
	 status = bri_init_channel(idb, maxdgram_change);
	 if (status != M32_NOERR) {
	     dchan_idb->state = IDBS_DOWN;
             GET_TIMESTAMP(dchan_idb->state_time);
	     reset_interrupt_level(SR);
	     return;
	 }
    }

    /*
     * Initialise the SID.
     */
    mb_tp3420_init(dchan_idb);

    if (! dchan_idb->loopback) {
	idb_init_names(dchan_idb->firstsw, FALSE);
        if ((dchan_idb->state != IDBS_ADMINDOWN) && (!ISBRILEASED(dchan_idb) ||
			(is_layer1_current_state_activated(dchan_idb))))
	    net_cstate(dchan_idb, IDBS_UP);
    }


    reset_interrupt_level(SR);

    /*
     * Initialize the loopback queue used in bri loopback test
     * Initialize the ICMP echo queue used in bri loopback test
     */
    test_bri_init(dchan_idb);

	/*
	 * If this interface has been configure for leased line ISDN then enable
	 * the interface and both B channels.
	 */
	if(ISBRILEASED(dchan_idb)){
		(*isdn_info->activate_line)(dchan_idb);
		(*isdn_info->enable_bri_channel)(dchan_idb->isdn_info->b1_chan_idb);
		(*isdn_info->enable_bri_channel)(dchan_idb->isdn_info->b2_chan_idb);
	}
}


/*
 * =====================================================================
 * bri_init_idb_vectors -
 *
 * Description:
 * Set the idb function pts. This is called from init_bri_idb_common().
 * This function is separate for 4000 and low-end since the idb
 * vectors are different for the two.
 *
 * Parameters:
 * hwidbtype *idb, could be d channel or b channel.
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
void bri_init_idb_vectors (hwidbtype *idb)
{
      /*
       * Set up function calls
       */
      idb->setencap = serial_encapsulation;
      idb->soutput = m32_tx_start;
      idb->reset = bri_restart;
      idb->shutdown = bri_shutdown;
      idb->enable = bri_enable;
      idb->setup_bridging = NULL;
      idb->periodic = NULL;
      idb->device_periodic = NULL;
      idb->clear_support = NULL;
      idb->show_controller = bri_show_controller;
      idb->txqlength = bri_txqlength;
      idb->fastsend = m32_fastsend;
      idb->isdn_info->activate_line = mb_isdn_activate_line;
      idb->isdn_info->disable_bri_channel = mb_isdn_disable_bri_channel;
      idb->isdn_info->enable_bri_channel = mb_isdn_enable_bri_channel;
      idb->isdn_info->set_bri_bandwidth = c4000_set_bri_bandwidth;
}  


/*
 * =====================================================================
 * init_bri_idb -
 *
 * Description:
 * Initialise the idbs and call init_bri_idb_common which enQs
 * the dchannel idb into the idbQ.
 *
 * Parameters:
 * slot, unit and subunit of the BRI interface in the 4xxx.
 * 
 * Returns: nothing.
 * =====================================================================
 */
void init_bri_idb (mcn_instance_t *ds,
		   uint slot,
                   uint subcont,
		   uint unit,
		   uint subunit)
{
    hwidbtype *dchan_idb = NULL,
              *idb = NULL;
    uint bri_channel;

    /*
     * Set all idb variables.
     */
    dchan_idb = init_bri_idb_common(unit);
    if (dchan_idb == NULL)
	return;

    for (bri_channel = 0; bri_channel < BRI_MAX_CHAN; bri_channel++) {
	switch (bri_channel) {
	  case BRI_D_CHAN:
	    idb = dchan_idb;
	    break;
	    
	  case BRI_B1_CHAN:
            idb = dchan_idb->isdn_info->b1_chan_idb;
	    break;
	    
	  case BRI_B2_CHAN:
            idb = dchan_idb->isdn_info->b2_chan_idb;
	    break;
	}

        idb->slot = slot;
        idb->subcont = subcont;
        idb->subunit = subunit;
        idb->m32_channel = bri_m32_map_channel(idb);

        /* 
         * Keep track of the mcn instance 
         */
        idb->devctl = ds;
    }

    /*
     * Intialize the interface
     */
    bri_init(dchan_idb, FALSE);
    /*
     * don't count this init as a reset
     */
    dchan_idb->counters.resets = 0;
    dchan_idb->isdn_info->b1_chan_idb->counters.resets = 0;
    dchan_idb->isdn_info->b2_chan_idb->counters.resets = 0;

    /*
     * save nim type and version.
     */
    dchan_idb->nim_type = nim_get_type(slot);
    dchan_idb->nim_version = nim_get_version(slot);
    
}


/*
 * =====================================================================
 * bri_subsys_init -
 *
 * Description: BRI subsystem initialisation entry point.
 *
 * Parameters:
 * subsys	: ptr to subsystem type.
 *
 * Returns:
 * nothing.
 * 
 * =====================================================================
 */
static void bri_subsys_init (subsystype *subsys)
{
    /*
     * Register the NIM ID for the MCN base card
     */
    reg_add_analyze_mcn_port_adaptor(MCN_PA_BRI_SINGLE,
				     mb_analyze_mcn_port_adaptor,
				     "mb_analyze_mcn_port_adaptor");
    reg_add_analyze_mcn_port_adaptor(MCN_PA_BRI_DUAL,
				     mb_analyze_mcn_port_adaptor,
				     "mb_analyze_mcn_port_adaptor");
    reg_add_test_specific_interface(IDBTYPE_BRI,
				    test_bri_interface,
				    "test_bri_interface");

	/*
	 * Leased Line initialisation registry
	 */
	reg_add_isdn_leased_line(init_bri_leased_line, "init_bri_leased_line");
}

/*
 * BRI subsystem header
 */
#define BRI_MAJVERSION   1
#define BRI_MINVERSION   0
#define BRI_EDITVERSION  1

SUBSYS_HEADER(bri, BRI_MAJVERSION, BRI_MINVERSION, BRI_EDITVERSION,
              bri_subsys_init, SUBSYS_CLASS_DRIVER,
              "seq: mcn",
              "req: mcn");

/*
 * Return TRUE is the BRI interface is in state ACTIVATED. FALSE is in any
 * other state. Added for Leased Line ISDN feature.
 */
boolean is_layer1_current_state_activated (hwidbtype *hwidb)
{
    hwidbtype *dchan_idb;
    mb_instance_t *mb_instance;

	dchan_idb = hwidb->isdn_info->d_chan_idb;  /* make sure it is dchannel*/
	mb_instance = MB_INSTANCE(dchan_idb->slot, dchan_idb->subcont);

	return((mb_instance->current_state[dchan_idb->subunit] >= l1_state_F7) ?
			TRUE : FALSE);
}
