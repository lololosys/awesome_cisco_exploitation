/* $Id: lapb.c,v 3.15.12.11 1996/09/05 01:17:04 snyder Exp $
 * $Source: /release/112/cvs/Xsys/x25/lapb.c,v $
 *------------------------------------------------------------------
 * lapb.c -- Link Access Protocol, Balanced
 *
 * December 1986, Greg Satz
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lapb.c,v $
 * Revision 3.15.12.11  1996/09/05  01:17:04  snyder
 * CSCdi68132:  declare consts in lapb x25 atm_arp tcl
 *              save 248 out of data, 12 from image
 * Branch: California_branch
 *
 * Revision 3.15.12.10  1996/08/28  13:23:06  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.15.12.9  1996/07/30  14:43:23  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.15.12.8  1996/07/23  13:32:29  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.15.12.7  1996/07/19  19:51:44  wmay
 * CSCdi60728:  ISDN dialer holdQ not handled correctly  - Do not call
 * media_protocol_up when lapb comes up and running ppp reliable-link
 * Branch: California_branch
 *
 * Revision 3.15.12.6  1996/06/27  09:47:36  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.15.12.5  1996/06/19  00:23:01  carvalho
 * CSCdi33844:  X25 over B-channel broken
 * Branch: California_branch
 *
 * Reorder initialization for LAPB encapsulation, to prevent failures for
 * hunt group leaders.
 *
 * Revision 3.15.12.4  1996/05/13  14:48:33  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.15.12.3  1996/04/29  16:03:05  gglick
 * CSCdi55677:  LAPB stuck in RNRSENT/REJSENT state
 * Branch: California_branch
 *
 * Revision 3.15.12.2  1996/04/27  06:45:54  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.8.2.3  1996/04/26  00:04:11  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.8.2.2  1996/04/17  00:04:15  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.8.2.1  1996/02/23  21:32:41  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.15.12.1  1996/03/18  22:49:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.2.4  1996/03/16  07:57:11  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.11.2.3  1996/03/13  02:13:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.11.2.2  1996/03/07  11:19:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11.2.1  1996/02/20  21:54:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.15  1996/02/15  02:15:02  gglick
 * CSCdi45199:  X.25 PVC command lost upon reload.
 *         Make sure X.25 doesn't come up before system initialization is
 *         complete.
 *
 * Revision 3.14  1996/02/07  16:17:52  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.13  1996/02/02  17:07:04  atsao
 * CSCdi46647:  Update N1 when MTU, X25 modulo or LAPB compression is
 * changed
 * Incorporate code review comment.
 *
 * Revision 3.12  1996/02/01  06:12:53  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.11  1996/01/22  07:46:33  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.10  1996/01/16  22:53:00  atsao
 * CSCdi46647:  Update N1 when MTU, X25 modulo or LAPB compression is
 * changed
 * Use lapb_update_n1 as a registry instead of a function call.
 *
 * Revision 3.9  1996/01/15  04:48:01  atsao
 * CSCdi46647:  Update N1 when MTU, X25 modulo or LAPB compression is
 * changed
 * Dynamically update N1 value whenever there is configuration change on
 * hardware MTU, LAPB/X25 modulo, or lapb compression.
 *
 * Revision 3.8  1996/01/12  23:29:27  atsao
 * CSCdi44422:  LAPB N1 command not accepted, N1 falls back to default
 * If user has ever configured LAPB N1 value, make sure this N1 is shown
 * in the show interface output, otherwise, update this N1 value to
 * reflect the current configuration.
 *
 * Revision 3.7  1996/01/12  07:53:25  atsao
 * CSCdi46024:  LAPB stops sending RRs
 * When a non-polled S frame or an I-frame is received, we need to reopen
 * the transmit window.
 *
 * Revision 3.6  1995/12/17  18:41:51  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/11/21  20:44:40  turadek
 * CSCdi43841:  add frame relay payload compression feature
 * syncing code to 11.0
 *
 * Revision 3.4  1995/11/20  22:29:08  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  18:06:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:51:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:52:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.13  1995/10/23  19:28:16  gchristy
 * CSCdi42576:  X25: encapulation x25 causes software forced crash
 * Reset interrupt level before returning due to secondary vectors not
 * being set.
 *
 * Revision 2.12  1995/10/16  22:22:51  gglick
 * CSCdi40956:  agsb crashes while configuring x25 encap on ser
 *
 * Revision 2.11  1995/09/23  21:27:39  gglick
 * CSCdi40746:  X.25 and LAPB not working
 *
 * Revision 2.10  1995/09/12  18:48:24  gglick
 * CSCdi38693:  lapb disables priority queuing
 *
 * Revision 2.9  1995/09/09  00:55:12  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.8  1995/08/18  21:42:58  lbustini
 * CSCdi37190:  X.25 encapsulation over BRI interface is failing
 * Fix queueing vectors setup.
 *
 * Revision 2.7  1995/08/10  15:30:27  gglick
 * CSCdi38546:  LAPB can delay sending an X.25 packet
 *
 * Revision 2.6  1995/08/08  03:13:27  atsao
 * CSCdi35191:  Modulo 128 for LAPB encap breaks fragmentation of large
 * packets
 * Encapsulation of large packet fails when lapb modulo 128 is configured.
 * Update N1 value when the link is reset solves the problem.
 *
 * Revision 2.5  1995/07/05  22:18:11  gtaylor
 * CSCdi34592:  PPP Reliable Link broken. Changed lapb's attachment to PPP
 * from taking over and saving the idb->iqueue to taking over the
 * parse_packet vector instead. This also fixes bridging problems with
 * reliable link.
 *
 * Revision 2.4  1995/06/30  00:31:42  gglick
 * CSCdi36571:  T4 disabled message produced when T4 isn't enabled
 *
 * Revision 2.3  1995/06/29  07:35:15  atsao
 * CSCdi34568:  Negative testing configuring enacp lapb and bridging fails
 * Restructure lapb_setup() to validate the existing prohibited features
 * before any context modifications.
 *
 * Revision 2.2  1995/06/14  20:39:17  gglick
 * CSCdi30355:  Unexpected LAPB reset under severe traffic condition
 *         Implement receiver busy (RNR) when interface is congested.
 *
 * Revision 2.1  1995/06/07  23:20:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../ui/parse_util.h"
#include "parser_defs_lapb.h"
#include "parser_defs_x25.h"
#include "address.h"
#include "x25.h"
#include "lapb.h"
#include "logger.h"
#include "../x25/msg_lapb.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "../wan/serial.h"
#include "../wan/dialer.h"
#include "../ip/ip.h"
#include "../if/network.h"
#include "lapb_debug.h"
#include "../parser/parser_defs_compress.h"
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"
#include "lapb_event.h"
#include "../wan/dialer_registry.h"
#include "../wan/ppp.h"


#define LAPB_U_OVERHEAD	2
#define LAPB_U_PF_INX	1
#define LAPB_U_PF_SHIFT	4

/*
 *  define the data that describes the difference between the modes
 *  implemented
 */
STRUCTURE_REF(lapb_mode);
struct lapb_mode_ {
    uchar modulo;
    uchar modulo_mask;
    uchar set_mode_control;	/* this mode setting control */
    uchar mib_entry_reason;	/* MIB reason to cite on mode entry */
    uchar mib_reset_reason;	/* MIB reason to cite on mode reset */
    uchar alt_set_mode_control;	/* the other mode setting control */

    struct {		/* I-frames */
	uchar overhead;
	uchar nr_inx;
	uchar nr_shift;
	uchar pf_inx;
	uchar pf_shift;
	uchar ns_inx;
	uchar ns_shift;
    } i;

    struct {		/* Supervisory frames */
	uchar overhead;
	uchar control_mask;
	uchar nr_inx;
	uchar nr_shift;
	uchar pf_inx;
	uchar pf_shift;
    } s;

    struct {		/* FRMR frames */
	uchar i_field_cnt;
	uchar rej_ctl_cnt;
	uchar vr_inx;
	uchar vr_shift;
	uchar c_r_inx;
	uchar c_r_shift;
	uchar vs_inx;
	uchar vs_shift;
	uchar cause_inx;
    } frmr;
};

static void lapb_change_mtu(hwidbtype *, idbtype *, int);

static lapb_mode	lapb_8 = {
    8, 0x07, LAPB_SABM,			    /* general mode parameters */
    LAPB_MIBREASON_ABM, LAPB_MIBREASON_RESET,
    LAPB_SABME,
    {LAPB_OVERHEAD_8, 1, 5, 1, 4, 1, 1},    /* I-frame parameters */
    {2, 0x0F, 1, 5, 1, 4},		    /* Supervisory frame parameters */
    {3, 1, 3, 5, 3, 4, 3, 1, 4}		    /* FRMR parameters */
};
static lapb_mode	lapb_128 = {
    128, 0x7f, LAPB_SABME,		    /* general mode parameters */
    LAPB_MIBREASON_ABME, LAPB_MIBREASON_E_RESET,
    LAPB_SABM,
    {LAPB_OVERHEAD_128, 2, 1, 2, 0, 1, 1},  /* I-frame parameters */
    {3, 0xFF, 2, 1, 2, 0},		    /* Supervisory frame parameters */
    {5, 2, 5, 1, 5, 0, 4, 1, 6}		    /* FRMR parameters */
};

#define SET_MODE(idb, mode)						\
		((mode) = ((idb)->lapb_modulo == LAPB_MODULO_128) ?	\
			&lapb_128 : &lapb_8)

/*
 *  define an evaluation function that determines if an I-frame can be sent--
 *  the transmit state is valid, we're not polling for a response, the
 *  window is open and we don't have a hardware outage being timed
 */
static inline boolean lapb_can_tx_i (register hwidbtype *idb)
{
    return((idb->lapb_tx_state == LAPB_TX_NORMAL) &&
	   (! idb->lapb_poll) &&
	   (QUEUESIZE(&idb->lapb_unackq) < idb->lapb_k) &&
	   (! TIMER_RUNNING(idb->lapb_t3_timer)));
}

/*
 *  define evaluation functions for the receive machine:
 *	enter the congested state (a few I-frames can still be accepted),
 *	dropping I-frames
 *	and exit the congested state
 *
 *  for a default input queue limit of 75:
 *	RNR will be signalled on receipt of the 72/75 (unprocessed) I-frame
 *	additional I-frames will be accepted until 75/75 (which, when
 *	  discarded, implies the input queue will top out at 74/75)
 *	and RNR will be exited (from the LAPB timer task) at 64/75 (or less)
 */
static inline boolean lapb_rx_congestion_enter (register hwidbtype *idb)
{
    return(idb->input_qcount >= (idb->input_defqcount - 3));
}
static inline boolean lapb_rx_drop_iframe (register hwidbtype *idb)
{
    /*
     *  NB: The interface can be configured with a bizzare input queue size!
     *
     *	We need to be careful not to drop an I-frame if we're only allowed
     *	one receive buffer; if so, the driver will drop any additional
     *  frames and there's nothing we can (or should) do about it.
     */
    return((idb->input_qcount >= idb->input_defqcount) &&
	   (idb->input_qcount > 1));
}
static inline boolean lapb_rx_congestion_exit (register hwidbtype *idb)
{
    /*
     *  NB: The interface can be configured with a bizzare input queue size!
     *
     *	Make sure we exit the congested state if the queue count is zero.
     */
    return((idb->input_qcount < (idb->input_defqcount - 10)) ||
	   (idb->input_qcount == 0));
}

/* externs for initing lapb compression registries */


int lapb_tf;				/* timer process' pid */
#ifdef NOBODY_READS_ME
int nlapb;				/* number of LAPB encaps */
#endif
queuetype lapbidbQ;
static tinybool timer_up = FALSE;

/*
 *  lapb_encaps_nvram_write()
 *  Generate the LAPB encapsulation command
 */
static void lapb_encaps_nvram_write (parseinfo *csb)
{
    idbtype *swidb = csb->interface;

    nv_write(TRUE, "encapsulation lapb");

    nv_add(IS_DCE(swidb->hwptr, lapb_dce), " dce");

    if (swidb->hwptr->lapb_protocol != LINK_IP) {
	if (swidb->hwptr->lapb_protocol == LINK_ILLEGAL)
	    nv_add(TRUE, " multi");
	else
	    nv_add(TRUE, " %s", print_protocol(swidb->hwptr->lapb_protocol));
    }
}

/*
 *  lapb_init()
 *  Power-up LAPB initialization
 */

void lapb_init (void)
{
    lapb_tf = 0;
#ifdef NOBODY_READS_ME
    nlapb = 0;
#endif
    queue_init(&lapbidbQ, 0);

    /*
     * Set up debugging flags
     */
    lapb_debug_init();

    /*
     * Register some functions
     */
    reg_add_clear_counters(lapb_clearcounters, "lapb_clearcounters");
    reg_add_media_serial_setup(ET_LAPB, lapb_setup, "lapb_setup");
    reg_add_encaps_nvram_write(ET_LAPB, lapb_encaps_nvram_write,
				"lapb_encaps_nvram_write");
    reg_add_media_copy_encap_fields(ET_LAPB, copy_lapb_fields,
					   "copy_lapb_fields");
    reg_add_media_fix_serial_vectors(ET_LAPB, 
	 lapb_check_output_vectors, "lapb_check_output_vectors");
    reg_add_media_change_mtu(lapb_change_mtu, "lapb_change_mtu");
    reg_add_interface_cfg_modified(lapb_check_output_vectors,
				"lapb_check_output_vectors");
    reg_add_lapb_setup_encap(ET_PPP, lapb_ppp_setup, "lapb_ppp_setup");
    reg_add_lapb_remove_encap(ET_PPP, lapb_ppp_close, "lapb_ppp_close");
    reg_add_lapb_update_n1(lapb_update_n1, "lapb_update_n1");

    /*
     * Initialize lapb parser support
     */
    lapb_parser_init();

    /* the default stuff */
    reg_add_default_compress_squish(
	(service_compress_squish_type)default_compress, "default compress");
    reg_add_default_compress_explode(
	(service_compress_explode_type)default_decompress, "default decompress");
    /* the predictor stuff */
    reg_add_compress_squish(
	COMP_RAND, (service_compress_squish_type)rand_compress, "predictor compress");
    reg_add_compress_explode(
	COMP_RAND, (service_compress_explode_type)rand_decompress, "predictor decompress");
    /* the stac stuff */
    reg_add_compress_squish(
	COMP_STAC, (service_compress_squish_type)stac_compress, "stac compress");
    reg_add_compress_explode(
	COMP_STAC, (service_compress_explode_type)stac_decompress, "stac decompress");
}


/*
 * lapb_vencapsulate
 * Workhorse LAPB encapsulation routine
 */

boolean lapb_vencapsulate (
    paktype *pak,
    long address)
{
    hwidbtype *idb;
    ushort frame_type;
    lapb_mode *mode;

    idb = pak->if_output->hwptr;

    if (!idb->lapb_upstate)
	return(FALSE);

    SET_MODE(idb, mode);

    switch (idb->enctype) {
      case ET_LAPB:
	/*
	 *  encapsulate the network layer protocol
	 */

	if ((frame_type = link2serial(pak->linktype)) == LINK_ILLEGAL)
	    return(FALSE);

	pak->encsize = 0;

	if (idb->lapb_protocol == LINK_ILLEGAL) {
	    /*
	     *  this is a multiprotocol LAPB link--insert the frame type
	     */
	    pak->encsize += LAPB_PID_BYTES;
	    pak->datagramsize  += LAPB_PID_BYTES;
	    pak->datagramstart -= LAPB_PID_BYTES;
	    PUTSHORT(pak->datagramstart, frame_type);
	} else if (frame_type != link2serial(idb->lapb_protocol)) {
	    return(FALSE);
	}

	/*
	 *  encapsulation fails if size is greater than allowed
	 *  Notes:
	 *	We need to account for the 16 bits of CRC the driver will
	 *	append.
	 *	X.25 encapsulations will perform fragmentation, so only
	 *	LAPB encapsulations get checked.
	 */
	if (((mode->i.overhead + pak->datagramsize) * 8 + LAPB_CRC_BITS) >
	    idb->lapb_n1)
	    return(FALSE);

	/* FALL THROUGH */
      case ET_X25:
      case ET_PPP:
	/*
	 *  add the LAPB overhead to the packet
	 */
	pak->encsize += mode->i.overhead;
	pak->datagramsize += mode->i.overhead;
	pak->datagramstart -= mode->i.overhead;
	break;

      default:
	return(FALSE);
    }

    pak->enctype = idb->enctype;
    return(TRUE);
}

/*
 *  lapb_output()
 *  Queue a packet for transmission.  Frames are "pulled" by the driver
 *  when the interface is able to send, so outgoing buffers are either
 *  queued using the standard interface output queues, or placed in a
 *  LAPB-specific queue (required for X.25 because layer 3 packets must not
 *  be re-ordered).
 */

boolean lapb_output (
    hwidbtype *idb,
    paktype *pak,
    enum HEADTAIL which)
{
    if (which == HEAD) {
	/*
	 *  This is a real dangerous thing, but such is life currently.
	 *  The frame was handed off to the driver, but the driver
	 *  is giving it back to us because of resource problems--
	 *  save it in an idb field to ensure that it's the next frame
	 *  transmitted; this avoids re-ordering the output stream.
	 */

	if (idb->lapb_requeue_pak) {
	    datagram_done(idb->lapb_requeue_pak);	/* frame lost */
	    idb->output_qcount--;
	}

	idb->lapb_requeue_pak = pak;

	/*
	 *  do NOT nudge the driver (the MCI driver will recurse,
	 *  overflowing the stack), but bump the output queue
	 *  count so the driver will try again
	 */
	idb->output_qcount++;
	return(FALSE);
    }

    /*
     *  the dialer may give us a raw/out-of-band frame to send without
     *  invoking the encapsulation function (which will prevent any other
     *  use of ET_HDLC)
     */
    if ((pak->enctype == ET_HDLC) && is_ddr(idb)) {
	/*
	 * V25.bis signaling packet
	 */
	if (! pak_enqueue(&idb->lapb_u_s_q, pak)) {
	    datagram_done(pak);
	    return(FALSE);
	}

	idb->output_qcount++;
	(*idb->soutput)(idb);
	return(TRUE);
    }

    switch (idb->lapb_state) {
      case LAPB_CONNECTED:
	/*
	 *  invoke the secondary enqueue routine to evaluate the
	 *  packet and place it on the appropriate queue
	 */
	if ((*idb->lapb_oqueue)(idb, pak, which)) {
	    /*
	     *  if we're able to send an I-frame, nudge the driver
	     */
	    if (lapb_can_tx_i(idb)) {
		(*idb->soutput)(idb);
	    } else {
		idb->lapb_tx_deferrals++;
	    }
	}

	break;

      case LAPB_DISCONNECTED:
      case LAPB_SABM_SENT:
      case LAPB_FRMR_SENT:
      case LAPB_DISC_SENT:
      default:
	/*
	 * All other states we toss packet
	 */
	datagram_done(pak);
    }

    return(FALSE);
}

/*
 * lapb_u_frame
 * build and queue a LAPB unnumbered frame (except XID); note that a FRMR
 * takes its data field from the idb->lapb_frmr_txdata.
 */

void lapb_u_frame (
    register hwidbtype *idb,
    register paktype *pak,
    uchar frame_id,
    boolean poll_final)
{
    lapb_mode *mode;
    int size;

    /*
     *  if this is a mode setting command, use any held-off parameters
     */
    if (frame_id == LAPB_SABM || frame_id == LAPB_SABME) {
	idb->lapb_modulo = idb->lapb_admin_modulo;
	idb->lapb_k = idb->lapb_admin_k;
	/*
	 * Update lapb_n1 only if user has never configured N1 value.
	 */
	if (!idb->lapb_override_def_n1)
	    idb->lapb_n1 = LAPB_ADMIN_N1(idb);
    }

    SET_MODE(idb, mode);

    size = LAPB_U_OVERHEAD +
		((frame_id == LAPB_FRMR) ? mode->frmr.i_field_cnt : 0);

    /*
     * if available, recycle the buffer given
     */
    if (pak) {
	clear_if_input(pak);
	pak->datagramstart = pak->network_start - LAPB_U_OVERHEAD;
    } else {
	pak = getbuffer(size);

	if (pak == NULL) {
	    return;
	}
    }

    /*
     *  construct the U frame
     */

    pak->if_output = idb->firstsw;

    pak->datagramsize = size;
    memset(pak->datagramstart, 0, size);

    switch (frame_id) {
      case LAPB_SABM:
      case LAPB_SABME:
	frame_id = mode->set_mode_control;
	TIMER_STOP(idb->lapb_col_timer);
	/* FALL THROUGH */
      case LAPB_DISC:
	pak->datagramstart[LAPB_ADDRESS_INX] = LAPB_CMDADDR(idb);
	break;

      case LAPB_DM:
      case LAPB_UA:
	pak->datagramstart[LAPB_ADDRESS_INX] = LAPB_RSPADDR(idb);
	break;
      case LAPB_FRMR:
	pak->datagramstart[LAPB_ADDRESS_INX] = LAPB_RSPADDR(idb);
	bcopy(&idb->lapb_frmr_txdata, &pak->datagramstart[LAPB_U_OVERHEAD],
		mode->frmr.i_field_cnt);
	break;
      default:
	/* blech */
	datagram_done(pak);
	return;
    }

    pak->datagramstart[LAPB_CONTROL_INX] = frame_id;

    if (poll_final)
	pak->datagramstart[LAPB_U_PF_INX] |= 1 << LAPB_U_PF_SHIFT;

    /*
     *  queue the frame for sending
     */
    if (pak_enqueue(&idb->lapb_u_s_q, pak)) {
	idb->output_qcount++;
	(*idb->soutput)(idb);
    } else {
	datagram_done(pak);
    }
}

/*
 * lapb_s_frame
 * build and queue a LAPB supervisory frame
 */

void lapb_s_frame (
    register hwidbtype *idb,
    register paktype *pak,
    boolean command,
    boolean poll_final)
{
    lapb_mode *mode;
    uchar frame_id;

    SET_MODE(idb, mode);

    /*
     * if available, recycle the buffer given
     */
    if (pak) {
	clear_if_input(pak);
	pak->datagramstart = pak->network_start - mode->s.overhead;
    } else {
	pak = getbuffer(mode->s.overhead);

	if (pak == NULL) {
	    return;
	}
    }

    /*
     *  determine the frame type to send
     */
    switch (idb->lapb_rx_state) {
      case LAPB_RX_REJ_SENT:
	frame_id = LAPB_REJ;
	break;
      case LAPB_RX_CONGESTED:
	frame_id = LAPB_RNR;
	break;
      case LAPB_RX_NORMAL:
      default:		/* keep the compiler happy */
	frame_id = LAPB_RR;
	break;
    }

    pak->if_output = idb->firstsw;

    pak->datagramsize = mode->s.overhead;
    memset(pak->datagramstart, 0, pak->datagramsize);

    pak->datagramstart[LAPB_ADDRESS_INX] =
		command ? LAPB_CMDADDR(idb) : LAPB_RSPADDR(idb);
    pak->datagramstart[LAPB_CONTROL_INX] = frame_id;
    if (poll_final)
	pak->datagramstart[mode->s.pf_inx] |= 1 << mode->s.pf_shift;
    pak->datagramstart[mode->s.nr_inx] |= idb->lapb_vr << mode->s.nr_shift;
    idb->lapb_nr = idb->lapb_vr;

    /*
     *  queue the frame for sending
     */
    if (pak_enqueue(&idb->lapb_u_s_q, pak)) {
	idb->output_qcount++;
	(*idb->soutput)(idb);
    } else {
	datagram_done(pak);
    }

    return;
}

/*
 * lapb_form_i_frame_inline
 * build a LAPB information frame
 */

static inline void lapb_form_i_frame_inline (
    register hwidbtype *idb,
    register paktype *pak)
{
    lapb_mode *mode;

    SET_MODE(idb, mode);

    memset(pak->datagramstart, 0, mode->i.overhead);
    pak->datagramstart[LAPB_ADDRESS_INX] = LAPB_CMDADDR(idb);
    pak->datagramstart[LAPB_CONTROL_INX] = LAPB_IFRAME;
    pak->datagramstart[mode->i.ns_inx] |= idb->lapb_vs << mode->i.ns_shift;
    idb->lapb_vs = ++idb->lapb_vs & mode->modulo_mask;
    pak->datagramstart[mode->i.nr_inx] |= idb->lapb_vr << mode->i.nr_shift;
    idb->lapb_nr = idb->lapb_vr;
}

/*
 *  lapb_pull_frame
 *  get a LAPB frame to send
 */

paktype *lapb_pull_frame (hwidbtype *idb)
{
    register paktype *pak;
    paktype *dup_pak;
    static char *queue_failed = "unable to queue buffer";
    lapb_mode *mode;

    SET_MODE(idb, mode);

    /*
     *  first priority--send any frame the driver pulled but was unable
     *  to send; we must do this to avoid re-ordering the tansmit stream
     */
    pak = idb->lapb_requeue_pak;

    /*
     *  look for a frame to send
     *  Unnumbered/Supervisory frames have precedence over I-frames, but
     *  that queue is checked later because severe I-frame processing
     *  problems may cause a link reset (U frame).
     */
    if ((pak == NULL) &&
	QUEUEEMPTY(&idb->lapb_u_s_q) &&
	lapb_can_tx_i(idb)) {
	/*
	 *  see if we have an Information frame to send--
	 *  check the retransmit queue first
	 */
	if (! QUEUEEMPTY(&idb->lapb_retxq)) {
	    pak = pak_dequeue(&idb->lapb_retxq);
	    idb->output_qcount--;

	    /*
	     *  retransmitted frames just might have an outstanding
	     *  reference in the driver, which must not be updated;
	     *  if we don't hold the only buffer reference, try to
	     *  duplicate the frame
	     */
	    if (pak->refcount != 1) {
		if ((dup_pak = pak_duplicate(pak))) {
		    datagram_done(pak);
		    pak = dup_pak;
		} else {
		    /*
		     *  We can't re-transmit this I-frame.  Commit
		     *  the minor sin of not sending the frame; simply
		     *  queue it to the unacknowledged I-frame queue.
		     */

		    if (lapb_debug) {
			errmsg(&msgsym(NOBUF, LAPB), idb->hw_namestring,
				"re-transmit I-frame");
		    }

		    if (pak_enqueue(&idb->lapb_unackq, pak)) {
			idb->lapb_vs = ++idb->lapb_vs & mode->modulo_mask;
			TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
		    } else {
			datagram_done(pak);
			lapb_reset(idb, queue_failed);
			lapb_u_frame(idb, pak, LAPB_SABM, TRUE);
			idb->lapb_state = LAPB_SABM_SENT;
			TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
			NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP,
					LAPB_MIBREASON_OTHER)
		    }

		    pak = NULL;
		}
	    }

	    /*
	     *  The MIP card may introduce extra encaps bytes,
	     *  which must be eliminated on re-transmission
	     */
	    if (pak && idb->cache_board_encap) {
		ushort temp_buffer, length;

		length = (*idb->cache_board_encap)(idb, &temp_buffer, 
						   sizeof(ushort));
		pak->datagramstart += length;
		pak->datagramsize -= length;
	    }
	} else {
	    /*
	     *  invoke the secondary output dequeue routine to get an I-frame
	     */
	    pak = (*idb->lapb_oqueue_dequeue)(idb);

	    /*
	     * COMP: compress frame if required
	     */
	    if (pak && idb->comp_db && !idb->ppp_lapb &&
		(idb->comp_db->comp_configed != COMP_NOT_ENABLED)) {

		/* compress routine frees the passed packet */
		pak = reg_invoke_compress_squish(idb->comp_db->comp_configed,
						 idb, pak);

		if ((pak == NULL) && lapb_debug) {
		    errmsg(&msgsym(NOBUF, LAPB), idb->hw_namestring,
			   "compress I-frame");
		}
	    }

	    if (pak) {
		idb->lapb_iframessent++;
	    }
	}

	if (pak) {
	    lapb_form_i_frame_inline(idb, pak);

	    /*
	     *  queue a new instance of the buffer to the unack. I-frame queue
	     */
	    pak_lock(pak);
	    if (pak_enqueue(&idb->lapb_unackq, pak)) {
		TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
	    } else {
		datagram_done(pak);
		datagram_done(pak);
		pak = NULL;
		lapb_reset(idb, queue_failed);
		lapb_u_frame(idb, pak, LAPB_SABM, TRUE);
		idb->lapb_state = LAPB_SABM_SENT;
		TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
		NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP, LAPB_MIBREASON_OTHER)
	    }
	}
    }

    /*
     *  get any queued Unnumbered or Supervisory frame to send
     */
    if ((pak == NULL) && (! QUEUEEMPTY(&idb->lapb_u_s_q))) {
	pak = pak_dequeue(&idb->lapb_u_s_q);
	idb->output_qcount--;

	/*
	 *  determine any frame type counter
	 */
	if (pak->enctype == ET_HDLC) {
	    ; /* EMPTY */
	} else if (LAPB_ISUFRAME(pak)) {
	    switch (pak->datagramstart[LAPB_CONTROL_INX] & LAPB_U_CONTROL_M) {
	      case LAPB_SABM:
	      case LAPB_SABME:
		idb->lapb_sabmssent++;
		break;
	      case LAPB_DISC:
		idb->lapb_discssent++;
		break;
	      case LAPB_FRMR:
		idb->lapb_frmrssent++;
		break;
	    }
	} else if (LAPB_ISSFRAME(pak)) {
	    switch (pak->datagramstart[LAPB_CONTROL_INX] &
		    mode->s.control_mask) {
	      case LAPB_REJ:
		idb->lapb_rejssent++;
		break;
	      case LAPB_RNR:
		idb->lapb_rnrssent++;
		break;
	      default:
		break;
	    }
	}
    }

    if (pak) {
	/*
	 *  LAPB frames must not be prioritized below this layer
	 */
	pak->flags &= ~PAK_PRIORITY;

	/*
	 *  some drivers perform different functions based on the linktype;
	 *  set all frames to a common value
	 */
	pak->linktype = LINK_ILLEGAL;

	/*
	 *  skip any debug printing or board encapsulation if we
	 *  previously gave the frame to the driver only to get it
	 *  shoved back at us
	 */
	if (idb->lapb_requeue_pak) {
	    idb->lapb_requeue_pak = NULL;

	    /*
	     *  if we've got a deferred packet, decrement the falsified
	     *  output queue count
	     */
	    idb->output_qcount--;
	} else {
	    if (lapb_debug && (pak->enctype != ET_HDLC)) {
		lapb_prframe(idb, pak, 'O');
	    }

	    /*
	     *  add any lower level encapsulation
	     */
	    if (idb->board_encap) {
		(void)(*idb->board_encap)(pak, idb);
	    }
	}
    }

    return(pak);
}

/*
 * lapb_prstate
 * Return pointer to string containing LAPB state(s).
 */

char *lapb_prstate (register hwidbtype *idb)
{
    static char *bad_state = "*BAD*";
    char *p;

    p = "UNKNOWN";
    switch (idb->lapb_state) {
      case LAPB_DISCONNECTED:
	p = "DISCONNECT";
	break;
      case LAPB_SABM_SENT:
	p = "SABMSENT";
	break;
      case LAPB_CONNECTED:
	if ((idb->lapb_tx_state != LAPB_TX_NORMAL) &&
	    (idb->lapb_tx_state != LAPB_TX_REMOTE_BUSY)) {
	    p = bad_state;
	    break;
	}

	switch (idb->lapb_rx_state) {
	  case LAPB_RX_NORMAL:
	    p = (idb->lapb_tx_state == LAPB_TX_NORMAL) ?
		"CONNECT" : "BUSY";
	    break;
	  case LAPB_RX_REJ_SENT:
	    p = (idb->lapb_tx_state == LAPB_TX_NORMAL) ?
		"REJSENT" : "BUSY/REJSENT";
	    break;
	  case LAPB_RX_CONGESTED:
	    p = (idb->lapb_tx_state == LAPB_TX_NORMAL) ?
		"RNRSENT" : "BUSY/RNRSENT";
	    break;
	  default:
	    p = bad_state;
	    break;
	}
	break;
      case LAPB_FRMR_SENT:
	p = "ERROR";
	break;
      case LAPB_DISC_SENT:
	p = "DISCSENT";
	break;
      default:
	p = bad_state;
	break;
    }

    return p;
}

#define LAPB_COMMAND 'c'
#define LAPB_RESPONSE 'r'
#define LAPB_EITHER '-'
#define LAPB_POLL 'P'
#define LAPB_FINAL 'F'
static const char bad_addr[] = " BAD-ADDR";
static const char bad_ctrl[] = "ILLEGAL";
static const char too_short[] = " too short";
static const char too_long[] = " too long";
/*
 * lapb_prframe
 * Print a description of a LAPB frame.
 */

void lapb_prframe (
    register hwidbtype *idb,
    register paktype *pak,
    char io)
{
    lapb_mode *mode;
    const char *type;		/* control field description string */
    const char *crPF;	/* command/response/Poll/Final string */
    char must_be;	/* address restriction (command/response/either) */
    boolean poll_final;	/* P/F bit value */
    char remainder[40];	/* remaining data description--N(S)/N(R)/FRMR data */
    const char *len_error;	/* too short/too long */

    SET_MODE(idb, mode);
    remainder[0] = '\0';
    len_error = "";

    type = bad_ctrl;
    must_be = LAPB_EITHER;
    poll_final = 0;

    if (pak->datagramsize < LAPB_CONTROL_INX + 1) {
	len_error = too_short;
    } else if (LAPB_ISIFRAME(pak)) {
	if (pak->datagramsize < mode->i.overhead)
	    len_error = too_short;
	else if (pak->datagramsize > ((idb->lapb_n1 - LAPB_CRC_BITS) / 8))
	    len_error = too_long;

	type = "IFRAME";

	must_be = LAPB_COMMAND;
	poll_final = pak->datagramstart[mode->i.pf_inx] &
		(1 << mode->i.pf_shift);
	sprintf(remainder, " %d %d",
		(pak->datagramstart[mode->i.ns_inx] >> mode->i.ns_shift) &
		 mode->modulo_mask,
		(pak->datagramstart[mode->i.nr_inx] >> mode->i.nr_shift) &
		 mode->modulo_mask);
    } else if (LAPB_ISSFRAME(pak)) {
	if (pak->datagramsize < mode->s.overhead)
		len_error = too_short;
	else if (pak->datagramsize > mode->s.overhead)
		len_error = too_long;

	switch (pak->datagramstart[LAPB_CONTROL_INX] & mode->s.control_mask) {
	  case LAPB_RR:
	    type = "RR";
	    break;
	  case LAPB_REJ:
	    type = "REJ";
	    break;
	  case LAPB_RNR:
	    type = "RNR";
	    break;
	  default:
	    type = bad_ctrl;
	    break;
	}

	must_be = LAPB_EITHER;
	poll_final = pak->datagramstart[mode->s.pf_inx] &
		(1 << mode->s.pf_shift);
	sprintf(remainder, " %d",
		(pak->datagramstart[mode->s.nr_inx] >> mode->s.nr_shift) &
		 mode->modulo_mask);
    } else if (LAPB_ISUFRAME(pak)) {
	if (pak->datagramsize < LAPB_U_OVERHEAD)
	    len_error = too_short;
	else if (pak->datagramsize > LAPB_U_OVERHEAD)
	    len_error = too_long;

	switch (pak->datagramstart[LAPB_CONTROL_INX] & LAPB_U_CONTROL_M) {
	  case LAPB_DM:
	    type = "DM";
	    must_be = LAPB_RESPONSE;
	    break;
	  case LAPB_SABM:
	    type = "SABM";
	    must_be = LAPB_COMMAND;
	    break;
	  case LAPB_DISC:
	    type = "DISC";
	    must_be = LAPB_COMMAND;
	    break;
	  case LAPB_UA:
	    type = "UA";
	    must_be = LAPB_RESPONSE;
	    break;
	  case LAPB_SABME:
	    type = "SABME";
	    must_be = LAPB_COMMAND;
	    break;
	  case LAPB_FRMR:
	    type = "FRMR";
	    must_be = LAPB_RESPONSE;

	    if (pak->datagramsize ==
		(LAPB_U_OVERHEAD + mode->frmr.i_field_cnt))
		len_error = "";
	    else if (pak->datagramsize <
			(LAPB_U_OVERHEAD + mode->frmr.i_field_cnt))
		len_error = too_short;
	    else
		len_error = too_long;

	    sprintf(remainder, " %#x %d %d %c %s%s%s%s",
		(mode->frmr.rej_ctl_cnt == 2) ?
			(pak->datagramstart[LAPB_U_OVERHEAD] << 8) |
			 pak->datagramstart[LAPB_U_OVERHEAD + 1] :
			pak->datagramstart[LAPB_U_OVERHEAD],
		(pak->datagramstart[mode->frmr.vs_inx] >>
			mode->frmr.vs_shift) & mode->modulo_mask,
		(pak->datagramstart[mode->frmr.vr_inx] >>
			mode->frmr.vr_shift) & mode->modulo_mask,
		(pak->datagramstart[mode->frmr.c_r_inx] &
			(1 << mode->frmr.c_r_shift)) ?
			LAPB_RESPONSE : LAPB_COMMAND,
		(pak->datagramstart[mode->frmr.cause_inx] & LAPB_FRMR_W) ?
			"W" : "",
		(pak->datagramstart[mode->frmr.cause_inx] & LAPB_FRMR_X) ?
			"X" : "",
		(pak->datagramstart[mode->frmr.cause_inx] & LAPB_FRMR_Y) ?
			"Y" : "",
		(pak->datagramstart[mode->frmr.cause_inx] & LAPB_FRMR_Z) ?
			"Z" : "");
	    break;
	  case LAPB_XID:
	    type = "XID";
	    must_be = LAPB_EITHER;

	    if (pak->datagramsize < LAPB_U_OVERHEAD)
		len_error = too_short;
	    else if (pak->datagramsize > ((idb->lapb_n1 - LAPB_CRC_BITS) / 8))
		len_error = too_long;
	    else
		len_error = "";
	    break;
	  default:
	    type = bad_ctrl;
	    must_be = LAPB_EITHER;
	    break;
	}

	poll_final =
	    pak->datagramstart[LAPB_U_PF_INX] & (1 << LAPB_U_PF_SHIFT);
    }

    /*
     *  determine if the frame is a c(ommand), r(esponse), P(oll) or F(inal)
     */
    if (pak->datagramstart[LAPB_ADDRESS_INX] ==
	((io == 'O') ? LAPB_CMDADDR(idb) : LAPB_RSPADDR(idb))) {
	if (must_be == LAPB_RESPONSE)
	    crPF = " (C/ERR)";
	else if (poll_final)
	    crPF = " P";
	else if (must_be == LAPB_EITHER)
	    crPF = " (C)";
	else
	    crPF = "";
    } else if (pak->datagramstart[LAPB_ADDRESS_INX] ==
	((io == 'O') ? LAPB_RSPADDR(idb) : LAPB_CMDADDR(idb))) {
	if (must_be == LAPB_COMMAND)
	    crPF = " (R/ERR)";
	else if (poll_final)
	    crPF = " F";
	else if (must_be == LAPB_EITHER)
	    crPF = " (R)";
	else
	    crPF = "";
    } else
	crPF = bad_addr;

    /*
     * generate the frame description:
     * "<if>: LAPB {I|O} <state> (<len>[ error]) <frame-type> {c|r|P|F/error}
     *		[remaining information]"
     */
    buginf("\n%s: LAPB %c %s (%d%s) %s%s%s",
	   idb->hw_namestring, io, lapb_prstate(idb),
	   pak->datagramsize, len_error, type, crPF, remainder);
}

/*
 * lapb_validcmdframe
 * Returns TRUE iff we have a valid command frame type
 */

boolean lapb_validcmdframe (
    hwidbtype *idb,
    register paktype *pak)
{
    lapb_mode *mode;

    SET_MODE(idb, mode);

    if (LAPB_ISIFRAME(pak))
	return(TRUE);
    else if (LAPB_ISUFRAME(pak)) {
	switch (pak->datagramstart[LAPB_CONTROL_INX] & LAPB_U_CONTROL_M) {
	  case LAPB_SABM:
	  case LAPB_SABME:
	  case LAPB_DISC:
	    return(TRUE);
	}
    } else if (LAPB_ISSFRAME(pak)) {
	switch (pak->datagramstart[LAPB_CONTROL_INX] & mode->s.control_mask) {
	  case LAPB_RNR:
	  case LAPB_RR:
	  case LAPB_REJ:
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * lapb_start
 * Start LAPB connect and ack if necessary.
 * Called when entering CONNECT state.
 */

void lapb_start (
    register hwidbtype *idb,
    register paktype *pak,
    uchar ack,
    uchar poll,
    uchar mib_reason)
{
    lapb_mode *mode;
    paktype *qpak;

    SET_MODE(idb, mode);

    /*
     * free any queued I-frames
     */
    while ((qpak = pak_dequeue(&idb->lapb_retxq))) {
	idb->output_qcount--;
	datagram_done(qpak);
    }

    while ((qpak = pak_dequeue(&idb->lapb_holdq))) {
	idb->output_qcount--;
	datagram_done(qpak);
    }

    while ((qpak = pak_dequeue(&idb->lapb_unackq))) {
	datagram_done(qpak);
    }

    /*
     *  holdq_reset() will zero the interface output_qcount; reconstruct
     *  the value after invoking it
     */
    holdq_reset(idb);
    idb->output_qcount = QUEUESIZE(&idb->lapb_u_s_q) +
	((idb->lapb_requeue_pak) ? 1 : 0);

    idb->lapb_vs = 0;
    idb->lapb_vr = 0;
    idb->lapb_rvr = 0;
    idb->lapb_nr = 0;
    TIMER_STOP(idb->lapb_t1_timer);
    if (idb->lapb_t4)
	TIMER_START(idb->lapb_t4_timer, idb->lapb_t4 * ONESEC);
    idb->lapb_retcnt = 0;

    /*
     *  COMP: restart compression if required
     */
    if (idb->comp_db && !idb->ppp_lapb &&
	(idb->comp_db->comp_configed != COMP_NOT_ENABLED))
	compress_restart(idb);

    idb->lapb_state = LAPB_CONNECTED;
    idb->lapb_rx_state = LAPB_RX_NORMAL;
    idb->lapb_tx_state = LAPB_TX_NORMAL;

    NEW_MIB_STATE(idb, LAPB_MIBMODE_CON, mib_reason);

    idb->lapb_upstate = TRUE;
    idb->lapb_poll = FALSE;

    if (ack)
	lapb_u_frame(idb, pak, LAPB_UA, poll);

    /*
     * Bring the link up for routing protocols
     */
    if (is_x25(idb)) {
	if (idb->x25_flags & X25_LINKRESTART)
	    x25_cstate(idb);
    } else {
	idb->reason = "LAPB connected";
	reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
	if (idb->ppp_lapb == FALSE)
	    reg_invoke_media_protocol_up(idb, LINK_ALL);
    }
}

/*
 * lapb_input
 * Handle incoming LAPB frames.
 * Called from device interrupt.
 */

void lapb_input (
    register hwidbtype *idb,
    paktype *pak)
{
    lapb_mode *mode;
    uchar control;
    boolean cmd;
    register boolean poll;
    uchar reason;
    int nr;
    int ns;
    leveltype level;
    uint linktype;
    boolean cant_send;


    SET_MODE(idb, mode);

    if (! timer_up ||
	! pak->if_input ||
	idb->state != IDBS_UP) {
	if (pak->if_input == NULL)
	  errmsg(&msgsym(NOINPIDB, LAPB));
	datagram_done(pak);
   	return;
    }

    SET_PAK_DIALER_INPUT(pak);

    if (lapb_debug)
	lapb_prframe(idb, pak, 'I');

    /*
     *	per 1988 CCITT Recommnedation X.25 sections 2.3.5.3 and 2.4.2,
     *	ignore frames that are too short and frames that don't have a
     *	valid address
     */
    if ((pak->datagramsize < (LAPB_CONTROL_INX + 1)) ||
	((pak->datagramstart[LAPB_ADDRESS_INX] != LAPB_ADDRESSA) &&
	 (pak->datagramstart[LAPB_ADDRESS_INX] != LAPB_ADDRESSB))) {
	/*
	 * If PPP-LAPB is running, notify PPP of the invalid LAPB address
	 */
	if (idb->ppp_lapb) {
	    reg_invoke_lapb_ppp_event_notify(idb);
	}
	datagram_done(pak);
	return;
    }

    if (IS_DCE(idb, lapb_dce)) {
	if (idb->loopback)
	    cmd = pak->datagramstart[LAPB_ADDRESS_INX] == LAPB_ADDRESSA;
	else
	    cmd = pak->datagramstart[LAPB_ADDRESS_INX] == LAPB_ADDRESSB;
    } else {
	if (idb->loopback)
	    cmd = pak->datagramstart[LAPB_ADDRESS_INX] == LAPB_ADDRESSB;
	else
	    cmd = pak->datagramstart[LAPB_ADDRESS_INX] == LAPB_ADDRESSA;
    }

    /*
     *  parse the relevent information
     */
    if (LAPB_ISIFRAME(pak)) {
	control = LAPB_IFRAME;
	poll = pak->datagramstart[mode->i.pf_inx] & (1 << mode->i.pf_shift);
	ns = (pak->datagramstart[mode->i.ns_inx] >> mode->i.ns_shift) &
		mode->modulo_mask;
	nr = (pak->datagramstart[mode->i.nr_inx] >> mode->i.nr_shift) &
		mode->modulo_mask;
    } else if (LAPB_ISSFRAME(pak)) {
	control = pak->datagramstart[LAPB_CONTROL_INX] & mode->s.control_mask;
	poll = pak->datagramstart[mode->s.pf_inx] & (1 << mode->s.pf_shift);
	ns = 0xFF;
	nr = (pak->datagramstart[mode->s.nr_inx] >> mode->s.nr_shift) &
		mode->modulo_mask;
    } else if (LAPB_ISUFRAME(pak)) {
	control = pak->datagramstart[LAPB_CONTROL_INX] & LAPB_U_CONTROL_M;
	poll = pak->datagramstart[LAPB_U_PF_INX] & (1 << LAPB_U_PF_SHIFT);
	ns = 0xFF;
	nr = 0xFF;

	/*
	 *  check for a change in mode
	 */
	if (((idb->lapb_modulo != idb->lapb_admin_modulo) ||
	     (idb->lapb_k != idb->lapb_admin_k)) &&
	    (pak->datagramsize == LAPB_U_OVERHEAD) &&
	    ((cmd && ((control == mode->alt_set_mode_control) ||
		      (control == mode->set_mode_control) ||
		      (control == LAPB_DISC))) ||
	     (!cmd && (control == LAPB_DM)))) {
	    idb->lapb_modulo = idb->lapb_admin_modulo;
	    idb->lapb_k = idb->lapb_admin_k;
	    /*
	     * Update lapb_n1 only if user has never configured N1 value.
	     */
	    if (!idb->lapb_override_def_n1)
		idb->lapb_n1 = LAPB_ADMIN_N1(idb);
	    SET_MODE(idb, mode);
	}
        /* NmEvent Hook 2 */
        if (control == LAPB_DISC)
            LAPBEvent(idb, DISCReceived);
    } else {
	control = LAPB_NOFRAME;
	poll = FALSE;
	ns = 0xFF;
	nr = 0xFF;
    }

    switch (idb->lapb_state) {
      case LAPB_DISCONNECTED:
	/*
	 * DISCONNECTED:
	 *    incoming SetMode, send UA and enter CONNECT
	 *    incoming AltSetMode, send SABM/E and enter SABMSENT
	 *    incoming DM, send SABM/E and enter SABMSENT
	 *    incoming IFRAME/P, DISC, or any valid command/P, send DM
	 *    otherwise, ignore frame.
	 */
	if (cmd && pak->datagramsize == LAPB_U_OVERHEAD &&
	    control == mode->set_mode_control) {
	    lapb_start(idb, pak, TRUE, poll, mode->mib_entry_reason);
	    idb->lapb_sabmsrcvd++;
	} else if (cmd && pak->datagramsize == LAPB_U_OVERHEAD &&
		   control == mode->alt_set_mode_control) {
	    idb->lapb_state = LAPB_SABM_SENT;
	    NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP, LAPB_MIBREASON_OTHER)
	    lapb_u_frame(idb, pak, LAPB_SABM, TRUE);
	    TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
	} else if (!cmd && pak->datagramsize == LAPB_U_OVERHEAD &&
		   control == LAPB_DM) {
	    idb->lapb_state = LAPB_SABM_SENT;
	    NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP, LAPB_MIBREASON_DM_RX)
	    lapb_u_frame(idb, pak, LAPB_SABM, TRUE);
	    TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
	} else if (cmd && lapb_validcmdframe(idb, pak) &&
		   (pak->datagramsize == LAPB_U_OVERHEAD ||
		    control == LAPB_IFRAME) &&
		   (poll || control == LAPB_DISC)) {
	    NEW_MIB_STATE(idb, LAPB_MIBMODE_DISCON, LAPB_MIBREASON_DM_TX)
	    if (control == LAPB_DISC)
		idb->lapb_discsrcvd++;
	    lapb_u_frame(idb, pak, LAPB_DM, poll);
	} else
	    datagram_done(pak);
	break;

      case LAPB_SABM_SENT:
	/*
	 * SABM SENT:
	 *    incoming UA/F (since we only send SABM/E Poll), enter CONNECT
	 *    incoming SetMode, send UA, enter CONNECT and mark SABM/E collision
	 *    incoming DISC, send DM and enter DISCONNECT
	 *    otherwise ignore frame.
	 */
	if (!cmd && pak->datagramsize == LAPB_U_OVERHEAD &&
	    control == LAPB_UA && poll) {
	    lapb_start(idb, pak, FALSE, poll, mode->mib_entry_reason);
	    datagram_done(pak);
	} else if (cmd && pak->datagramsize == LAPB_U_OVERHEAD &&
		   control == mode->set_mode_control) {
	    lapb_start(idb, pak, TRUE, poll, mode->mib_entry_reason);
	    /* start the mode setting collision timer */
	    TIMER_START(idb->lapb_col_timer, idb->lapb_t1);
	    idb->lapb_sabmsrcvd++;
	} else if (cmd && pak->datagramsize == LAPB_U_OVERHEAD &&
		   control == LAPB_DISC) {
	    lapb_reset(idb, "LAPB DISC received when SABMSENT");


	    lapb_u_frame(idb, pak, LAPB_DM, poll);
	    /*
	     * British PSS requires a SABM/E immediately
	     * after the DM since the DM state is unknown
	     * to them.
	     */
	    if (is_x25(idb) && !ISDDN(idb)) {
		idb->lapb_state = LAPB_SABM_SENT;
		NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP, LAPB_MIBREASON_DISC_RX);
		lapb_u_frame(idb, NULL, LAPB_SABM, TRUE);
	    } else {
		NEW_MIB_STATE(idb, LAPB_MIBMODE_DISCON, LAPB_MIBREASON_DISC_RX);
	    }
	    TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
	    idb->lapb_discsrcvd++;
	} else {
	    datagram_done(pak);
	}
	break;

      case LAPB_CONNECTED:
	/*
	 * determine if we can't send any I-frame.
	 */
	cant_send = !lapb_can_tx_i(idb);

	if (control == LAPB_IFRAME) {
	    if (!cmd || (pak->datagramsize < mode->i.overhead)) {
		lapb_frmr(idb, pak, LAPB_FRMR_W, poll, cmd);
		datagram_done(pak);
		break;
	    }
	    idb->lapb_iframesrcvd++;
	    /*
	     *	reject the I-frame if size is greater than allowed;
	     *  note that we need to account for the 16 bits of CRC the
	     *  driver stripped
	     */
	    if ((pak->datagramsize*8 + LAPB_CRC_BITS) > idb->lapb_n1) {
		lapb_frmr(idb, pak, LAPB_FRMR_Y, poll, cmd);
		datagram_done(pak);
		break;
	    }

	    /*
	     *  process the piggybacked acknowledgement
	     */
	    if (!lapb_ack(idb, pak, poll, cmd, nr))
		break;

	    /*
	     *  if the interface is congested, check if the I-frame
	     *  should be dropped
	     *
	     *  NB: we can start dropping I-frames before entering the
	     *	    congested state if the input queue limit is bizarre,
	     *  we'll only exit the congested state from the LAPB task
	     */
	    if (lapb_rx_drop_iframe(idb) ||
		idb->lapb_exit_rnr_rej) {
		idb->lapb_exit_rnr_rej = TRUE;
		if (poll ||
		    (idb->lapb_rx_state != LAPB_RX_CONGESTED)) {
		    idb->lapb_rx_state = LAPB_RX_CONGESTED;
		    lapb_s_frame(idb, pak, FALSE, poll);
		} else {
		    datagram_done(pak);
		}
		break;
	    }

	    if (ns == idb->lapb_vr) {
		/*
		 * Correctly sequenced IFRAME received.
		 */
		pak->datagramstart += mode->i.overhead;
		pak->datagramsize -= mode->i.overhead;
		pak->network_start = pak->datagramstart;

		/*
		 * COMP: do decompression if required
		 * if decompressor getbuffer() fails we don't
		 * want to ack this frame.
		 */
		if (idb->comp_db && !idb->ppp_lapb &&
		    (idb->comp_db->comp_configed != COMP_NOT_ENABLED)) {
		    pak = reg_invoke_compress_explode(
				idb->comp_db->comp_configed, idb, pak);
		    if (pak == NULL) {
			break;
		    }
		}

		/*
		 * Increment receive state variable and pass up if
		 * IFRAME contains data
		 */
		idb->lapb_vr = ++idb->lapb_vr & mode->modulo_mask;

		if ((pak->datagramsize == 0) && !is_x25(idb)) {
		    datagram_done(pak);
		} else {
		    switch (pak->enctype) {
		    case ET_PPP:
            {
                void (*siqueue)(hwidbtype *, paktype *);
                uchar *p;

                /* ppp_parse_packet needs this header */
                pak->datagramsize += PPP_PROTO_HDR_ENCAPBYTES;
                pak->datagramstart -= PPP_PROTO_HDR_ENCAPBYTES;
                p = pak->datagramstart;
                PPP_PUTSHORT(PPP_STATION, p);
                siqueue = (*idb->ppp_save_parse_packet)(idb, pak, NULL);
                if(siqueue)
                   (*siqueue)(idb, pak);
                else
                   datagram_done(pak);
            }
            break;
  
		    case ET_LAPB:
			/*
			 * If this interface is a rotary group member we need
			 * to adjust its idb to reflect the group leader.
			 */
			if (reg_invoke_dialer_change_pak_if(idb, pak)) {
			    datagram_done(pak);
			    break;
			}
			/*
			 * Multiprotocol LAPB 
			 */
			if (idb->lapb_protocol == LINK_ILLEGAL) {
			    /*
			     *  remove protocol ID
			     */
			    pak->datagramstart += LAPB_PID_BYTES;
			    pak->datagramsize -= LAPB_PID_BYTES;
			    pak->network_start = pak->datagramstart;
			    /*
			     *  This is a bridge packet, If the spanning
			     *  tree protocol is turned on, set PAK_TBRIDGE_IN 
			     *  flag and forward it to the bridge suite.
			     */
			    if ((linktype = lapb_getlink(pak))
				== LINK_BRIDGE) {
				if (!pak->if_input->span_ptr)
				    datagram_done(pak);
				else {
				    pak->flags |= PAK_TBRIDGE_IN;
				    net_bridge_enqueue(pak);
				}
				break;
			    }
			    /*
			     *  This is a spanning tree packet. 
			     *  Snap the frame to alignment and pass it up.
			     */
			    else if ((linktype == LINK_IEEE_SPANNING)
				|| (linktype == LINK_DEC_SPANNING)) {
				ALIGN_DATAGRAM_CMD(pak_realign(pak, NULL));
				reg_invoke_raw_enqueue(linktype, pak);
				break;
			    }
			}
			raw_enqueue(idb, pak);
			break;

		    case ET_X25:
			x25_input(idb, pak);
			break;
		    default:
			datagram_done(pak);
			break;
		    }
		}

		pak = NULL;

		/*
		 *  check if the receive engine needs to signal congestion
		 */
		if ((idb->lapb_rx_state != LAPB_RX_CONGESTED) &&
		    lapb_rx_congestion_enter(idb)) {
		    idb->lapb_rx_state = LAPB_RX_CONGESTED;
		    idb->lapb_exit_rnr_rej = FALSE;
		    lapb_s_frame(idb, NULL, FALSE, poll);
		} else if (poll || (idb->lapb_rx_state == LAPB_RX_REJ_SENT)) {
		    /*
		     *  send an explicit ack. if the I-frame had the Poll bit
		     *  set or if the I-frame was requested via a REJect
		     */

		    /*
		     *  exit the I-frame recovery state
		     */
		    if (idb->lapb_rx_state == LAPB_RX_REJ_SENT) {
			idb->lapb_rx_state = LAPB_RX_NORMAL;
			NEW_MIB_STATE(idb, LAPB_MIBMODE_CON,
					LAPB_MIBREASON_OTHER)
		    }
		    lapb_s_frame(idb, NULL, FALSE, poll);
		}
	    } else if (poll && (idb->lapb_rx_state == LAPB_RX_CONGESTED)) {
		lapb_s_frame(idb, pak, FALSE, poll);
	    } else if (poll || (idb->lapb_rx_state != LAPB_RX_REJ_SENT)) {
		/*
		 * If not correct sequence number,
		 * Send REJ for first incorrect sequenced IFRAME
		 * or IFRAME/P.
		 */
		if (idb->lapb_rx_state == LAPB_RX_NORMAL) {
		    idb->lapb_rx_state = LAPB_RX_REJ_SENT;
		    NEW_MIB_STATE(idb, LAPB_MIBMODE_REJ, LAPB_MIBREASON_OTHER)
		}
		lapb_s_frame(idb, pak, FALSE, poll);
	    } else
		datagram_done(pak);
	} else if (LAPB_ISUFRAME(pak)) {
	    /*
	     *	validate the control field for a command or response and
	     *	validate the frame length
	     */
	    reason = 0;
	    if (cmd) {
		if (control != LAPB_SABM && control != LAPB_SABME &&
		    control != LAPB_DISC)
		    reason = LAPB_FRMR_W;
		else if (pak->datagramsize != LAPB_U_OVERHEAD)
		    reason = LAPB_FRMR_W | LAPB_FRMR_X;
	    } else {
		if (control == LAPB_FRMR) {
		    if (pak->datagramsize !=
			LAPB_U_OVERHEAD + mode->frmr.i_field_cnt)
			reason = LAPB_FRMR_W | LAPB_FRMR_X;
		    else {
			bcopy(&pak->datagramstart[LAPB_CONTROL_INX],
			      idb->lapb_frmr_rxdata, mode->frmr.i_field_cnt);
			idb->lapb_frmr_rxlen = mode->frmr.i_field_cnt;
		    }
		} else if (control == LAPB_UA || control == LAPB_DM) {
		    if (pak->datagramsize != LAPB_U_OVERHEAD)
			reason = LAPB_FRMR_W | LAPB_FRMR_X;
		} else
		    reason = LAPB_FRMR_W;
	    }
	    if (reason) {
		lapb_frmr(idb, pak, reason, poll, cmd);
		datagram_done(pak);
		break;
	    }

	    switch (control) {
	      case LAPB_SABM:
	      case LAPB_SABME:
		if (control == mode->set_mode_control) {
		    /*
		     *  incoming SABM/E, bounce state, send UA and
		     *  re-enter CONNECT
		     */
		    lapb_reset(idb, "LAPB SABM/SABME seen when CONNECT");
		    lapb_start(idb, pak, TRUE, poll, mode->mib_reset_reason);
		    idb->lapb_sabmsrcvd++;
		} else {
		    /*
		     *  incoming AltSetMode, send SABM and enter SABMSENT
		     */
		    lapb_reset(idb, "LAPB Set Mode control seen when CONNECT");
		    lapb_u_frame(idb, pak, LAPB_SABM, TRUE);
		    idb->lapb_state = LAPB_SABM_SENT;
		    TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
		    NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP, LAPB_MIBREASON_OTHER)
		}
		break;
	      case LAPB_FRMR:
		errmsg(&msgsym(FRAMEERR, LAPB),
		    idb->hw_namestring,
		    (mode->frmr.rej_ctl_cnt == 2) ?
			(pak->datagramstart[LAPB_U_OVERHEAD] << 8) |
			 pak->datagramstart[LAPB_U_OVERHEAD + 1] :
			 pak->datagramstart[LAPB_U_OVERHEAD],
		    (pak->datagramstart[mode->frmr.vs_inx] >>
			mode->frmr.vs_shift) & mode->modulo_mask,
		    (pak->datagramstart[mode->frmr.c_r_inx] &
			(1 << mode->frmr.c_r_shift)) ? 'R' : 'C',
		    (pak->datagramstart[mode->frmr.vr_inx] >>
			mode->frmr.vr_shift) & mode->modulo_mask,
		    pak->datagramstart[mode->frmr.cause_inx]);
		if (pak->datagramstart[mode->frmr.cause_inx] & LAPB_FRMR_W) {
		    if (pak->datagramstart[mode->frmr.cause_inx] & LAPB_FRMR_X){
		      errmsg(&msgsym(INFOBAD, LAPB), idb->hw_namestring);
                      /* NmEvent Hook 3 */
                      LAPBEvent(idb, LocalReceivedFRMR_X);
                    }
		    else{
		      errmsg(&msgsym(CTRLBAD, LAPB), idb->hw_namestring);
                      /* NmEvent Hook 4 */
                      LAPBEvent(idb, LocalReceivedFRMR_W);
		}
		}
		if (pak->datagramstart[mode->frmr.cause_inx] & LAPB_FRMR_Y){
		  errmsg(&msgsym(N1TOOBIG, LAPB), idb->hw_namestring);
                  /* NmEvent Hook 5 */
                  LAPBEvent(idb, LocalReceivedFRMR_Y);
                }
		if (pak->datagramstart[mode->frmr.cause_inx] & LAPB_FRMR_Z){
		  errmsg(&msgsym(INVNR, LAPB), idb->hw_namestring);
                  /* NmEvent Hook 6 */
                  LAPBEvent(idb, LocalReceivedFRMR_Z);
                }
		idb->lapb_frmrsrcvd++;
		/* Fall through */
	      case LAPB_UA:
		/*
		 *  a UA is valid if we had a SABM collision;
		 *  check if it was recieved within the T1 time limit from
		 *  when the collision was detected
		 */
		 if ((control == LAPB_UA) &&
		     (idb->lapb_state == LAPB_CONNECTED)) {
		     if (TIMER_RUNNING(idb->lapb_col_timer) &&
			 SLEEPING(idb->lapb_col_timer)) {
		       TIMER_STOP(idb->lapb_col_timer);
	    	       datagram_done(pak);
		       break;
		     }
		}
		/* Fall through */
	      case LAPB_DM:
		/*
		 *  incoming FRMR, UA or DM, send SABM and enter SABMSENT
		 */
		if (control == LAPB_FRMR) {
		    lapb_reset(idb, "LAPB FRMR received when CONNECT");
		    NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP,
				  LAPB_MIBREASON_FRMR_RX)
		} else if (control == LAPB_UA) {
		    lapb_reset(idb, "LAPB UA received when CONNECT");
		    NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP, LAPB_MIBREASON_OTHER)
		} else {
		    lapb_reset(idb, "LAPB DM received when CONNECT");
		    NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP, LAPB_MIBREASON_DM_RX)
		}
		idb->lapb_state = LAPB_SABM_SENT;
		lapb_u_frame(idb, pak, LAPB_SABM, TRUE);
		TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
		break;

	      case LAPB_DISC:
		/*
		 *  incoming DISC, send UA and enter DISCONNECT
		 */
		lapb_reset(idb, "LAPB DISC received when CONNECT");

		lapb_u_frame(idb, pak, LAPB_UA, poll);

		/*
		 * Output a SABM immediately after the UA in an
		 * attempt to bring the link back up. Only really
		 * needed when encapsulation is X25 (PSS).
		 */
		if (is_x25(idb) && !ISDDN(idb)) {
		    idb->lapb_state = LAPB_SABM_SENT;
		    NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP,
				  LAPB_MIBREASON_DISC_RX);
		    lapb_u_frame(idb, NULL, LAPB_SABM, TRUE);
		} else {
		    NEW_MIB_STATE(idb, LAPB_MIBMODE_DISCON,
				  LAPB_MIBREASON_DISC_RX);
		}
		TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
		idb->lapb_discsrcvd++;
		break;
	    }
	} else if (LAPB_ISSFRAME(pak)) {
	    if (control != LAPB_RNR && control != LAPB_RR &&
		control != LAPB_REJ) {
		/*
		 * Received illegal frame so respond
		 * with FRMR and enter ERROR.
		 */
		lapb_frmr(idb, pak, LAPB_FRMR_W, poll, cmd);
		datagram_done(pak);
		break;
	    }
	    if (pak->datagramsize != mode->s.overhead) {
		lapb_frmr(idb, pak, LAPB_FRMR_X | LAPB_FRMR_W, poll, cmd);
		datagram_done(pak);
		break;
	    }
	    if (!lapb_ack(idb, pak, poll, cmd, nr))
		break;
	    if (cmd) {
		/*
		 * Incoming command SFRAME, ack with RR/F if P.
		 */
		if (poll) {
		    lapb_s_frame(idb, pak, FALSE, poll);
		} else {
		    datagram_done(pak);
		}
	    } else if (poll) {
		/*
		 * Input response SFRAME, if F and not expecting it,
		 * respond with SABM and enter SABMSENT. If expecting
		 * F, then save VS for possible retransmission.
		 * Otherwise, flush frame.
		 */
		if (idb->lapb_poll) {
		    idb->lapb_poll = FALSE;
		    datagram_done(pak);
		    if ((control == LAPB_RR) && (nr != idb->lapb_vs))
			idb->lapb_t1_retx++;
		} else {
		    lapb_reset(idb, "LAPB unexpected Final bit");
		    idb->lapb_state = LAPB_SABM_SENT;
		    NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP,
				  LAPB_MIBREASON_OTHER)
		    lapb_u_frame(idb, pak, LAPB_SABM, TRUE);
		    TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
		    break;
		}
	    } else
		datagram_done(pak);

	    switch (control) {
	      case LAPB_RNR:
		/*
		 * Enter REMOTE BUSY and start T1 timer for polling
		 */
		idb->lapb_rnrsrcvd++;
		NEW_MIB_STATE(idb, (idb->lapb_rx_state == LAPB_RX_REJ_SENT) ?
				LAPB_MIBMODE_REJ_RNR : LAPB_MIBMODE_RNR_RX,
				LAPB_MIBREASON_OTHER)
		idb->lapb_tx_state = LAPB_TX_REMOTE_BUSY;
		TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
		break;
	      case LAPB_REJ:
		idb->lapb_rejsrcvd++;
		/* FALLTHROUGH */
	      case LAPB_RR:
		/*
		 * per X.25 1988 section 2.4.5.7 paragraph 4 and
		 * section 2.4.5.9 paragraph 6,
		 * if we receive an unsolicited RR or REJ while awaiting
		 * a response to a Poll, we should wait for the
		 * Final response
		 */
		if (idb->lapb_poll)
		    break;

		if (idb->lapb_tx_state == LAPB_TX_REMOTE_BUSY) {
		    /*
		     * Return transmit state to normal and stop polling timer
		     */
		    idb->lapb_tx_state = LAPB_TX_NORMAL;
		    NEW_MIB_STATE(idb, LAPB_MIBMODE_CON, LAPB_MIBREASON_OTHER)
		    TIMER_STOP(idb->lapb_t1_timer);
		    idb->lapb_retcnt = 0;
		}

		/*
		 * For an REJ or a RR/Final (or REMOTE BUSY state exited),
		 * retransmit all frames up to current send state variable.
		 */
		if ((control == LAPB_REJ) || (!cmd && poll)) {
		    TIMER_STOP(idb->lapb_t1_timer);
		    idb->lapb_retcnt = 0;

		    /*
		     *  UGH!
		     *  staple the unacknowledged I-frame queue to the
		     *  beginning of the retransmit queue
		     */
		    level = raise_interrupt_level(ALL_DISABLE);

		    if (idb->lapb_unackq.qtail) {
			((elementtype *)idb->lapb_unackq.qtail)->next =
				idb->lapb_retxq.qhead;
			idb->lapb_retxq.qhead = idb->lapb_unackq.qhead;
			idb->lapb_retxq.count += idb->lapb_unackq.count;

			/*
			 *  make sure the output queue count is adjusted!
			 */
			idb->output_qcount += idb->lapb_unackq.count;

			idb->lapb_unackq.qhead = NULL;
			idb->lapb_unackq.qtail = NULL;
			idb->lapb_unackq.count = 0;
		    }

		    idb->lapb_vs = nr;

		    reset_interrupt_level(level);
		}
		break;
	    }

	    /*
	     * make sure the timer is running if we've got unacknowledged
	     * I-frames
	     */
	    if (!TIMER_RUNNING(idb->lapb_t1_timer) &&
		!QUEUEEMPTY(&idb->lapb_unackq))
		TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
	} else {
	    /*
	     *	Gotcha! corner case of !(I frame || S frame || U frame)
	     */
	    lapb_frmr(idb, pak, LAPB_FRMR_W, poll, cmd);
	    datagram_done(pak);
	}
	/*
	 * If this frame reopened the transmit window, or there's something
	 * in the retransmit queue to send, try to restart output.
	 */
	if ((cant_send || !QUEUEEMPTY(&idb->lapb_retxq))
	    && lapb_can_tx_i(idb)) {
	    (*idb->soutput)(idb);
	}

	break;
      case LAPB_DISC_SENT:
	/*
	 * DISC SENT:
	 *    incoming SABM/E, send DM
	 *    incoming DISC, send UA
	 *    incoming UA, enter DISCONNECT
	 */
	if (cmd && pak->datagramsize == LAPB_U_OVERHEAD &&
	    ((control == mode->set_mode_control) ||
	     (control == mode->alt_set_mode_control))) {
	    lapb_u_frame(idb, pak, LAPB_DM, poll);
	    idb->lapb_sabmsrcvd++;
	} else if (cmd && pak->datagramsize == LAPB_U_OVERHEAD &&
		   control == LAPB_DISC) {
	    NEW_MIB_STATE(idb, LAPB_MIBMODE_DISCON, LAPB_MIBREASON_DISC_RX)
	    lapb_u_frame(idb, pak, LAPB_UA, poll);
	    idb->lapb_discsrcvd++;
	} else if (!cmd && pak->datagramsize == LAPB_U_OVERHEAD &&
		   control == LAPB_UA) {
	    lapb_reset(idb, "LAPB UA received when DISCSENT");
	    NEW_MIB_STATE(idb, LAPB_MIBMODE_DISCON, LAPB_MIBREASON_OTHER)
	    TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
	    datagram_done(pak);
	} else
	    datagram_done(pak);
	break;
    case LAPB_FRMR_SENT:
	/*
	 * FRMR SENT:
	 *    incoming FRMR or DM, send SABM/E and enter SABMSENT
	 *    any other incoming response, ignore
	 *    incoming SetMode, send UA and enter CONNECT
	 *    incoming DISC, send UA and enter DISCONNECT
	 *    invalid U command, resend FRMR
	 *    poll frame, resend FRMR
	 */
	if (LAPB_ISUFRAME(pak)) {
	    if (!cmd && control == LAPB_FRMR) {
		if (pak->datagramsize ==
		    LAPB_U_OVERHEAD + mode->frmr.i_field_cnt) {
		    idb->lapb_frmr_rxlen =
			LAPB_U_OVERHEAD + mode->frmr.i_field_cnt;
		    bcopy(&pak->datagramstart[LAPB_U_OVERHEAD],
			  idb->lapb_frmr_rxdata, idb->lapb_frmr_rxlen);
		}
	    }
	    if (!cmd &&
		(((control == LAPB_FRMR) &&
		  (pak->datagramsize == LAPB_U_OVERHEAD +
			mode->frmr.i_field_cnt)) ||
		 ((control == LAPB_DM) &&
			pak->datagramsize == LAPB_U_OVERHEAD))) {
		if (control == LAPB_FRMR) {
		    idb->lapb_frmrsrcvd++;
		    if (!idb->loopback)
			lapb_reset(idb, "LAPB FRMR received when ERROR");
		    NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP,
				  LAPB_MIBREASON_FRMR_RX)
		} else {
		    lapb_reset(idb, "LAPB DM received when ERROR");
		    NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP, LAPB_MIBREASON_DM_RX)
		}

		idb->lapb_state = LAPB_SABM_SENT;
		lapb_u_frame(idb, pak, LAPB_SABM, TRUE);
		TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
		break;
	    } else if (!cmd) {
		datagram_done(pak);
		break;
	    } else if (pak->datagramsize == LAPB_U_OVERHEAD &&
			control == mode->set_mode_control) {
		idb->lapb_sabmsrcvd++;
		lapb_reset(idb, "LAPB SABM/E received when ERROR");
		lapb_start(idb, pak, TRUE, poll, mode->mib_entry_reason);
		break;
	    } else if (pak->datagramsize == LAPB_U_OVERHEAD &&
			control == LAPB_DISC) {
		idb->lapb_discsrcvd++;
		lapb_reset(idb, "LAPB DISC received when ERROR");
		NEW_MIB_STATE(idb, LAPB_MIBMODE_DISCON, LAPB_MIBREASON_DISC_RX)
		lapb_u_frame(idb, pak, LAPB_UA, poll);
		TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
		break;
	    }

	    /*
	     *  we have an invalid Unnumbered command frame--
	     *  retransmit the original FRMR
	     */
	    lapb_frmr(idb, NULL, 0, poll, cmd);
	} else if (cmd) {
	    /*
	     *  1988 CCITT section 2.4.7.3, ignore I and S frames unless
	     *	they have the Poll bit set
	     */
	    /*
	     *  1980 CCITT section 2.4.9.4,
	     *  resend the original FRMR on receipt of a command
	     */
	    if (poll || IS_1980(idb))
		lapb_frmr(idb, NULL, 0, poll, cmd);
	}

	datagram_done(pak);
	break;
    }

    /*
     *  if T1 isn't running and a T4 duration is configured, start T4
     */
    if (TIMER_RUNNING(idb->lapb_t1_timer)) {
	if (TIMER_RUNNING(idb->lapb_t4_timer))
	    TIMER_STOP(idb->lapb_t4_timer);
    } else if (idb->lapb_t4) {
	if (idb->lapb_state == LAPB_CONNECTED) {
	    TIMER_START(idb->lapb_t4_timer, idb->lapb_t4 * ONESEC);
	} else if (TIMER_RUNNING(idb->lapb_t4_timer)) {
	    TIMER_STOP(idb->lapb_t4_timer);
	}
    }
}

/*
 * lapb_frmr
 * Send a LAPB frame error.
 */

void lapb_frmr (
    register hwidbtype *idb,
    paktype *epak,
    uchar reason,
    uchar poll_final,
    uchar cmd)
{
    lapb_mode *mode;

    SET_MODE(idb, mode);

    /*
     *	check if we're entering the FRMR_SENT state
     */
    if (idb->lapb_state != LAPB_FRMR_SENT) {
	/*
	 *  format the FRMR information and change the state
	 */
	idb->lapb_frmr_txlen = LAPB_U_OVERHEAD + mode->frmr.i_field_cnt;
	memset(idb->lapb_frmr_txdata, 0, idb->lapb_frmr_txlen);

	idb->lapb_frmr_txdata[0] = epak->datagramstart[LAPB_CONTROL_INX];
	if (mode->frmr.rej_ctl_cnt == 2) {
	    /*
	     *  I-frames and S-frames have a two byte rejected control field
	     */
	    if ((LAPB_ISIFRAME(epak) || LAPB_ISSFRAME(epak)) &&
		(epak->datagramsize > LAPB_CONTROL_INX + 1))
		idb->lapb_frmr_txdata[1] =
			epak->datagramstart[LAPB_CONTROL_INX + 1];
	}
	idb->lapb_frmr_txdata[mode->frmr.vr_inx - LAPB_U_OVERHEAD] |=
		idb->lapb_vr << mode->frmr.vr_shift;
	if (!cmd)
	    idb->lapb_frmr_txdata[mode->frmr.c_r_inx - LAPB_U_OVERHEAD] |=
		1 << mode->frmr.c_r_shift;
	idb->lapb_frmr_txdata[mode->frmr.vs_inx - LAPB_U_OVERHEAD] |=
		idb->lapb_vs << mode->frmr.vs_shift;
	idb->lapb_frmr_txdata[mode->frmr.cause_inx - LAPB_U_OVERHEAD] = reason;

	lapb_reset(idb, "LAPB FRMR sent");
	idb->lapb_state = LAPB_FRMR_SENT;
	NEW_MIB_STATE(idb, LAPB_MIBMODE_FRMR, LAPB_MIBREASON_OTHER);
	idb->lapb_retcnt = 0;

        switch (reason) {
        case (LAPB_FRMR_W):
            LAPBEvent(idb, LocalSentFRMR_W);  /* NmEvent Hook 7 */
            break;
        case (LAPB_FRMR_X | LAPB_FRMR_W):
            LAPBEvent(idb, LocalSentFRMR_X);  /* NmEvent Hook 8 */
            break;
        case (LAPB_FRMR_Y):
            LAPBEvent(idb, LocalSentFRMR_Y);  /* NmEvent Hook 9 */
            break;
        case (LAPB_FRMR_Z):
            LAPBEvent(idb, LocalSentFRMR_Z);  /* NmEvent Hook 10 */
            break;
        default:
            break; /* if reason = 0 then the frmr is resent 
                    * but we don't want to resend the alert. 
                    */
        }

    } else {
	NEW_MIB_STATE(idb, LAPB_MIBMODE_FRMR, LAPB_MIBREASON_FRMR_TX);
    }

    /*
     *	generate and send the FRMR frame; if we can't generate the frame
     *	it'll simply look like the link swallowed it
     */
    lapb_u_frame(idb, NULL, LAPB_FRMR, cmd ? poll_final : FALSE);
    TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
}

/*
 * lapb_ack
 * Validate and acknowledge frames from an N(R) value
 */

boolean lapb_ack (
    register hwidbtype *idb,
    register paktype *pak,
    uchar poll,
    uchar cmd,
    register int nr)
{
    lapb_mode *mode;

    SET_MODE(idb, mode);

    /*
     *  we have a bad N(R) if it would acknowledge more frames than
     *  remain unacknowledged
     */
    if (((nr + mode->modulo - idb->lapb_rvr) & mode->modulo_mask) >
	QUEUESIZE(&idb->lapb_unackq)) {
	lapb_frmr(idb, pak, LAPB_FRMR_Z, poll, cmd);
	datagram_done(pak);
	return(FALSE);
    }

    /*
     *  if some frames have been acknowledged zero the retry counter and,
     *  in the absence of abnormal conditions (polling, or RNR received),
     *  cancel the T1 timer
     */
    if (idb->lapb_rvr != nr) {
	idb->lapb_retcnt = 0;

	if (!idb->lapb_poll && (idb->lapb_tx_state == LAPB_TX_NORMAL))
	    TIMER_STOP(idb->lapb_t1_timer);
    }

    while (idb->lapb_rvr != nr) {
	datagram_done(pak_dequeue(&idb->lapb_unackq));
	idb->lapb_rvr = ++idb->lapb_rvr & mode->modulo_mask;
    }

    /*
     *  if a poll is outstanding or some frames are unacknowledged,
     *  make sure T1 is running
     */
    if (idb->lapb_poll || (QUEUESIZE(&idb->lapb_unackq) > 0)) {
	if (!TIMER_RUNNING(idb->lapb_t1_timer))
	    TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
    }

    return(TRUE);
}

/*
 * lapbBLOCK
 * Scheduler test for the lapb timer process
 */

boolean lapbBLOCK(void)
{
    register hwidbtype *idb;
    register lapbidbtype *lapbidb;

    for (lapbidb = (lapbidbtype *) lapbidbQ.qhead; lapbidb; 
            lapbidb = lapbidb->next) {
        idb = lapbidb->hwidb;
	if (! is_lapb(idb))
	    continue;

	/*
	 *  stop the set mode collision timer if it has expired
	 */
	if (TIMER_RUNNING(idb->lapb_col_timer) &&
	    ! SLEEPING(idb->lapb_col_timer)) {
	    TIMER_STOP(idb->lapb_col_timer);
	}

	/*
	 *  wake up if T3 has expired
	 */
	if (idb->state != IDBS_UP) {
	    if (! idb->lapb_upstate)
		continue;

	    if (TIMER_RUNNING(idb->lapb_t3_timer) &&
		SLEEPING(idb->lapb_t3_timer)) {
		/*
		 *  renew T1 if it has expired
		 */
		if (TIMER_RUNNING(idb->lapb_t1_timer) &&
		    ! SLEEPING(idb->lapb_t1_timer)) {
		    TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
		}
		continue;
	    }

	    return(FALSE);
	}

	/*
	 *  wake up if the receive machine is no longer congested
	 */
	if ((idb->lapb_rx_state == LAPB_RX_CONGESTED) &&
	    lapb_rx_congestion_exit(idb)) {
	    return(FALSE);
	}

	/*
	 *  sanity cleanup--cancel T4 if T1 is running
	 */
	if (TIMER_RUNNING(idb->lapb_t4_timer) &&
	    TIMER_RUNNING(idb->lapb_t1_timer)) {
	    TIMER_STOP(idb->lapb_t4_timer);
	}

	/*
	 *  wake up if T1 has expired
	 */
	if (TIMER_RUNNING(idb->lapb_t1_timer) &&
	    ! SLEEPING(idb->lapb_t1_timer)) {
	    return(FALSE);
	}

	/*
	 *  wake up if we need to explicitly acknowledge a received I-frame
	 *  (LAPB timer T2 would be checked here--implicitly T2 == 0)
	 *  NB: defer an explicit acknowledgement if we've got frames to
	 *	send (ack. can be piggybacked) unless we're prohibited from
	 *	sending I-frames
	 */
	if ((idb->lapb_nr != idb->lapb_vr) &&
	    ((idb->output_qcount <= 0) ||
	     (! lapb_can_tx_i(idb)))) {
	    return(FALSE);
	}

	/*
	 *  wake up if T4 has expired
	 */
	if (TIMER_RUNNING(idb->lapb_t4_timer) &&
	    ! SLEEPING(idb->lapb_t4_timer)) {
		return(FALSE);
	}
    }

    return(TRUE);
}

/*
 * lapb_timer
 * Handle retransmission among various other protocol oddities
 */

forktype lapb_timer (void)
{
    register hwidbtype *idb;
    register lapbidbtype *lapbidb;
    leveltype level;

    edisms((blockproc *)systeminitBLOCK, 0);
    timer_up = TRUE;

    for (;;) {
	edisms((blockproc *)lapbBLOCK, 0L);
        for (lapbidb = (lapbidbtype *) lapbidbQ.qhead; lapbidb; 
                lapbidb = lapbidb->next) {
            idb = lapbidb->hwidb;
	    if (! is_lapb(idb))
		continue;

	    /*
	     *  when T3 expires, take the protocol down
	     */
	    level = raise_interrupt_level(NETS_DISABLE);
	    if (TIMER_RUNNING(idb->lapb_t3_timer) &&
		! SLEEPING(idb->lapb_t3_timer)) {
		lapb_cstate(idb);
		if (lapb_debug) {
		    buginf("\n%s: LAPB T3 %s %u",
			   idb->hw_namestring, lapb_prstate(idb),
			   idb->lapb_t3_timer);
		}
		TIMER_STOP(idb->lapb_t3_timer);
	    }
	    reset_interrupt_level(level);

	    if (idb->state != IDBS_UP)
		continue;

	    level = raise_interrupt_level(NETS_DISABLE);

	    /*
	     *  exit the RNR state if we're no longer congested
	     */
	    if ((idb->lapb_rx_state == LAPB_RX_CONGESTED) &&
		lapb_rx_congestion_exit(idb)) {
		idb->lapb_rx_state = (idb->lapb_exit_rnr_rej) ?
			LAPB_RX_REJ_SENT : LAPB_RX_NORMAL;
		idb->lapb_exit_rnr_rej = FALSE;
		lapb_s_frame(idb, NULL, TRUE, FALSE);
	    }

	    /*
	     *  handle expiration of T1
	     */
	    if (TIMER_RUNNING(idb->lapb_t1_timer) &&
		! SLEEPING(idb->lapb_t1_timer)) {
		if (lapb_debug) {
		    buginf("\n%s: LAPB T1 %s %u %d",
			   idb->hw_namestring, lapb_prstate(idb),
			   idb->lapb_t1_timer, idb->lapb_retcnt);
		}

		TIMER_STOP(idb->lapb_t1_timer);

		if (idb->lapb_retcnt == (idb->lapb_n2 - 1)) {
		    /*
		     * We need this to force stupid hardware to wake up
		     */
		    (*idb->reset)(idb);
		    lapb_reset(idb, "LAPB restarted when N2 exceeded");
		    NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP, LAPB_MIBREASON_N2)
		    idb->lapb_state = LAPB_SABM_SENT;
		    lapb_u_frame(idb, NULL, LAPB_SABM, TRUE);
		    TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);

		    /* NmEvent Hook 1 */
		    LAPBEvent(idb, RetryCountExpired);
		} else {
		    switch (idb->lapb_state) {
		      case LAPB_DISCONNECTED:
		      case LAPB_SABM_SENT:		/* resend SABM */
			NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP,
					LAPB_MIBREASON_OTHER);
			idb->lapb_state = LAPB_SABM_SENT;
			lapb_u_frame(idb, NULL, LAPB_SABM, TRUE);
			TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
			break;
		      case LAPB_CONNECTED:		/* send Poll */
			NEW_MIB_STATE(idb,
				(idb->lapb_tx_state == LAPB_TX_REMOTE_BUSY) ?
				  LAPB_MIBMODE_T1_RNR_RX : LAPB_MIBMODE_T1,
				LAPB_MIBREASON_OTHER);
			lapb_s_frame(idb, NULL, TRUE, TRUE);
			idb->lapb_poll = TRUE;
			TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
			break;
		      case LAPB_DISC_SENT:		/* resend DISC */
			NEW_MIB_STATE(idb, LAPB_MIBMODE_DISCON,
					LAPB_MIBREASON_DISC_TX);
			lapb_u_frame(idb, NULL, LAPB_DISC, FALSE);
			TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
			break;
		      case LAPB_FRMR_SENT:		/* resend FRMR */
			lapb_frmr(idb, NULL, 0, FALSE, FALSE);
			break;
		    }
		    idb->lapb_retcnt++;
		}
	    }

	    /*
	     *  check if we need to explicitly acknowledge a received I-frame
	     *  (LAPB timer T2 would be checked here--implicitly T2 == 0)
	     */
	    if ((idb->lapb_nr != idb->lapb_vr) &&
		((idb->output_qcount <= 0) ||
		 (! lapb_can_tx_i(idb)))) {
		lapb_s_frame(idb, NULL, FALSE, FALSE);
	    }

	    /*
	     *  check T4, the keepalive timer
	     */
	    if (! TIMER_RUNNING(idb->lapb_t1_timer) &&
		TIMER_RUNNING(idb->lapb_t4_timer) &&
		! SLEEPING(idb->lapb_t4_timer)) {
		if (lapb_debug) {
		    buginf("\n%s: LAPB T4 %s %u",
			   idb->hw_namestring, lapb_prstate(idb),
			   idb->lapb_t4_timer);
		}

		TIMER_STOP(idb->lapb_t4_timer);
		lapb_s_frame(idb, NULL, TRUE, TRUE);
		idb->lapb_poll = TRUE;
		TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
	    }

	    reset_interrupt_level(level);
	} /* for all LAPB contexts */
    } /* task endless loop */
}

/*
 * lapb_reset
 * Reset LAPB protocol machine
 */

void lapb_reset (register hwidbtype *idb, char *why)
{
    register paktype *pak;

    TIMER_STOP(idb->lapb_t1_timer);
    TIMER_STOP(idb->lapb_t3_timer);
    TIMER_STOP(idb->lapb_t4_timer);
    TIMER_STOP(idb->lapb_col_timer);
    idb->lapb_retcnt = 0;
    idb->lapb_state = LAPB_DISCONNECTED;
    idb->lapb_tx_state = LAPB_TX_INACTIVE;
    idb->lapb_rx_state = LAPB_RX_INACTIVE;
    idb->lapb_exit_rnr_rej = FALSE;
    idb->lapb_upstate = FALSE;
    idb->lapb_poll = FALSE;
    idb->lapb_vs = 0;
    idb->lapb_vr = 0;
    idb->lapb_rvr = 0;
    idb->lapb_nr = 0;

    if (idb->lapb_requeue_pak) {
	datagram_done(idb->lapb_requeue_pak);
	idb->lapb_requeue_pak = NULL;
	idb->output_qcount--;
    }

    while ((pak = pak_dequeue(&idb->lapb_u_s_q))) {
	idb->output_qcount--;
	datagram_done(pak);
    }

    while ((pak = pak_dequeue(&idb->lapb_retxq))) {
	idb->output_qcount--;
	datagram_done(pak);
    }

    while ((pak = pak_dequeue(&idb->lapb_holdq))) {
	idb->output_qcount--;
	datagram_done(pak);
    }

    while ((pak = pak_dequeue(&idb->lapb_unackq))) {
	datagram_done(pak);
    }

    holdq_reset(idb);

    /*
     *  notify other protocols the link is down
     */
    if (is_x25(idb)) {
	if (idb->x25_flags & X25_LINKRESTART)
	    x25_cstate(idb);
    } else if (idb->ppp_lapb) {
	/*
	 * If PPP-LAPB is running, notify PPP of the lapb reset event
	 */
	reg_invoke_lapb_ppp_event_notify(idb);
    } else {
	/* 
	 * No layer over LAPB, generate the link down trap 
	 */
	idb->reason = why;
	reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
    }
}

/*
 * lapb_getlink
 * Determine the protocol family of the packet
 */

long lapb_getlink (paktype *pak)
{
    lapb_mode *mode;

    SET_MODE(pak->if_input->hwptr, mode);

    switch (pak->enctype) {
	case ET_LAPB:
	    if (pak->if_input->hwptr->lapb_protocol == LINK_ILLEGAL)
		return(serial2link(
			GETSHORT(pak->datagramstart - LAPB_PID_BYTES)));
	    return(pak->if_input->hwptr->lapb_protocol);
	case ET_HDLC:
	    return(hdlc_getlink(pak));
	default:
	    return(LINK_ILLEGAL);
    }
}

/*
 * lapb_bcast
 * Predicate for existing protocol broadcast support
 */
fuzzy
lapb_bcast (hwidbtype *hwidb, paktype *pak)
{
    return FALSE;
}

static const char change_held[] = "\nChange held until LAPB is reset";

void lapb_encaps (parseinfo *csb)
{
    register hwidbtype *idb;
    leveltype level;

    idb = csb->interface->hwptr;

    if (reg_invoke_dialer_huntgroup_member(idb)) {
        if (!csb->nvgen) {
            printf("\nCannot change LAPB configuration of hunt group member");
        }
        return;
    }

    if (csb->nvgen) {
	switch (idb->enctype) {
	  case ET_X25:
	  case ET_LAPB:
	    break;
	  default:
	    return;
	}
	switch (csb->which) {
	  case LAPB_CMD_MODULO:
	    nv_write(idb->lapb_admin_modulo != LAPB_DEFAULT_MODULO, "%s %d",
		     csb->nv_command, idb->lapb_admin_modulo);
	    break;
	  case LAPB_CMD_T1:
	    nv_write(idb->lapb_t1 != LAPB_DEFAULT_T1, "%s %d",
		     csb->nv_command, idb->lapb_t1);
	    break;
	  case LAPB_CMD_T3:
	    nv_write(idb->lapb_t3 != LAPB_DEFAULT_T3, "%s %d",
		     csb->nv_command, idb->lapb_t3);
	    break;
	  case LAPB_CMD_T4:
	    nv_write(idb->lapb_t4 != LAPB_DEFAULT_T4, "%s %d",
		     csb->nv_command, idb->lapb_t4);
	    break;
	  case LAPB_CMD_N1:
	    nv_write(idb->lapb_n1 != LAPB_DEFAULT_N1(idb), "%s %d",
		     csb->nv_command, idb->lapb_n1);
	    break;
	  case LAPB_CMD_N2:
	    nv_write(idb->lapb_n2 != LAPB_DEFAULT_N2, "%s %d",
		     csb->nv_command, idb->lapb_n2);
	    break;
	  case LAPB_CMD_K:
	    nv_write(idb->lapb_admin_k != LAPB_DEFAULT_K, "%s %d",
		     csb->nv_command, idb->lapb_admin_k);
	    break;
	  case LAPB_CMD_PROTOCOL:
	  case LAPB_CMD_HOLDQ:
	    /*
	     *  Obsolete
	     */
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case LAPB_CMD_MODULO:
	if (csb->sense) {
	    idb->lapb_admin_modulo = GETOBJ(int,1);
	} else {
	    idb->lapb_admin_modulo = LAPB_DEFAULT_MODULO;
	}

	/*
	 *  lower the configured k value if necessary
	 */
	if (idb->lapb_admin_k >= idb->lapb_admin_modulo)
	    idb->lapb_admin_k = idb->lapb_admin_modulo - 1;

	if (idb->lapb_admin_modulo != idb->lapb_modulo) {
	    switch (idb->lapb_state) {
	      case LAPB_DISCONNECTED:
	      case LAPB_DISC_SENT:
		idb->lapb_modulo = idb->lapb_admin_modulo;
		idb->lapb_k = idb->lapb_admin_k;
		/*
		 * Update lapb_n1 only if user has never configured N1 value.
		 */
		if (!idb->lapb_override_def_n1)
		    idb->lapb_n1 = LAPB_ADMIN_N1(idb);
		break;
	      case LAPB_SABM_SENT:
	      case LAPB_CONNECTED:
	      case LAPB_FRMR_SENT:
		printf(change_held);
	    }
	}
	break;
      case LAPB_CMD_T1:
	if (csb->sense) {
	    idb->lapb_t1 = GETOBJ(int,1);
	} else {
	    idb->lapb_t1 = LAPB_DEFAULT_T1;
	}
	if (idb->lapb_t4 && (idb->lapb_t1 >= (idb->lapb_t4 * ONESEC))) {
	    printf("\nT4 disabled");
	    idb->lapb_t4 = LAPB_DEFAULT_T4;
	    TIMER_STOP(idb->lapb_t4_timer);
	}
	break;
      case LAPB_CMD_T3:
	if (csb->sense) {
	    idb->lapb_t3 = GETOBJ(int,1);
	} else {
	    idb->lapb_t3 = LAPB_DEFAULT_T3;
	}
	break;
      case LAPB_CMD_T4:
	if (csb->sense) {
	    if (GETOBJ(int,1) && (idb->lapb_t1 >= (GETOBJ(int,1) * ONESEC)))
		printf("\nT4 must be larger than T1");
	    else
		idb->lapb_t4 = GETOBJ(int,1);
	} else {
	    idb->lapb_t4 = LAPB_DEFAULT_T4;
	}

	level = raise_interrupt_level(NETS_DISABLE);

	if (idb->lapb_t4) {
	    if (!TIMER_RUNNING(idb->lapb_t1_timer) &&
		(idb->lapb_state == LAPB_CONNECTED))
		TIMER_START(idb->lapb_t4_timer, idb->lapb_t4 * ONESEC);
	} else {
	    TIMER_STOP(idb->lapb_t4_timer);
	}

	reset_interrupt_level(level);

	break;
      case LAPB_CMD_N1:
	if (csb->sense) {
	    idb->lapb_n1 = GETOBJ(int,1);
	    idb->lapb_override_def_n1 = TRUE;

	    /*
	     *  generate a warning if this may be insufficient for X.25
	     */
	    if (is_x25(idb) && (lapb_payload(idb) < X25_MAX_NON_DATA_PACKET))
		printf("\nN1 allows X.25 %d bytes; X.25 may require %d bytes",
			lapb_payload(idb), X25_MAX_NON_DATA_PACKET);
	} else {
	    idb->lapb_n1 = LAPB_DEFAULT_N1(idb);
	    idb->lapb_override_def_n1 = FALSE;
	}
	break;
      case LAPB_CMD_N2:
	if (csb->sense) {
	    idb->lapb_n2 = GETOBJ(int,1);
	} else {
	    idb->lapb_n2 = LAPB_DEFAULT_N2;
	}
	break;
      case LAPB_CMD_K:
	if (csb->sense) {
	    if (GETOBJ(int,1) >= idb->lapb_admin_modulo)
		printf("\nWindow size too large for modulo set");
	    else
		idb->lapb_admin_k = GETOBJ(int,1);
	} else {
	    idb->lapb_admin_k = LAPB_DEFAULT_K;
	}
	if (idb->lapb_admin_k != idb->lapb_k) {
	    switch (idb->lapb_state) {
	      case LAPB_DISCONNECTED:
	      case LAPB_DISC_SENT:
	      case LAPB_SABM_SENT:
		idb->lapb_k = idb->lapb_admin_k;
		break;
	      case LAPB_CONNECTED:
	      case LAPB_FRMR_SENT:
		printf(change_held);
	    }
	}
	break;
      case LAPB_CMD_PROTOCOL:
	if (csb->sense) {
	    idb->lapb_protocol = GETOBJ(int,1);
	} else {
	    idb->lapb_protocol = LINK_IP;
	}
	break;
      case LAPB_CMD_HOLDQ:
	printf("\n%%Obsolete command--use: hold-queue %d out", GETOBJ(int,1));
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

    reg_invoke_interface_cfg_modified(idb);
}

/*
 * lapb_default_n1
 * Determine an N1 parameter given the multitude
 * layers we must deal with
 */

int lapb_default_n1 (register hwidbtype *idb)
{
    int size;
    lapb_mode *mode;

    SET_MODE(idb, mode);

    size = mode->i.overhead;

    if (is_x25(idb))
	size += x25_default_n1(idb);
    else if (is_mlapb_only(idb))
	size += LAPB_PID_BYTES;

    if (idb->comp_db && idb->comp_db->comp_configed != COMP_NOT_ENABLED) {
	/*
	 * if compression is enabled, include the compression header.
	 */
	size += COMPHEADERBYTES;
    }

    size += idb->maxmtu;
    return((size * 8) + LAPB_CRC_BITS);
}

/*
 * lapb_admin_n1
 * Determine an N1 parameter when the new modulo is configured
 */

int lapb_admin_n1 (register hwidbtype *idb)
{
    int size;

    size = (idb->lapb_admin_modulo == LAPB_MODULO_8) ? LAPB_OVERHEAD_8 : 
						       LAPB_OVERHEAD_128;

    if (is_x25(idb))
	size += x25_default_n1(idb);
    else if (is_mlapb_only(idb))
	size += LAPB_PID_BYTES;

    if (idb->comp_db && idb->comp_db->comp_configed != COMP_NOT_ENABLED) {
	/*
	 * if compression is enabled, include the compression header.
	 */
	size += COMPHEADERBYTES;
    }

    size += idb->maxmtu;
    return((size * 8) + LAPB_CRC_BITS);
}

/*
 * lapb_min_n1
 * Determine the minimum N1 parameter
 */

int lapb_min_n1 (register hwidbtype *idb)
{
    int size;

    size = 128; /* default packet size */
    size += (idb->lapb_admin_modulo == LAPB_MODULO_8) ? 
	    LAPB_OVERHEAD_8 : LAPB_OVERHEAD_128;

    if (is_x25(idb))
	size += x25_default_n1(idb);
    else if ((idb->enctype == ET_LAPB) && (idb->lapb_protocol == LINK_ILLEGAL))
	size += LAPB_PID_BYTES;

    return((size * 8) + LAPB_CRC_BITS);
}

/*
 * lapb_change_mtu
 *
 * A user has configured a different physical MTU size for this
 * interface. Update N1 to reflect the configuration change.  
 */
void lapb_change_mtu(hwidbtype *hwidb, idbtype *swidb, int delta)
{
    reg_invoke_lapb_update_n1(hwidb);
}

/*
 * lapb_update_n1
 *
 * A user has configured a different physical MTU size, a different
 * L3 modulo or compression for this interface.  
 * Update N1 value to reflect the current configuration.
 */
void lapb_update_n1 (hwidbtype *hwidb)
{

    if (hwidb->lapb_override_def_n1) {
	/*
	 * User has configured N1, validate if this N1 is still in 
	 * the allowed range, if not, set lapb_n1 back to default.
	 */
	if ((hwidb->lapb_n1 < lapb_min_n1(hwidb)) ||
	    (hwidb->lapb_n1 > LAPB_DEFAULT_N1(hwidb))) {
	    hwidb->lapb_n1 = LAPB_DEFAULT_N1(hwidb);  
	    hwidb->lapb_override_def_n1 = FALSE;
	}	
    } else {
	/*
	 * User has not configured N1, adjust lapb_n1 to reflect the 
	 * current configuration change.
	 */
	hwidb->lapb_n1 = LAPB_DEFAULT_N1(hwidb);  
    }
}

/*
 * copy_lapb_fields
 * copy lapb fields from one idb to another.  Used to propagate info from
 * dialer hunt groups to interfaces.
 */
void copy_lapb_fields (hwidbtype *idb1, hwidbtype *idb2)
{
  leveltype level;
  
  if (idb2->lapb_admin_modulo != idb1->lapb_admin_modulo) {
    idb2->lapb_admin_modulo = idb1->lapb_admin_modulo;
    if (idb2->lapb_admin_k >= idb2->lapb_admin_modulo)
      idb2->lapb_admin_k = idb2->lapb_admin_modulo - 1;
    if (idb2->lapb_admin_modulo != idb2->lapb_modulo) {
      if ((idb2->lapb_state == LAPB_DISCONNECTED) ||
	  (idb2->lapb_state == LAPB_DISC_SENT)) {
	idb2->lapb_modulo = idb2->lapb_admin_modulo;
	idb2->lapb_k = idb2->lapb_admin_k;
	/*
	 * Update lapb_n1 only if user has never configured N1 value.
	 */
	if (!idb2->lapb_override_def_n1)
	    idb2->lapb_n1 = LAPB_ADMIN_N1(idb2);
      }
    }
  }
  if (idb2->lapb_t1 != idb1->lapb_t1) {
    idb2->lapb_t1 = idb1->lapb_t1;
    if (idb2->lapb_t1 >= (idb2->lapb_t4 * ONESEC)) {
      idb2->lapb_t4 = LAPB_DEFAULT_T4;
      TIMER_STOP(idb2->lapb_t4_timer);
    }
  }
  idb2->lapb_t3 = idb1->lapb_t3;
  if (idb2->lapb_t4 != idb1->lapb_t4) {
    idb2->lapb_t4 = idb1->lapb_t4;
    level = raise_interrupt_level(NETS_DISABLE);
    if (idb2->lapb_t4) {
      if (!TIMER_RUNNING(idb2->lapb_t1_timer) &&
	  (idb2->lapb_state == LAPB_CONNECTED))
	TIMER_START(idb2->lapb_t4_timer, idb2->lapb_t4 * ONESEC);
    } else {
      TIMER_STOP(idb2->lapb_t4_timer);
    }
    reset_interrupt_level(level);
  }
  idb2->lapb_n1 = idb1->lapb_n1;
  idb2->lapb_n2 = idb1->lapb_n2;
  if (idb2->lapb_admin_k != idb1->lapb_admin_k) {
    idb2->lapb_admin_k = idb1->lapb_admin_k;
    if (idb2->lapb_admin_k != idb2->lapb_k) {
      if ((idb2->lapb_state == LAPB_DISCONNECTED) ||
	  (idb2->lapb_state == LAPB_DISC_SENT)) {
	idb2->lapb_k = idb2->lapb_admin_k;
      }
    }
  }
  /* this gets hosed in lapb_setup when called by the dialer... */
  idb2->lapb_dce = idb1->lapb_dce;
  idb2->lapb_protocol = idb1->lapb_protocol;
  idb2->lapb_holdq.maximum = idb1->lapb_holdq.maximum;
}

/*
 * lapb_linestate
 * Return current line protocol status
 */

static boolean lapb_linestate (hwidbtype *idb)
{
    if (reg_invoke_dialer_spoofing_prot(idb))
	return(TRUE);
    else
	return(idb->lapb_upstate);
}

/*
 * lapb_extract_addr
 *
 * Given a LAPB packet, extract the requested 'MAC' level address
 * into a hwaddrtype structure.
 */
static boolean lapb_extract_addr (
    paktype *pak,
    hwaddrtype *address,
    enum SRCDST which)
{
    if (pak->enctype != ET_LAPB)
	return(FALSE);

    if (pak->if_input &&
	(pak->if_input->hwptr->lapb_protocol == LINK_ILLEGAL)) {
	address->type = STATION_CISCO_MLAPB;
	address->length = STATIONLEN_CISCO_MLAPB;
    } else {
	address->type = STATION_LAPB;
	address->length = STATIONLEN_LAPB;
    }

    address->target_type = TARGET_UNICAST;
    address->addr[0] = 0;
    return(TRUE);
}

/*
 * lapb_parse_packet
 *
 * Set up a packet buffer so that the network level protocol fields
 * are initialized.
 *
 * If data is NULL, pak->datagramstart is assumed to point to the
 * start of the frame to parse. No attempt will be made to align
 * the frame to The Line.
 *
 */

static iqueue_t lapb_parse_packet (
    hwidbtype *idb,
    paktype *pak,
    uchar *data)
{
    charlong firstlong;
    int encsize;
    lapb_mode *mode;

    SET_MODE(idb, mode);

    /*
     * Grab the encapsulation type, and figure out the encapsulation size.
     */
    pak->enctype = idb->enctype;
    encsize = mode->i.overhead;

    if (is_mlapb_only(idb)) {
	/*
	 * Check to see if building a frame in a virgin packet, or whether
	 * we're just setting up media pointers into the frame
	 */
	if (data) {
	    /*
	     * We've got a virgin packet.
	     */
	    firstlong.d.lword = GETLONG(data);
	} else {
	    /*
	     * Soiled packet. Start from datagramstart
	     */
	    firstlong.d.lword = GETLONG(pak->datagramstart);
	}

	if ((firstlong.d.byte[LAPB_CONTROL_INX] & LAPB_IFRAME_M) ==
	    LAPB_IFRAME_V)
	    encsize = mode->i.overhead + LAPB_PID_BYTES;
    }


    /*
     * If we're building a virgin frame, we have to set datagramstart
     */
    if (data) {
	pak->datagramstart = pak_center(pak) - encsize;
    }
    pak->mac_start     = pak->datagramstart;
    pak->addr_start    = NULL;
    pak->rif_start     = NULL;
    pak->riflen	       = 0;
    pak->info_start    = NULL;
    pak->network_start = pak->datagramstart + encsize;

    /*
     * Record just how big this encapsulation was rather than every protocol
     * have it's own fun and wacky way to work it out.
     */
    pak->encsize = pak->network_start - pak->datagramstart;
    return(lapb_input);
}

/*
 * LAPB deencapsulation
 */

void lapb_deencapsulate (hwidbtype *idb)
{
    register leveltype level;
    idbtype *swidb;

    no_compression(idb);
    lapb_reset(idb, "New config encapsulation");

    /*
     * remove this idb from the lapbidbq
     */
    if (idb->lapbidb) {
        unqueue(&lapbidbQ, idb->lapbidb);
        free(idb->lapbidb);
        idb->lapbidb = NULL;
    }

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *  reset the primary queue vectors to the standard functions (for X.25
     *  interfaces, which used its own routines) or the secondary vectors
     *  (for LAPB interfaces, which may have used the fancy queuing routines)
     */
    if (idb->oqueue == lapb_x25_oqueue || idb->lapb_oqueue == NULL)
	idb->oqueue = holdq_enqueue;
    else
	idb->oqueue = idb->lapb_oqueue;
    if (idb->oqueue_dequeue == lapb_x25_oqueue_dequeue || idb->lapb_oqueue_dequeue == NULL)
	idb->oqueue_dequeue = holdq_dequeue;
    else
	idb->oqueue_dequeue = idb->lapb_oqueue_dequeue;

    idb->lapb_oqueue = NULL;
    idb->lapb_oqueue_dequeue = NULL;

#ifdef NOBODY_READS_ME
    nlapb--;
#endif

    if (is_mlapb_only(idb)) {
	/*
	 *  Remove this interface from bridging processing.
	 */
	FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	    if (swidb->span_ptr) {
		idb->mlapb_bridge = FALSE;
		idb->span_bridge_process_force &= ~SPAN_BR_PROC_MLAPB_BRIDGE;
	    }
	}
    }
    reset_interrupt_level(level);
}

/*
 * lapb_setup
 * Set up a LAPB encapsulation
 */

boolean lapb_setup (parseinfo *csb, hwidbtype *idb)
{
    idbtype *swidb;
    register leveltype level;
    dialerdbtype *ddb;
    boolean huntgroup_leader;

    /*
     * shut off interrupts 
     */
    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *  check for existing, prohibited features
     */
    if (is_lapb(idb) && !is_x25(idb)) {
	/*
	 * TCP header compression is not compatible with LAPB encapsulation
	 */
	if (idb->tcp_compress) {
	    reset_interrupt_level(level);
	    printf("\n%%Please remove TCP header compression before "
		   "encapsulating LAPB");
	    return(FALSE);
	}

	/*
	 * Bridging is only permitted on a multiprotocol LAPB encapsulation
	 */
	if (GETOBJ(int,3) != LINK_ILLEGAL) {
	    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
		if (swidb->span_ptr) { 
		    reset_interrupt_level(level);
		    printf("\n%%Please remove bridging before "
			   "encapsulating LAPB");
		    return(FALSE);
		}
	    }
	}
    }

    /*
     * set encapsulation configuration
     */
    idb->lapb_dce = LAPBX25_DTE;
    idb->lapb_protocol = LINK_IP;

    if (csb != NULL) {
	if (GETOBJ(int,2) & ENCAPS_LAPB_X25_DCE)
	    idb->lapb_dce = LAPBX25_DCE;

	idb->lapb_protocol = GETOBJ(int,3);
    }

    if (idb->lapbidb) {
	/*
	 * need an errmsg here probably.  this should never happen, but if
	 * it does, we should know about it.
	 */
	unqueue(&lapbidbQ, idb->lapbidb);
	free(idb->lapbidb);
	idb->lapbidb = NULL;
    }


    /*
     * initialize protocol dependant fields
     * (hunt group leaders need these set up too, as these values are copied
     * to the hunt group members later)
     */
    idb->lapb_admin_modulo = LAPB_DEFAULT_MODULO;
    idb->lapb_modulo = idb->lapb_admin_modulo;
    idb->lapb_t1 = LAPB_DEFAULT_T1;
    idb->lapb_t3 = LAPB_DEFAULT_T3;
    idb->lapb_t4 = LAPB_DEFAULT_T4;
    idb->lapb_n1 = LAPB_DEFAULT_N1(idb); /* bits */
    idb->lapb_override_def_n1 = FALSE;
    idb->lapb_n2 = LAPB_DEFAULT_N2;
    idb->lapb_admin_k = LAPB_DEFAULT_K;
    idb->lapb_k = idb->lapb_admin_k;

    huntgroup_leader = reg_invoke_dialer_huntgroup_leader(idb);
    if (!huntgroup_leader) {
	/* if this is not a huntgroup leader, allocate context for the
	 * lapb processing, and add it to the queue.  Hunt group leaders
	 * don't actually run LAPB, so they don't need this context.
	 */
	idb->lapbidb = malloc(sizeof(lapbidbtype));
	if (!idb->lapbidb) {
	    reset_interrupt_level(level);
	    return(FALSE);
	}
	idb->lapbidb->hwidb = idb;  /* back pointer to hwidb */
	enqueue(&lapbidbQ, idb->lapbidb);
    }
    /*
     * reset interface queueing
     *
     * NB: this will cause the primary and secondary queuing vectors
     *     to be initialized in lapb_check_output_vectors(), which is
     *     invoked from the registry _interface_cfg_modified
     */
    set_default_queueing(idb);
    holdq_reset(idb);

    /*
     * if the idb is not a hunt group leader, and the queueing vectors were
     * not installed successfully, report failure.
     */
    if (!huntgroup_leader &&
	((idb->lapb_oqueue == NULL) || (idb->lapb_oqueue_dequeue == NULL))) {
	reset_interrupt_level(level);
	return(FALSE);
    }

    /* arrange to show LAPB parameters in "show interface" */
    idb->show_support = lapb_interface;  /* sanjeev, CSCdi06440 */

    if (huntgroup_leader) {
	/* set call limit for group. if we're running x25 over lapb,
	 * x25_setup will update this.
	 */
        ddb = get_dialerdb(idb);
        if (!ddb) {
	    reset_interrupt_level(level);
	    return(FALSE);
        }
	ddb->dialer_max_call = DIALER_MAX_P2P_CALLS;
	/* all the necessary initialization has now been completed if this
	 * is a hunt group leader.
	 */
	reset_interrupt_level(level);
	return(TRUE);
    }

    /*
     * Protocol handling vectors
     */
    idb->vencap = lapb_vencapsulate;
    idb->bridge_vencap = (bridge_vencap_t)return_null;
    /*
     * x25_setup() over writes deencap with x25_deencapsulate after it
     * calls lapb_setup()
     */
    idb->deencap = lapb_deencapsulate;
    idb->getlink = lapb_getlink;
    idb->iqueue = lapb_input;
    idb->parse_packet = lapb_parse_packet;
    idb->extract_hwaddr = invalidate_hwaddr;
    idb->extract_snpa = lapb_extract_addr;
    idb->broadcast = lapb_bcast;
    idb->periodic = NULL;
    idb->lineaction = lapb_cstate;
    idb->linestate = lapb_linestate;

    /*
     *  the encapsulation size is used by the drivers to determine how
     *  much of the frame preamble to discard--we must get the entire
     *  frame, so simply set the HDLC framing encapsulation size
     */
    idb->encsize = HDLC_ENCAPBYTES;

    lapb_reset(idb, "LAPB initialized");

    idb->lapb_requeue_pak = NULL;
    queue_init(&idb->lapb_u_s_q, 0);
    queue_init(&idb->lapb_unackq, 0);
    queue_init(&idb->lapb_holdq, 0);
    queue_init(&idb->lapb_retxq, 0);

    idb->lapb_iframessent = 0;
    idb->lapb_iframesrcvd = 0;
    idb->lapb_rnrssent = 0;
    idb->lapb_rnrsrcvd = 0;
    idb->lapb_rejssent = 0;
    idb->lapb_rejsrcvd = 0;
    idb->lapb_sabmssent = 0;
    idb->lapb_sabmsrcvd = 0;
    idb->lapb_frmrssent = 0;
    idb->lapb_frmrsrcvd = 0;
    idb->lapb_discssent = 0;
    idb->lapb_discsrcvd = 0;

    idb->si_lapb_iframessent = 0;
    idb->si_lapb_iframesrcvd = 0;
    idb->si_lapb_rnrssent = 0;
    idb->si_lapb_rnrsrcvd = 0;
    idb->si_lapb_rejssent = 0;
    idb->si_lapb_rejsrcvd = 0;
    idb->si_lapb_sabmssent = 0;
    idb->si_lapb_sabmsrcvd = 0;
    idb->si_lapb_frmrssent = 0;
    idb->si_lapb_frmrsrcvd = 0;
    idb->si_lapb_discssent = 0;
    idb->si_lapb_discsrcvd = 0;

    /*
     *  MIB-specific values
     */
    idb->lapb_mib_state = LAPB_MIBMODE_DISCON;
    idb->lapb_state_reason = LAPB_MIBREASON_INITIAL;
    idb->lapb_mib_state_changes = 0;
    idb->lapb_tx_deferrals = 0;
    idb->lapb_t1_retx = 0;
    idb->lapb_frmr_txlen = 0;
    idb->lapb_frmr_rxlen = 0;

    idb->si_lapb_mib_state_changes = 0;
    idb->si_lapb_tx_deferrals = 0;
    idb->si_lapb_t1_retx = 0;

#ifdef NOBODY_READS_ME
    nlapb++;
#endif

    /*
     *  initialize the protocol state so that the timer task will bring it up
     */
    idb->lapb_state = LAPB_DISCONNECTED;
    idb->lapb_tx_state = LAPB_TX_INACTIVE;
    idb->lapb_rx_state = LAPB_RX_INACTIVE;
    idb->lapb_exit_rnr_rej = FALSE;
    /*
     * Start timer T1
     */
    TIMER_START(idb->lapb_t1_timer, 1);

    /*
     *  if needed, fork off a timer task
     */
    if (lapb_tf == 0)
	lapb_tf = cfork((forkproc *) lapb_timer, 0L, 0, "LAPB Timer", 0);

    /*
     *  per-software-IDB setup
     */
    if (is_mlapb_only(idb)) {
	FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	    /*
	     * If bridge circuit group was previously enabled on this interface
	     * then remove the circuit group.  
	     */
	    if (swidb->span_ptr) {
		reg_invoke_bridge_circuit_remove(swidb);
		idb->mlapb_bridge = TRUE;
		idb->span_bridge_process_force |= SPAN_BR_PROC_MLAPB_BRIDGE;
	    }
	}
    }
    reset_interrupt_level(level);
    return(TRUE);
}

/*
 *  lapb_x25_oqueue
 *  X.25-specific output queueing procedure, which must maintain strict
 *  serialization of the packets as proffered.
 */
boolean lapb_x25_oqueue (
    hwidbtype *idb,
    paktype *pak,
    enum HEADTAIL which)
{
    register leveltype level;

    if (which == HEAD) {
	/*
	 *  This is a real dangerous thing, but such is life currently.
	 *  The frame was handed off to the driver, but the driver
	 *  is giving it back to us because of resource problems--
	 *  save it in an idb field to ensure that it's the next frame
	 *  transmitted; this avoids re-ordering the output stream.
	 */

	if (idb->lapb_requeue_pak) {
	    datagram_done(idb->lapb_requeue_pak);	/* frame lost */
	    idb->output_qcount--;
	}

	idb->lapb_requeue_pak = pak;

	/*
	 *  do NOT nudge the driver (the MCI driver will recurse,
	 *  overflowing the stack), but bump the output queue
	 *  count so the driver will try again
	 */
	idb->output_qcount++;
	return(FALSE);
    }

    /*
     *  the dialer may give us a raw/out-of-band frame to send without
     *  invoking the encapsulation function (which will prevent any other
     *  use of ET_HDLC)
     */
    if ((pak->enctype == ET_HDLC) && is_ddr(idb)) {
	/*
	 * V25.bis signaling packet
	 */
	if (! pak_enqueue(&idb->lapb_u_s_q, pak)) {
	    datagram_done(pak);
	    return(FALSE);
	}

	idb->output_qcount++;
	(*idb->soutput)(idb);
	return(TRUE);
    }

    if (pak_enqueue(&idb->lapb_holdq, pak)) {
	idb->output_qcount++;
	/*
	 *  if we're able to send an I-frame, nudge the driver
	 */
	if (lapb_can_tx_i(idb)) {
	    (*idb->soutput)(idb);
	} else {
	    idb->lapb_tx_deferrals++;
	}
	return(TRUE);
    }

    datagram_done(pak);
    level = raise_interrupt_level(NETS_DISABLE);
    lapb_reset(idb, "Cannot send X.25 packet");
    lapb_u_frame(idb, pak, LAPB_SABM, TRUE);
    idb->lapb_state = LAPB_SABM_SENT;
    TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
    NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP, LAPB_MIBREASON_OTHER)
    reset_interrupt_level(level);
    return(FALSE);
}

/*
 *  lapb_x25_oqueue_dequeue
 *  X.25-specific output dequeueing procedure
 */
paktype *lapb_x25_oqueue_dequeue (
    hwidbtype *idb)
{
    paktype *pak;

    if ((pak = pak_dequeue(&idb->lapb_holdq))) {
	idb->output_qcount--;
    }

    return(pak);
}

/*
 *  lapb_check_output_vectors
 *  Some functions will reset the idb->oqueue and idb->oqueue_dequeue
 *  vectors out from underneath LAPB; we need to insist on LAPB's
 *  private routines for these vectors, but the new pointers represent
 *  secondary vectors for output handling that must also be invoked as
 *  appropriate.
 */
void lapb_check_output_vectors (hwidbtype *idb)
{
  register leveltype level;

    if (idb == NULL)
	return;
    /*
     * if this idb is the leader for a hunt group, process each of the
     * idbs in the hunt group, but don't continue with the leader's idb,
     * to avoid breaking isdn d-channels.
     */
    if (reg_invoke_dialer_huntgroup_leader(idb)) {
	reg_invoke_for_all_dialer_members(idb, lapb_check_output_vectors);
	return;
    }

    if (! is_lapb(idb))
	return;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *  For X.25 encapsulations, there is no alternative to our own
     *  primary and secondary vectors; the following code will ensure
     *  they're set to the proper values.
     */

    /*
     *  idb->oqueue vector:
     *  LAPB encapsulations must have lapb_output() in idb->oqueue;
     *  update the secondary vector if this is not the case
     */
    if (is_x25(idb)) {
	idb->lapb_oqueue = lapb_x25_oqueue;
	idb->oqueue = x25_output;
    } else if (idb->oqueue == lapb_output) {
	if (! idb->priority_list) {
	    idb->lapb_oqueue = holdq_enqueue;
	}

	/*
	 *  if fancy queueing is in effect, the secondary vector is ok as-is
	 */
    } else {
	/*
	 *  the queuing discipline has been modified--
	 *  save the new function as the secondary vector and restore ours
	 */
	idb->lapb_oqueue = idb->oqueue;
	idb->oqueue = lapb_output;
    }

    /*
     *  idb->oqueue_dequeue vector:
     *  LAPB encapsulations must have lapb_pull_frame() in idb->oqueue_dequeue;
     *  update the secondary vector if this is not the case
     */
    if (is_x25(idb)) {
	idb->lapb_oqueue_dequeue = lapb_x25_oqueue_dequeue;
	idb->oqueue_dequeue = lapb_pull_frame;
    } else if (idb->oqueue_dequeue == lapb_pull_frame) {
	if (! idb->priority_list) {
	    idb->lapb_oqueue_dequeue = holdq_dequeue;
	}

	/*
	 *  if fancy queueing is in effect, the secondary vector is ok as-is
	 */
    } else {
	/*
	 *  the queuing discipline has been modified--
	 *  save the new function as the secondary vector and restore ours
	 */
	idb->lapb_oqueue_dequeue = idb->oqueue_dequeue;
	idb->oqueue_dequeue = lapb_pull_frame;
    }

    reset_interrupt_level(level);
}

/*
 * lapb_cstate
 * Called by the cstat handler when the carrier state changes
 * This routine is *not* an interface vector routine.
 */

void lapb_cstate (hwidbtype *idb)
{
    register leveltype level;
    lapb_mode *mode;

    SET_MODE(idb, mode);

    /*
     * shut off interrupts--
     * this will prevent incoming frames from being processed
     * while we're making a state transition
     */
    level = raise_interrupt_level(NETS_DISABLE);

    if (idb->state != IDBS_UP) {
	/*
	 *  we have a physical outage--
	 *  wait T3 msec. before declaring the link dead;
	 *  if the physical outage doesn't last beyond T3, no state
	 *  change occurs
	 */
	if (idb->lapb_upstate && idb->lapb_t3) {
	    if (!TIMER_RUNNING(idb->lapb_t3_timer))
		TIMER_START(idb->lapb_t3_timer, idb->lapb_t3);
	}

	if (!TIMER_RUNNING(idb->lapb_t3_timer) ||
	    !SLEEPING(idb->lapb_t3_timer))
	    lapb_reset(idb, "LAPB physical layer down");
    } else if (idb->lapb_upstate && idb->lapb_t3) {
	/*
	 *  we had a brief physical outage--no harm done
	 */
	if (TIMER_RUNNING(idb->lapb_t3_timer))
	    TIMER_STOP(idb->lapb_t3_timer);
    /*
     * We check for LAPB_DISCONNECTED here in case carrier has come up,
     * and the other side sent us a SABM already which we replied to,
     * and then lapb_cstate gets called.  But if we miss a down transition,
     * we are in trouble, as this assumes that cstate has been called and
     * we've called lapb_reset to say we are disconnected.
     */
    } else if (idb->lapb_state == LAPB_DISCONNECTED) {
	/*
	 *  we need to initialize LAPB
	 */
	lapb_reset(idb, "LAPB physical layer up");
	idb->lapb_state = LAPB_SABM_SENT;
	NEW_MIB_STATE(idb, LAPB_MIBMODE_SETUP, mode->mib_reset_reason);
	lapb_u_frame(idb, NULL, LAPB_SABM, TRUE);
	TIMER_START(idb->lapb_t1_timer, idb->lapb_t1);
    }

    reset_interrupt_level(level);
}

/*
 * Output correct information for the show interface command.
 * Display is done as in if/network.c by subtracting variables from shadows.
 */

void lapb_interface (register hwidbtype *idb)
{
    printf("\n  LAPB %s, modulo %d, k %d, N1 %d, N2 %d",
	   IS_DCE(idb, lapb_dce) ? "DCE" : "DTE",
	   idb->lapb_modulo,
	   idb->lapb_k, 
	   (idb->lapb_override_def_n1 ? idb->lapb_n1 : LAPB_DEFAULT_N1(idb)),
	   idb->lapb_n2);
    if ((idb->lapb_admin_modulo != idb->lapb_modulo) ||
	(idb->lapb_admin_k != idb->lapb_k))
	printf("\n      (on reset, LAPB modulo %d, k %d, N1 %d)",
		idb->lapb_admin_modulo, idb->lapb_admin_k, 
		(idb->lapb_override_def_n1 ? idb->lapb_n1 : LAPB_ADMIN_N1(idb)));
    printf("\n      T1 %d, interface outage (partial T3) %d, T4 %d",
	   idb->lapb_t1, idb->lapb_t3, idb->lapb_t4);
    if (idb->enctype == ET_LAPB) {
	if (idb->lapb_protocol == LINK_ILLEGAL)
	    printf(", Multi-protocol");
	else
	    printf(", Protocol %s", print_linktype(idb->lapb_protocol));
    }

    if (reg_invoke_dialer_huntgroup_leader(idb)) {
	/*
	 * A hunt group leader doesn't run LAPB, so don't try to show
	 * LAPB-specific state or counters
	 */
	return;
    }
    printf("\n      State %s, VS %d, VR %d, Remote VR %d, Retransmissions %d",
	lapb_prstate(idb),
	idb->lapb_vs, idb->lapb_vr, idb->lapb_rvr, idb->lapb_retcnt);

    printf("\n      Queues: U/S frames %d, I frames %d, unack. %d, reTx %s%d",
	QUEUESIZE(&idb->lapb_u_s_q), QUEUESIZE(&idb->lapb_holdq),
	QUEUESIZE(&idb->lapb_unackq), idb->lapb_requeue_pak ? "1 + " : "",
	QUEUESIZE(&idb->lapb_retxq));
    if (QUEUESIZE(&idb->lapb_unackq) == idb->lapb_k)
       printf(", Window is closed");

    printf("\n      IFRAMEs %d/%d RNRs %d/%d REJs %d/%d SABM/Es %d/%d "
	   "FRMRs %d/%d DISCs %d/%d",
	(idb->lapb_iframessent - idb->si_lapb_iframessent),
        (idb->lapb_iframesrcvd - idb->si_lapb_iframesrcvd),
	(idb->lapb_rnrssent - idb->si_lapb_rnrssent),
	(idb->lapb_rnrsrcvd - idb->si_lapb_rnrsrcvd),
	(idb->lapb_rejssent - idb->si_lapb_rejssent),
	(idb->lapb_rejsrcvd - idb->si_lapb_rejsrcvd),
	(idb->lapb_sabmssent - idb->si_lapb_sabmssent),
	(idb->lapb_sabmsrcvd - idb->si_lapb_sabmsrcvd),
	(idb->lapb_frmrssent - idb->si_lapb_frmrssent),
	(idb->lapb_frmrsrcvd - idb->si_lapb_frmrsrcvd),
	(idb->lapb_discssent - idb->si_lapb_discssent),
	(idb->lapb_discsrcvd - idb->si_lapb_discsrcvd));
}

/*
 * lapb_clearcounters
 * Clear LAPB counters when a "clear counters" command is issued.  Clear is done
 * as in if/network.c by copying the data into shadow variables.
 */

void lapb_clearcounters (hwidbtype *idb)
{
    idb->si_lapb_iframessent = idb->lapb_iframessent;
    idb->si_lapb_iframesrcvd = idb->lapb_iframesrcvd;
    idb->si_lapb_rnrssent = idb->lapb_rnrssent;
    idb->si_lapb_rnrsrcvd = idb->lapb_rnrsrcvd;
    idb->si_lapb_rejssent = idb->lapb_rejssent;
    idb->si_lapb_rejsrcvd = idb->lapb_rejsrcvd;
    idb->si_lapb_sabmssent = idb->lapb_sabmssent;
    idb->si_lapb_sabmsrcvd = idb->lapb_sabmsrcvd;
    idb->si_lapb_frmrssent = idb->lapb_frmrssent;
    idb->si_lapb_frmrsrcvd = idb->lapb_frmrsrcvd;
    idb->si_lapb_discssent = idb->lapb_discssent;
    idb->si_lapb_discsrcvd = idb->lapb_discsrcvd;
    idb->si_lapb_mib_state_changes = idb->lapb_mib_state_changes;
    idb->si_lapb_tx_deferrals = idb->lapb_tx_deferrals;
    idb->si_lapb_t1_retx = idb->lapb_t1_retx;
}

/* When using Reliable link, the default value for parameter N2
 * SHOULD be 3 attempts per RFC1663.
 */
#define LAPB_PPP_DEFAULT_N2	3
/*
 *  lapb_payload(idb)
 *  Returns the maximum number of data bytes that can be carried in an I-frame
 */
int
lapb_payload (hwidbtype *idb)
{
    lapb_mode *mode;

    SET_MODE(idb, mode);

    if (!is_lapb(idb))
	return 0;

    return(((idb->lapb_n1 - LAPB_CRC_BITS) / 8) - mode->i.overhead);
}

/*
 * ppp_lapb_setup
 * Set up LAPB in conjunction with PPP encapsulation
 */
void lapb_ppp_setup (hwidbtype *idb, uchar address, uchar window, int maxhdr)
{
	/*
     * set encapsulation configuration
     */
    idb->lapb_dce = LAPBX25_DTE;
    idb->lapb_protocol = LINK_PPP;

	if (address == LAPB_ADDRESSB)
		idb->lapb_dce = LAPBX25_DCE;

	idb->enctype = ET_PPP; /* Just making sure it has been set corectly */

	/*
	 * This flag provides the means to accertain if ppp is using lapb
	 */
	idb->ppp_lapb = TRUE;

    /*
     * reset interface queueing
     *
     * NB: this will cause the primary and secondary queuing vectors
     *     to be initialized in lapb_check_output_vectors(), which is
     *     invoked from the registry _interface_cfg_modified
     */
    set_default_queueing(idb);
    holdq_reset(idb);

    /*
     * Protocol handling vectors
     */
	idb->ppp_new_vencap = lapb_vencapsulate;
	idb->ppp_new_lineaction = lapb_cstate;

	/*
	 * Additional user information for "show interface" command.
	 */
	idb->ppp_new_show_support = lapb_interface;

	/*
	 * Save the original I/O PPP vectors.
	 */
	idb->ppp_save_parse_packet = idb->parse_packet;
	idb->parse_packet = lapb_parse_packet;

    /*
     * Allocate the lapbidb and put it on the queue
     */

    /*
     * First check to see if it is already there.  If so, something is
     * very wrong, but delete it and make a new one...
     */

    if (idb->lapbidb) {
       /*
        * need an errmsg here probably.  this should never happen, but if
	* it does, we should know about it.
	*/
        unqueue(&lapbidbQ, idb->lapbidb);
        free(idb->lapbidb);
        idb->lapbidb = NULL;
    }

    idb->lapbidb = malloc(sizeof(lapbidbtype));
    if (!idb->lapbidb) {
        return;
    }
    idb->lapbidb->hwidb = idb;	/* back pointer to hwidb */
    enqueue(&lapbidbQ, idb->lapbidb);

	/*
     * Protocol dependant fields
     */
    lapb_reset(idb, "LAPB initialized by PPP");
	if (window < LAPB_MODULO_8)
	    idb->lapb_admin_modulo = LAPB_MODULO_8;
	else
		idb->lapb_admin_modulo = LAPB_MODULO_128;
    idb->lapb_modulo = idb->lapb_admin_modulo;
    idb->lapb_t1 = LAPB_DEFAULT_T1;	/* 3 seconds */
    idb->lapb_t3 = LAPB_DEFAULT_T3;
    idb->lapb_t4 = LAPB_DEFAULT_T4;
    idb->lapb_n1 = LAPB_DEFAULT_N1(idb) + maxhdr; /* max tx-able frame size */
    idb->lapb_n2 = LAPB_PPP_DEFAULT_N2;		/* 3 retries */
    idb->lapb_admin_k = window;
    idb->lapb_k = idb->lapb_admin_k;
    idb->lapb_requeue_pak = NULL;
    queue_init(&idb->lapb_u_s_q, 0);
    queue_init(&idb->lapb_unackq, 0);
    queue_init(&idb->lapb_holdq, 0);
    queue_init(&idb->lapb_retxq, 0);

    idb->lapb_iframessent = 0;
    idb->lapb_iframesrcvd = 0;
    idb->lapb_rnrssent = 0;
    idb->lapb_rnrsrcvd = 0;
    idb->lapb_rejssent = 0;
    idb->lapb_rejsrcvd = 0;
    idb->lapb_sabmssent = 0;
    idb->lapb_sabmsrcvd = 0;
    idb->lapb_frmrssent = 0;
    idb->lapb_frmrsrcvd = 0;
    idb->lapb_discssent = 0;
    idb->lapb_discsrcvd = 0;

    idb->si_lapb_iframessent = 0;
    idb->si_lapb_iframesrcvd = 0;
    idb->si_lapb_rnrssent = 0;
    idb->si_lapb_rnrsrcvd = 0;
    idb->si_lapb_rejssent = 0;
    idb->si_lapb_rejsrcvd = 0;
    idb->si_lapb_sabmssent = 0;
    idb->si_lapb_sabmsrcvd = 0;
    idb->si_lapb_frmrssent = 0;
    idb->si_lapb_frmrsrcvd = 0;
    idb->si_lapb_discssent = 0;
    idb->si_lapb_discsrcvd = 0;

    /*
     *  MIB-specific values
     */
    idb->lapb_mib_state = LAPB_MIBMODE_DISCON;
    idb->lapb_state_reason = LAPB_MIBREASON_INITIAL;
    idb->lapb_mib_state_changes = 0;
    idb->lapb_tx_deferrals = 0;
    idb->lapb_t1_retx = 0;
    idb->lapb_frmr_txlen = 0;
    idb->lapb_frmr_rxlen = 0;

    idb->si_lapb_mib_state_changes = 0;
    idb->si_lapb_tx_deferrals = 0;
    idb->si_lapb_t1_retx = 0;

#ifdef NOBODY_READS_ME
    nlapb++;
#endif

    /*
     *  initialize the protocol state so that the timer task will bring it up
     */
    idb->lapb_state = LAPB_DISCONNECTED;
    idb->lapb_tx_state = LAPB_TX_INACTIVE;
    idb->lapb_rx_state = LAPB_RX_INACTIVE;
    idb->lapb_exit_rnr_rej = FALSE;

	/*
     * Start timer T1 if not a rotary group.
     */
    if (!reg_invoke_dialer_huntgroup_leader(idb))
	TIMER_START(idb->lapb_t1_timer, 1);

    /*
     *  if needed, fork off a timer task
     */
    if (lapb_tf == 0)
	lapb_tf = cfork((forkproc *) lapb_timer, 0L, 0, "LAPB Timer", 0);

	/*
	 * Since the line is already up we need to tell LAPB that this has
	 * happend or its going to do nothing until the line goes up->down->up
	 */
	lapb_cstate (idb);

	return;
}

/*
 * lapb_ppp_close
 * Restore interface to standard PPP setup.
 */
void lapb_ppp_close (hwidbtype *idb)
{
    register leveltype level;

	level = raise_interrupt_level(NETS_DISABLE);
	/*
	 * This flag only indicates an ACTIVE lapb, if the line goes down we
	 * clear the flag.
	 */
	idb->ppp_lapb = FALSE;

    /*
     * remove this idb from the lapbidbq
     */
    if (idb->lapbidb) {
        unqueue(&lapbidbQ, idb->lapbidb);
        free(idb->lapbidb);
        idb->lapbidb = NULL;
    }

    /*
     * The New Protocol handling vectors
     */
	idb->ppp_new_vencap = NULL;
	idb->ppp_new_lineaction = NULL;

	/*
	 * Additional user information for "show interface" command.
	 */
	idb->ppp_new_show_support = NULL;

	/*
	 * Replace the original I/O PPP vectors.
	 * As a sanity measure check that we are not setting these vectors to
	 * NULL.
	 */

	if (idb->ppp_save_parse_packet!= NULL)
		idb->parse_packet = idb->ppp_save_parse_packet;

	/*
	 * Reset lapb and tell fast switch of changes
	 */
	lapb_reset(idb, "closedown called by PPP");

	/*
	 *  restore queue vectors, if we saved vectors to restore
	 */
	if (idb->lapb_oqueue)
	    idb->oqueue = idb->lapb_oqueue;
	if (idb->lapb_oqueue_dequeue)
	    idb->oqueue_dequeue = idb->lapb_oqueue_dequeue;

	/*
	 * Clear all storage vectors
	 */
	idb->ppp_save_parse_packet = NULL;

	idb->lapb_oqueue = NULL;
	idb->lapb_oqueue_dequeue = NULL;
	idb->lapb_state = LAPB_DISCONNECTED;
	idb->lapb_tx_state = LAPB_TX_INACTIVE;
	idb->lapb_rx_state = LAPB_RX_INACTIVE;
	idb->lapb_exit_rnr_rej = FALSE;
#ifdef NOBODY_READS_ME
	nlapb--;
#endif
	reset_interrupt_level(level);
}
      
