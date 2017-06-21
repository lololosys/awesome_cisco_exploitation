/* $Id: sdllc.c,v 3.15.8.11 1996/09/06 21:58:30 pmorton Exp $
 * $Source: /release/112/cvs/Xsys/srt/sdllc.c,v $
 *------------------------------------------------------------------
 * sdllc.c -- Media Translation (SDLLC) Support
 *
 * June 1 1992,      Percy P. Khabardar 
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sdllc.c,v $
 * Revision 3.15.8.11  1996/09/06  21:58:30  pmorton
 * CSCdi68328:  sdllc doesnt use atomic_decrement, where it should
 * Branch: California_branch
 * It does now.
 *
 * Revision 3.15.8.10  1996/08/28  13:16:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.15.8.9  1996/08/26  15:12:39  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.15.8.8  1996/08/03  23:37:56  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.15.8.7  1996/07/23  13:29:24  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.15.8.6  1996/07/20  05:29:18  schiang
 * CSCdi61499:  router crashes during unconfig sdllc traddr
 * Branch: California_branch
 *
 * Revision 3.15.8.5  1996/07/18  04:26:18  ppearce
 * CSCdi56398:  Router (sdlc-secondary) fails to respond to SNRM inputs
 * Branch: California_branch
 *   Ensure SDLLC process is re-started when SDLLC is configured
 *
 * Revision 3.15.8.4  1996/07/09  06:05:41  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.15.8.3  1996/05/17  12:13:47  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.13.2.10  1996/05/14  16:34:01  ppearce
 * CSCdi56071:  45k Router crashes on sdllc configuration
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.13.2.9  1996/05/06  00:01:28  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.13.2.8  1996/04/26  15:23:24  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.13.2.7  1996/04/26  07:58:38  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.13.2.6  1996/04/25  21:30:20  pmorton
 * CSCdi55772:  Display wrong for send TEST
 * Branch: IbuMod_Calif_branch
 * Make the display correspond to the data.
 *
 * Revision 3.13.2.5  1996/04/11  14:38:29  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.13.2.4  1996/04/03  21:59:19  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.13.2.3  1996/03/28  08:21:03  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.13.2.2  1996/03/17  18:49:56  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.13.2.1  1996/03/01  16:31:38  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.15.8.2  1996/04/27  06:28:56  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.7.8.3  1996/04/26  00:29:47  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.7.8.2  1996/04/16  23:32:50  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.7.8.1  1996/02/23  20:58:54  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.15.8.1  1996/03/18  22:10:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.5  1996/03/16  07:38:18  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.2.4  1996/03/13  02:02:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.8.2.3  1996/03/07  10:51:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.2.2  1996/02/20  18:48:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.15  1996/03/01  00:49:30  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up correctly
 * Restore CSCdi47275 w/fix to netbios_as.c
 *
 * Revision 3.14  1996/02/28  09:34:14  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.13  1996/02/21  02:43:56  hampton
 * Clean up platform specific SRB defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.12  1996/02/16  18:09:46  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Backout CSCdi47275 temporarily-- it breaks igs-c-l build
 *
 * Revision 3.11  1996/02/15  01:38:39  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Fix cleanup and change appl. cleanup from registry to vector
 *
 * Revision 3.10  1996/02/07  16:15:40  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.9  1996/02/01  06:10:26  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.8.2.1  1996/01/24  23:28:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.8  1996/01/22  07:16:35  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7  1996/01/05  10:25:09  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.6  1995/12/31  18:17:35  fbordona
 * CSCdi46267:  DLSw sdlc secondary multidrop PUs do not connect.
 *
 * Revision 3.5  1995/12/30  00:29:19  hou
 * CSCdi37413:  RTBR bridging fails for Ether->Token->Ether on vines and
 * ip.
 * - Set the default OUI for SNAP on token ring interface as 0x000000
 *   instead of 0x0000f8.
 * - Make low-end "ethernet-transit-oui" fully functional.
 *
 * Revision 3.4  1995/12/15  22:43:12  rbatz
 * CSCdi43378:  Explorers destined for SDLLC Virtual Ring forwarded to
 *              wrong ring.  Rework of vr_virtualp required.
 *
 * Revision 3.3  1995/11/17  18:55:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:42:20  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:31:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:30:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/10/19  08:10:41  bchan
 * CSCdi27730:  SNA Fragmentation break if no buffers available
 *
 * Revision 2.7  1995/10/09  23:05:17  richl
 * CSCdi39832:  SegV exception at sdllc_fix_llc
 *
 * Revision 2.6  1995/08/08  16:49:35  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.5  1995/08/07 22:21:27  motto
 * Removed an extra nuke_pak call.
 * CSCdi38330:  %SYS-2-LINKED: Bad p_enqueue of 8316A0 in queue 8B93AC
 *
 * Revision 2.4  1995/08/07  05:44:05  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *              move dlsw_work to its own file, create libibm_util.a for
 *              utility routines used by dlsw and rsrb
 *
 * Revision 2.3  1995/07/19  21:06:45  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add services to reducce dependency on idb.
 * Install 'qllc dlsw' command.
 * Build QLLC Activate/Deactivate Ring CLSI message.
 * Consolidate qllc fields from swidb to a single data structure (subblock)
 *
 * Revision 2.2  1995/07/06  01:33:52  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.1  1995/06/07 23:01:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "interface_private.h" /* -> _generic when subblock work done */
#include "../ibm/sdlc_registry.h"
#include "srt_registry.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../ibm/msg_sdlc.c"		/* Not a typo, see logger.h */
#include "../ibm/parser_defs_sdlc.h"
#include "rsrb.h"
#include "../if/network.h"
#include "srb.h"
#include "srb_sb_private.h"
#include "../llc2/llc2.h"
#include "../llc2/llc2_sb.h"
#include "../wan/dialer_registry.h"

#include "../srt/lack_externs.h"
#include "../srt/lack2fsm.h"
#include "../srt/lack2_externs.h"

#include "../ibm/sdlc.h"
#include "../ibm/sdlc_public.h"
#include "../ibm/sna_util.h"
#include "../if/rif_inline.h"
#include "../ibm/libibm_externs.h"
#include "../if/rif_util.h"
#include "../if/rif.h"
#include "../if/if_vidb.h"

/*
 * Global Storage
 */
queuetype  sdllc_linkupQ;	/* was in lack.c in 9.1(8) */

/*
 *  Forward declarations
 */
static void sdllc_config_traddr(parseinfo *csb, hwidbtype *idb);
static void sdllc_config_trlf(parseinfo *csb, hwidbtype *idb);

static dlc_rtn_t la2_sdlc_open(dlc_db_t *dlc_db, lak_t *lackie);
static dlc_rtn_t la2_sdlc_close(dlc_db_t *dlc_db, lak_t *lackie);
static dlc_rtn_t la2_sdlc_busy(dlc_db_t *dlc_db, boolean busyit);
static dlc_rtn_t la2_sdlc_abort(dlc_db_t *dlc_db);

static dlc_handle_t sdlc_dlc_handle = {
    DLC_SDLC,
    la2_sdlc_open,	la2_sdlc_close,	la2_sdlc_busy,	la2_sdlc_abort
};
static dlc_handle_t *sdlc_get_dlc_handle (void);
static boolean sdllc_active(hwidbtype *hwidb);

/*
 * Externs
 */
extern char trprint_rifstr[];
extern int lack_passthru_rings[];
extern dlc_handle_t llc2_dlc_handle;



/*
 * sdllc_start ()
 * Get sdllc process running.  This process is only used to open llc2 connections
 * with remote token ring devices.  We need to go to a process level because we
 * use llc2_open in a blocking mode.
 *
 * Note, this process is also required for SDLLC local ack.
 */
static void sdllc_start (void)
{

    if (!sdllc_pid) {
	sdllc_pid = cfork(sdllc_proc, 0L, 0, "SDLLC Process", 0 );
	if (sdllc_pid == NO_PROCESS) {
	    sdllc_pid = 0;
	    return;
	}
    }
   reg_add_media_llc_create(sdllc_fix_llc, "sdllc_fix_llc");

   reg_add_lack_get_dlc_handle(DLC_SDLC, sdlc_get_dlc_handle, 
			       "sdlc_get_dlc_handle");
    reg_add_srt_other_media(sdllc_active, "sdllc_active");

}

/*
 * sdllc_linkupBLOCK
 * Scheduler test for the sdllc linkup process.  If packet present on
 * sdllc_linkupQ, unblock sdllc linkup process else it remains blocked.
 */

static boolean sdllc_linkupBLOCK (void)
{
    hwidbtype *idb;
 
    if (sdllc_linkupQ.qhead != NULL)
	return(FALSE);
    FOR_ALL_HWIDBS(idb) {
	if (is_sdlc(idb))
	    return(TRUE);
    }
    if (sdllc_pid) {
	process_kill(sdllc_pid);
	sdllc_pid = 0;
    }
    return(TRUE);
}

                     
/*
 * sdllc_proc
 * This process is useful when sdlc is the primary side of an "sdllc session".
 * It is used to establish an llc2 session with the token ring station.  A
 * seperate process is needed since llc2_open is used in blocking mode and we
 * cannot block in interrupt mode.  This process only gets invoked when an llc2
 * session is to be established.  Once the llc2 session gets established most
 * of the work gets done at interrupt level.
 * Now, after RSDLLC support works like a background process.
 */

forktype sdllc_proc (void)
{
    paktype *pak;
    llctype *llc2 = NULL;
    sdlc_data_block *sdb;
    hwidbtype *idb;
    trrif_t *trh;
    boolean match = FALSE;
    uchar addr;

    edisms(systeminitBLOCK, 0);
    for (;;) {
        edisms((blockproc *)sdllc_linkupBLOCK, 0);
	/*
	 * CONNECTION BRING UP 
	 * It fires off SABME in local/non-lack RSDLLC conversion
         */
        while ((pak = pak_dequeue(&sdllc_linkupQ)) != NULL) {
           trh = (trrif_t *)pak->datagramstart;
	   match = FALSE;

           /* check if da matches (virtual) sdllc tr address */
           FOR_ALL_HWIDBS(idb) {
		if (idb->enctype != ET_SDLCS)
		    continue;
		if (!idb->sdllc_active)
		    continue;

		if (bcmp(trh->daddr, idb->sdlc_vr_macaddr,
		       IEEEBYTES - 1) == 0) {
                 match = TRUE;
                 break;
              }
           }
           if (!match) {
	       if (sdllc_debug)
		   buginf("\nSDLLCERR: no idb, pak dropped, da %e, sa %e, rif %s",
			  trh->daddr, trh->saddr,
			  get_rif_str((uchar *)&(trh->rc_blth), trprint_rifstr));
	       datagram_done(pak);
	       continue;
           }

           if (idb->sdlc_data[trh->daddr[ IEEEBYTES - 1] ] == NULL) {
              if (sdllc_debug)
                 buginf("\nSDLLCERR: last da byte 0, pak dropped, da %e, sa %e, rif %s",
                     trh->daddr, trh->saddr,
                     get_rif_str((uchar *)&(trh->rc_blth), trprint_rifstr));
	      datagram_done(pak);
	      continue;
           }

           addr = trh->daddr[IEEEBYTES - 1];
           sdb  = (sdlc_data_block *)idb->sdlc_data[addr];

           if ((sdb) &&
	       (sdb->sdllc_state == SDLLC_AW_NET) &&
	       (sdb->sdllc_llc)) {
	      /*
	       * This is only used for NON-LOCACK, upstream serial.
	       */
              llc2 = llc2_open2(sdb->sdllc_llc, TRUE);
              if (llc2) {
		    sdllc_net_ind_connect(idb, sdb);
		    llc2_stateswitch(llc2, SET_LOCAL_BUSY);
		    sdb->sdllc_llc = llc2;
		    llc2->sdllc_address = trh->daddr[5];
		    llc2->sdllc_active = TRUE;
                    llc2->media_llc_cleanup = sdllc_cleanup_llc;
              } else 
		  sdllc_net_ind_disconnect(idb, sdb);
           } else
	       if (sdllc_debug)
		   buginf("\nSDLLCERR: unable to open llc2 session sdb %x, llc2 %x, state %s",
			  sdb, sdb->sdllc_llc, sdllc_prstate(sdb, 0));
	   datagram_done(pak);
	   continue;
        }
    }
}


/*
 * Send an SDLC frame from LLC2, possibly breaking the frame up into lots
 * of little pieces first (fragmentation)
 */
static void sdllc_sendframe (paktype *pak,
		      uchar    addr)              /* SDLC address */
{
    lak_t *lackie;              /* Local ack handle */
    sdlc_data_block *sdb;
    hwidbtype *idb;

    idb = hwidb_or_null(pak->if_output);
    if (!idb) {
    	vr_nukepak(pak);
	return;
    }
    
    sdb = idb->sdlc_data[addr];

    /* sdlc not configured? */
    if (!sdb ||
	( (idb->enctype == ET_SDLCP) && (!IS_SDLC_CONNECT(sdb)) ) ){
       vr_nukepak(pak);
       return;
    }

    /*
     * RSDLLC keeps these packets, if any(?) until the connection comes up!
     */
    if  ( (idb->enctype == ET_SDLCS) && (sdb->sdllc_state != SDLLC_CONNECT)
	   && sdllc_debug ) {
	buginf ("\n RSDLLC : I-frames from TR side before sdlc side is up ");
    }

    pak->lnx_cb = NULL;          /* ensure lnx not set => sdllc */
    SdlcSendThruQ(sdb, pak);
	
    /*
     * Check if the high threshold value has been reached to quench ON 
     * the llc2 side.
     * Once sdlc starts draining the i-frames, it will quench OFF the
     * llc2 side when the low water mark is reached in sdlc_dooutput().
     */
    if (!sdb->sdllc_local_busy &&
	((100 * sdb->sdlc_holdq.count)/sdb->sdlc_holdq.maximum >= SDLC_TXQ_HIWATER)) {
	if (sdb->sdllc_lak_enable) {
	    lackie = sdb2lak(idb, sdb);
	    if (lackie)
		lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_txqhigh);
	} else
	    llc2_stateswitch(sdb->sdllc_llc, SET_LOCAL_BUSY);
	sdb->sdllc_local_busy = TRUE;
    }
}



/*
 * Should this frame start an SDLLC Serial Secondary Local ACK session?
 */
boolean sdllc_local_ack_this (paktype *pak, hwidbtype *idb, tr_ventrytype *vre)
{
  tr_vgrouptype *vrg;


  if (!idb)
    return(FALSE);

  vrg = vrfind_ringgroup(srb_sb_get_targetring(idb->firstsw));
  if (!vrg)
    return(FALSE);

  /* If SDLLC Local Ack is globally disabled, return FALSE */
  if (!sdllc_lak_enable)
    return(FALSE);

  return (dlc_local_ack_this(pak, idb, vre));
}


void sdllc_datagram_out (paktype *pak, uint type)
{
    unsigned acct_proto;		/* accounting protocol type */
    leveltype level;			/* lock out interrupts */
    ulong acct_bytes;
    trrif_t *trh;
    ulong *llc, riflen;
    tr_vpeertype *vrp;
    hwidbtype *idb = pak->if_output->hwptr;	/* addr of associated idb */

    SAVE_CALLER();

    if (idb->oqueue != rsrb_sdllc_oqueue)
	datagram_out(pak);

    if ((idb->state != IDBS_UP) || reg_invoke_dialer_spoofing_int(idb)) {
	datagram_done(pak);		/* it's down, flush datagram */
	return;				/* all done */
    }
    
    if (idb->status & IDB_TR) {
	trh = (trrif_t *) pak->datagramstart;
	riflen = (trh->saddr[0] & TR_RII) ? (trh->rc_blth & RC_LTHMASK) : 0;
	if (pak->datagramsize <= 14 + riflen) {
	    llc = (ulong *) ((uchar *) &(trh->rc_blth) + riflen);
	    buginf("\n%s: datagram_out, illegal output size, caller= %x, length %d", idb->hw_short_namestring,
		   caller(), pak->datagramsize);
	    tr_print_packet(idb, "XIDOUT", (tring_hdr *)trh, llc, 0, pak->datagramsize);
	}
    }

    /*
     * Decrement the input hold queue count on the inbound interface.
     * N.B.: Do not use clear_if_input() since it clears pak->if_input.
     */
    if (pak->flags & PAK_INPUTQ) {
	if (pak->if_input)
	    atomic_decrement(&pak->if_input->hwptr->input_qcount);
	pak->flags &= ~PAK_INPUTQ;
    }

    /*
     * Assume the data will be enqueued and subsequently delivered.
     */
    acct_proto = pak->acct_proto;
    idb->counters.tx_frames[acct_proto]++;
    acct_bytes = pak->datagramsize;
    idb->counters.tx_bytes[acct_proto] += acct_bytes;
    level = raise_interrupt_level(NETS_DISABLE);
    pak->if_input = idb->firstsw;
    vrp = pak->peer_ptr;
    if (!vrp || vrp->vrp_version == RSRB_LACK_BC_VERSION
	  || vrp->state != RSRB_PS_CONNECTED || (!vrp->local_ack) )
	type = RSRB_OP_SDLLC;
    pak->flags &= ~PAK_LACK_OUT;
    vrforward_pak(pak, TRUE, type, RSRB_RIFLF_LEAVEALONE);
    reset_interrupt_level(level);
}



/*
 * sdllc_opening_xchg
 *
 * To date, sdllc supports only SDLC leased lines.  There are two 
 * modes of operation; reverse SDLLC provides for a SDLC attached FEP
 * and downstream LLC PU T2 devices, and normal SDLLC which provides
 * for a T/R attached PU 4 or 5 and downstream SDLC PU T2 devices.
 * 
 * INPUT		OUTPUT SDLC-P			OUTPUT SDLC-S
 * ---------------------------------------------------------------------
 * Test(P)		Test(F) 		 	Test(F) 
 * Test(F)		XID(P) - LONG (0T2)		XID(P) - NULL
 * XID(P)-NULL-STD	XID(F)-NULL-STD			XID(F)-NULL-STD
 * XID-NULL-SNA	        XID- LONG (0T2)		        XID(NULL)
 * XID(P/F)-LONG	nuke it.			SABME/CR
 */

static void sdllc_opening_xchg (paktype *pak, hwidbtype *inidb,
			    hwidbtype *sdlc_idb, sdlc_data_block *sdb, tr_ventrytype *vre)
{
   uchar            conch;
   boolean	    pfbit;
   trrif_t         *trh;
   int              riflen, llclen, xid_format;
   uchar           *frameptr;
   sap_hdr         *llc1;
   paktype         *newpak;
   paktype         *xid_pak;
   llctype         *llcp;
   uchar            addr[14];
   xid3_t          *xid_data;
   lak_t	   *lackie;
   paktype         *test_pak;
   tring_hdr       *trframe;
   hwaddrtype      srcMac, destMac;

   trh                 = (trrif_t *)pak->datagramstart;
   riflen	       = (trh->saddr[0] & TR_RII) ? trh->rc_blth & RC_LTHMASK : 0;

   if (!riflen) {
       if (lack_debug_err || sdllc_debug)
	   buginf("\nSDLLC: internal error, no rif on opening exchange");
       return;
   }

   pak->llc2_sapoffset = TRING_ENCAPBYTES + riflen;
   frameptr            = pak->datagramstart + pak->llc2_sapoffset;
   llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + riflen);
   llclen = pak->datagramsize - TRING_ENCAPBYTES - riflen;

   pfbit = (*(frameptr + 2) & P_BIT) != 0;
   conch =  *(frameptr + 2) & ~P_BIT;	/* snuff out p/f bit */

   trh->saddr[0] &= (~TR_RII);   /* turn off routing information-included-bit */
   trh->rc_dlf ^= RC_D;          /* toggle direction bit */

   if (sdlc_idb->state != IDBS_UP) 
        return;
    
   if (!sdb->sdlc_have_prtnr) {
       if (sdllc_debug)
	   buginf("\nSDLLC: %s, %02x, no sdllc partner address configured",
		  sdlc_idb->hw_namestring, sdb->sdlc_address);
       return;
   }

   /*
    * SDLLC TEST Response Processing:
    * If the RIF exists, then we have already received a TEST frame rsp. SNA 
    * uses the fastest route available to establish the session.  Turn off the 
    * RII bit in the SA since we don't save that in rif_update.  There will 
    * always be a RII since we are on a virtual ring. Duplicate responses due 
    * to multiple paths are dumped.
    */
   switch(conch) {
     case LLC_TEST:
       if (llc1->ssap & SAP_RESPONSE) {

	   if (rif_lookup(trh->saddr, NULL, trh->daddr, 0) != NULL ||
	       rif_lookup(trh->daddr, NULL, trh->saddr, 0) != NULL ) {
	       if (sdllc_debug)
		   buginf("\n\t dumping alternate route test response");
	       break;
	   }

	   if (sdllc_debug)
	       buginf("\n\tcaching rif");

           trh->rc_dlf ^= RC_D;    /* toggle direction bit for rif_update */
	   rif_update(NULL, 0, trh->saddr, trh->daddr,(srbroute_t *) &trh->rc_blth, RIF_AGED);
           trh->rc_dlf ^= RC_D;    /* toggle it back to send out frame */

	   lackie = trmac2lak(pak, addr, TRMAC2LAK_FROM_NET);
	   if (lackie) {
	       if (lack_debug_err || sdllc_debug)
		   buginf("\nSDLLC: internal control block already exists for this test response pak");
	       break;
	   }

	   xid_pak = tr_setup_xid(trh->saddr, trh->daddr, inidb,
				  (uchar *) &trh->rc_blth, TRUE, sdb->sdlc_address, sdb->sdlc_dsap,
				  sdb->sdlc_ssap, FALSE);
	   
	   TIMER_START(sdb->sdlc_initiation_throttle_time,
		       SDLC_THROTTLE_INTERVAL);
	   
	   if (xid_pak) {
	       /* Print out destination, then source address
		* looks reversed because we are echo'ing xid back to trh->saddr
		* so our XID is pointed to da == trhf->saddr...
		* (in else statement too...)
		*/
	       if (sdllc_debug)
		   buginf("\nSDLLC: O xid(null), %e %e %x %x [%s]", 
			  trh->saddr, trh->daddr, sdb->sdlc_dsap, sdb->sdlc_ssap,
			  get_rif_str((uchar *) &trh->rc_blth, trprint_rifstr));
	       xid_pak->peer_ptr = pak->peer_ptr;
	       sdllc_datagram_out((paktype *) xid_pak, RSRB_OP_SEND_XID);
	   } else if (sdllc_debug)
	       buginf("\nSDLLC: no buf, xid(null), %e %e %x %x [%s]",
		      trh->saddr, trh->daddr, sdb->sdlc_dsap, sdb->sdlc_ssap,
		      get_rif_str((uchar *) &trh->rc_blth, trprint_rifstr));
       }
       else {
	   /*
	    * Send out a TEST response.  The direction bit was already flipped.
	    */
	   if (trh->daddr[0] & TR_GROUP)
	       break;
	   
	   test_pak = getbuffer(llclen);

	   if (!test_pak)
	       return;

	   trframe = (tring_hdr *)
	       (test_pak->data_area + ENCAPBYTES - TRING_ENCAPBYTES - riflen);
	   trframe->ac       = AC_PRI0;
	   trframe->fc       = FC_TYPE_LLC;
	   ieee_copy(trh->saddr, trframe->daddr);
	   ieee_copy(trh->daddr, trframe->saddr);

	   bcopy((uchar *)&trh->rc_blth, trframe->data, riflen);
	   trframe->saddr[0] |= TR_RII;
	   trframe->data[0] &= ~RC_RTMASK;

	   /*
	    * limit max frame size to the value given for the interface
	    */
	   trframe->data[1] = (trframe->data[1] & ~(RC_LFMASK)) |
	       inidb->sdllc_largest_token_frame;

	   if (sdllc_debug) {
	       buginf("\nSDLLC: O TEST Response dst %e src %e dsap %02x ssap %02x",
		      trh->saddr, trh->daddr, llc1->ssap, llc1->dsap);
	       buginf("\n\trif[%s]", get_rif_str((uchar *)&trframe->data[0], trprint_rifstr));
	   }

	   /* 
	    * Exchange saps and turn on response bit.
	    */
	   llc1->dsap ^= llc1->ssap;
	   llc1->ssap ^= llc1->dsap;
	   llc1->dsap ^= llc1->ssap;
	   llc1->ssap |= SAP_RESPONSE;
	   bcopy((uchar *)llc1, test_pak->data_area + ENCAPBYTES, llclen);

	   test_pak->if_output     = inidb->firstsw;
	   test_pak->enctype       = inidb->enctype;
	   test_pak->datagramstart = (uchar *) trframe;
	   test_pak->datagramsize  = llclen + TRING_ENCAPBYTES + riflen;

	   test_pak->peer_ptr = pak->peer_ptr;
	   sdllc_datagram_out((paktype *)test_pak, RSRB_OP_SDLLC);
       }
       break;

     case XID:
       if (sdb->sdlc_dsap != (llc1->ssap & ~SAP_RESPONSE)) {
	   if ( sdllc_debug ) {
	       buginf("\nSDLLC: %s SDLC address = %02x; configured dsap = %02x ",
		      sdlc_idb->hw_namestring, sdb->sdlc_address, sdb->sdlc_dsap);
	       buginf("\n\t XID packet received with ssap = %02x", llc1->ssap);
	   }
	   break;
       }

       if (sdb->sdlc_ssap != llc1->dsap) {
	   if (sdllc_debug) {
	       buginf("\nSDLLC: %s SDLC address = %02x; configured ssap = %02x ",
		      sdlc_idb->hw_namestring, sdb->sdlc_address, sdb->sdlc_ssap);
	       buginf("\n\t XID packet received with dsap = %02x", llc1->dsap);
	   }
           break;
       }

       if (sdb->sdllc_state != SDLLC_AW_NET) {
	   if (sdllc_debug) {
	       buginf("\nSDLLC: I XID ignored, sdllc state %s, %e %e %x %x -> %s %02x",  
		      sdllc_prstate(sdb, 0), trh->saddr, trh->daddr, 
		      sdb->sdlc_dsap, sdb->sdlc_ssap, sdlc_idb->hw_namestring, 
		      sdb->sdlc_address);
	   }
	   break;
       }

       switch (sdlc_idb->enctype) {
	 case ET_SDLCP:
	   /*
	    * Only respond to XID(NULL).  
	    */
	   if (llclen <= SAP_HDRBYTES) {
	       
	       xid_pak = tr_setup_xid(trh->saddr, trh->daddr, inidb,
				      (uchar *) &trh->rc_blth, TRUE, sdb->sdlc_address, sdb->sdlc_dsap,
				      sdb->sdlc_ssap, TRUE);
	       if (xid_pak) {
		   if (sdllc_debug)
		       /* Print out destination, then source address
			* looks reversed because we are echo'ing xid back to trh->saddr
			* so our XID is pointed to da == trhf->saddr...
			* (in else statement too...)
			*/
		       buginf("\nSDLLC: O xid(0T2), %e %e %x %x [%s]",
			      trh->saddr, trh->daddr, sdb->sdlc_dsap, sdb->sdlc_ssap,
			      get_rif_str((uchar *) &trh->rc_blth, trprint_rifstr));
		   xid_pak->peer_ptr = pak->peer_ptr;
		   if ((llc1->ssap & SAP_RESPONSE) != SAP_RESPONSE)
		       ((llc1_xid_t *)(xid_pak->data_area + ENCAPBYTES))->ssap |= SAP_RESPONSE;
		   sdllc_datagram_out((paktype *) xid_pak, RSRB_OP_SEND_XID);
	       } else if (sdllc_debug)
		   buginf("\nSDLLC: no buf, xid(0T2), %e %e %x %x [%s]",
			  trh->saddr, trh->daddr, sdb->sdlc_dsap, sdb->sdlc_ssap,
			  get_rif_str((uchar *) &trh->rc_blth, trprint_rifstr));
	       break;
	   }
	   break;

	 case ET_SDLCS:
	   /*
	    * XID(NULL)
	    */
	   if (llclen <= SAP_HDRBYTES) {
	       
	       xid_pak = tr_setup_xid(trh->saddr, trh->daddr, inidb,
				      (uchar *) &trh->rc_blth, TRUE, sdb->sdlc_address, 
				      sdb->sdlc_dsap, sdb->sdlc_ssap, FALSE);
	       if (xid_pak) {
		   if (sdllc_debug)
		       /* Print out destination, then source address
			* looks reversed because we are echo'ing xid back to trh->saddr
			* so our XID is pointed to da == trhf->saddr...
			* (in else statement too...)
			*/
		       buginf("\nSDLLC: O xid(NULL), %e %e %x %x [%s]",
			      trh->saddr, trh->daddr, sdb->sdlc_dsap, sdb->sdlc_ssap,
			      get_rif_str((uchar *) &trh->rc_blth, trprint_rifstr));
		   xid_pak->peer_ptr = pak->peer_ptr;
		   if ((llc1->ssap & SAP_RESPONSE) != SAP_RESPONSE)
		       ((llc1_xid_t *)(xid_pak->data_area + ENCAPBYTES))->ssap |= SAP_RESPONSE;
		  sdllc_datagram_out((paktype *) xid_pak, RSRB_OP_SEND_XID);
	       } else if (sdllc_debug)
		   buginf("\nSDLLC: no buf, xid(NULL), %e %e %x %x [%s]",
			  trh->saddr, trh->daddr, sdb->sdlc_dsap, sdb->sdlc_ssap,
			  get_rif_str((uchar *) &trh->rc_blth, trprint_rifstr));
	       break;
	   }
	   
	   /*
	    * Formatted XID, validate, then bring up connection
	    */
	   if (llclen > SAP_HDRBYTES) {
	       if (sdb->sdlc_xid_configured) {
		   xid_data = (xid3_t *)(pak->datagramstart + 
				  TRING_ENCAPBYTES + riflen + SAP_HDRBYTES);
		   xid_format = xid_data->xid3_fmt & XID_FMT_MASK;
		   if (xid_format != XID_FMT_0 && xid_format != XID_FMT_3) {
		       errmsg(&msgsym(XID_NOMATCH, SDLC), sdlc_idb->hw_namestring, sdb->sdlc_address);
		       errmsg(&msgsym(XID_FORMAT, SDLC), xid_format >> 4);
		       break;
		   }
		   if (bcmp(&(sdb->sdlc_xid), &(xid_data->xid3_reserved[0]), SDLC_XID_SIZE)!=0){
		       errmsg(&msgsym(XID_NOMATCH, SDLC), sdlc_idb->hw_namestring, sdb->sdlc_address);
		       errmsg(&msgsym(XID_DISPLAY, SDLC), 
			      trh->saddr, trh->daddr, sdb->sdlc_ssap, sdb->sdlc_dsap, 
			      xid_data->xid3_reserved[0], xid_data->xid3_reserved[1],
			      xid_data->xid3_reserved[2], xid_data->xid3_reserved[3]);
		       break;
		   }
	       }

	       /*
		* This is the route chosen by the end node. 
		* Lock the route into the rif table by using RIF_NOAGE.
		*/
               trh->rc_dlf ^= RC_D;          /* toggle direction bit */
	       rif_update(NULL, 0, trh->saddr, trh->daddr,
			  (srbroute_t *) (riflen ? &trh->rc_blth : NULL), RIF_NOAGE);
	       trh->rc_dlf ^= RC_D;    

	       /* 
		* check if this session is to be sdllc locally acked 
		*/
	       sdb->sdllc_lak_enable = sdllc_local_ack_this(pak, sdlc_idb, vre);
	       if (sdb->sdllc_lak_enable) {
		   trh->saddr[0] |= TR_RII;   /* turn back on TR_RII for local-ack routines */
		   lack2_rsdllc_initiate_rem_conn(pak, sdb, vre);
		   sdllc_setstate(sdlc_idb, sdb, SDLLC_AW_NET, "SDLLC opening (LUR net wait)");
		   break;
	       }

	       /*
		* We're going to send a SABME.  
	        * ieee_copy(trh->saddr, &dmac[0]);
	        * ieee_copy(trh->daddr, &dmac[6]);
		*/

	       snpa_init(&destMac, STATION_IEEE48, TARGET_UNICAST, trh->saddr);
	       snpa_init(&srcMac, STATION_IEEE48, TARGET_UNICAST, trh->daddr);
    
	       pak->if_output = sdlc_idb->vidb_link->firstsw;
	       sdb->sdllc_llc = llc2_open1(sdb->sdlc_dsap, 
					   sdb->sdlc_ssap,
					   &destMac, &srcMac, pak->if_output,
					   LINK_LLC2, 
                                           llc_sb_get_wind(pak->if_output),
                                           NULL, TRUE,
					   TRUE, FALSE, NULL_L3_PID, NULL_DLCI);
	       if (sdb->sdllc_llc) {
		   sdllc_setstate(sdlc_idb, sdb, SDLLC_AW_NET, "SDLLC opening (llc net wait)");
		   llcp = sdb->sdllc_llc;
		   llcp->sdllc_active = TRUE;
                   llcp->media_llc_cleanup = sdllc_cleanup_llc;
		   llcp->sdllc_idb = sdlc_idb;
		   newpak = pak_duplicate(pak);
		   if (newpak)
		       pak_enqueue(&sdllc_linkupQ, newpak);
	       } else if (sdllc_debug)
		   buginf("\nSDLLC: llc2_open1 failed...");
	       break;

	       /* long XID form and it is a control vector to indicate the error */
	       if (sdb->sdlc_have_prtnr && llclen > SAP_HDRBYTES && (llc1->data[0] == XID_ERROR_CV ) ) {
		   if (sdllc_debug)
		       buginf("\nSDLLC: I xid negotiation error %02x %e %e %x %x [%s]",  
			      llc1->data[0], trh->saddr, trh->daddr, sdb->sdlc_dsap, sdb->sdlc_ssap,
			      get_rif_str((uchar *) &trh->rc_blth, trprint_rifstr));
	       }
	   }

	   break;
	 default:
	   break;
       }
     default:
       break;
   }
}


/*
 * s d b 2 l a k
 *
 * Given an sdb structure and an idb pntr go find the
 * corresponding lack entity.
 */

lak_t *sdb2lak (hwidbtype *idb, sdlc_data_block *sdb)
{
    uchar entity[14];

    if (!idb || !sdb)
	return(NULL);
    ieee_copy(idb->sdlc_vr_macaddr, entity);
    entity[5] = sdb->sdlc_address;
    ieee_copy(sdb->sdlc_prtnr, &entity[6]);
    entity[12] = sdb->sdlc_ssap;
    entity[13] = sdb->sdlc_dsap;
    return(lack_by_machdr(entity));
}

/* sdlc_get_dlc_handle
 *
 * provide the sdlc handle for lack_get_dlc_handle
 */
static dlc_handle_t *sdlc_get_dlc_handle (void)
{
    return(&sdlc_dlc_handle);
}

/*
 * l a 2 _ s d l c _ o p e n
 *
 * Cause SDLC to do an open
 */

static dlc_rtn_t la2_sdlc_open (dlc_db_t *dlc_db, lak_t *lackie)
{
    sdlc_data_block *sdb;
    hwidbtype *sdlc_idb;
    paktype *pak;
    
    if (!dlc_db || !lackie) {
	buginf("\nLOCACK: la2_sdlc_open, null dlc_db or null lackie");
	return(DLC_RS_FAILED);
    }
    if (dlc_db->type != DLC_SDLC) {
	buginf("\nLOCACK: la2_sdlc_open called with wrong type of dlc_db");
	return(DLC_RS_FAILED);
    }
    if (!sdllc_lak_enable)
	return(DLC_RS_FAILED);
    if (!lackie->piggie_pak) {
	buginf("\nLOCACK: la2_sdlc_open with no piggie pak");
	return(DLC_RS_FAILED);
    }
    pak = lackie->piggie_pak;
    if (pak && !sdllc_get_sdb(pak, &sdlc_idb, &sdb))
	return(DLC_RS_FAILED);
    if (!sdb || !sdlc_idb)
	return(DLC_RS_FAILED);
    dlc_db->ptr.sdb = sdb;
    if (sdb->sdllc_state >= SDLLC_AW_NET) {
	sdb->sdllc_lak_enable = TRUE;
	sdllc_net_ind_connect(sdb->sdlc_idb, sdb);
	return(DLC_RS_OK);
    }
    return(DLC_RS_FAILED);
}


/*
 * l a 2 _ s d l c _ c l o s e
 *
 * close down an sdlc connection
 */

static dlc_rtn_t la2_sdlc_close (dlc_db_t *dlc_db, lak_t *lackie)
{
    sdlc_data_block *sdb;

    if (!dlc_db || !lackie) {
	buginf("\nLOCACK: la2_sdlc_close, null dlc_db or null lackie");
	return(DLC_RS_FAILED);
    }
    if (dlc_db->type != DLC_SDLC) {
	buginf("\nLOCACK: la2_sdlc_close called with wrong dlc_db type.");
	return(DLC_RS_FAILED);
    }
    sdb = dlc_db->ptr.sdb;
    if (!sdb)
	return(DLC_RS_FAILED);
    if (sdb->sdllc_state > SDLLC_AW_NET)
	sdllc_net_ind_disconnect(sdb->sdlc_idb, sdb);
    return(DLC_RS_OK);
}


/*
 * l a 2 _ s d l c _ b u s y
 *
 * busy or unbusy the sdlc connection
 */

static dlc_rtn_t la2_sdlc_busy (dlc_db_t *dlc_db, boolean busyit)
{
    sdlc_data_block *sdb;

    if (!dlc_db) {
	buginf("\nLOCACK: la2_sdlc_busy, null dlc_db");
	return(DLC_RS_FAILED);
    }
    if (dlc_db->type != DLC_SDLC) {
	buginf("\nLOCACK: la2_sdlc_unbusy called with wrong dlc_db type.");
	return(DLC_RS_FAILED);
    }
    sdb = dlc_db->ptr.sdb;
    if (!sdb)
	return(DLC_RS_FAILED);
    if ( (sdb->sdlc_idb->enctype == ET_SDLCS ) && (!busyit) &&
	 (sdb->sdllc_state != SDLLC_CONNECT ) )
	sdllc_net_ind_connect(sdb->sdlc_idb, sdb);
    else
	sdlc_set_clear_busy(sdb->sdlc_idb, sdb->sdlc_address, busyit);
    return(DLC_RS_OK);
}


/*
 * l a 2 _ s d l c _ a b o r t
 *
 * abort the sdlc connection
 */

static dlc_rtn_t la2_sdlc_abort (dlc_db_t *dlc_db)
{
    sdlc_data_block *sdb;

    if (!dlc_db) {
	buginf("\nLOCACK: la2_sdlc_abort, null dlc_db");
	return(DLC_RS_OK);
    }
    if (dlc_db->type != DLC_SDLC) {
	buginf("\nLOCACK: la2_sdlc_abort called with wrong dlc_db type.");
	return(DLC_RS_FAILED);
    }
    sdb = dlc_db->ptr.sdb;
    if (!sdb)
	return(DLC_RS_FAILED);
    sdllc_net_ind_disconnect(sdb->sdlc_idb, sdb);
    return(DLC_RS_OK);
}


/*
 * s d l l c _ n e t _ i n d _ d i s c o n n e c t
 *
 * a disconnection indication from the sdllc network side is being
 * signaled to sdllc.
 *
 * This forces a disconnection from anystate.  SDLLC goes to disconnect
 * and we force SDLC into DISCSENT if it was not disconnected or DISCSENT.
 */

void sdllc_net_ind_disconnect (hwidbtype *idb, sdlc_data_block *sdb)
{
    /*
     * The sdllc_setstate MUST happen before sdlc_setstate.  sdlc_setstate
     * depending on what state it is in may cause the sdllc_state to go
     * to DW_NET depending on what kind of corner case is occuring.
     */
    TIMER_STOP(sdb->time_linkup_sent);
    if (sdb->sdllc_state >= SDLLC_AW_NET)
	sdllc_setstate(idb, sdb, SDLLC_DW_SDLC, "Net disconnect");
    if (sdb->sdlc_state > SDLC_DISCSENT) {
	sdlc_setstate(idb, SDLC_DISCSENT, sdb);
	sdlc_set_pause_timer(idb, sdb);
    } else if (sdb->sdlc_state != SDLC_DISCSENT)
	sdlc_setstate(idb, SDLC_DISCONNECT, sdb);
}


/*
 * s d l l c _ n e t _ i n d _ c o n n e c t
 *
 * a connection indication from the sdllc network side is being
 * signalled to sdllc.
 */

void sdllc_net_ind_connect (hwidbtype *idb, sdlc_data_block *sdb)
{
    if (sdb->sdllc_state != SDLLC_AW_NET && sdllc_debug)
	buginf("\nSDLLC_ERR: net_ind_connect wrong state: %s for %s %02x",
	       sdllc_prstate(sdb, 0), idb->hw_namestring, sdb->sdlc_address);
    if (idb->enctype == ET_SDLCP) {
	/*
	 * downstream serial (primary sdlc).
	 */
	sdllc_setstate(idb, sdb, SDLLC_CONNECT, "Net connect");
	if (!sdb->sdllc_lak_enable)
	    sdlc_set_clear_busy(idb, sdb->sdlc_address, FALSE);
    } else if (idb->enctype == ET_SDLCS) {
	/*
	 * upstream serial (secondary sdlc).  Only do it if we are
	 * in the right state to transition.
	 */
	sdllc_setstate(idb, sdb, SDLLC_AW_SDLC_SEC, "Net connect (secondary)");
    }
}


   

/*
 *
 * lak_forwardpak
 * This function generates and forwards packet which helps in change of
 * local ack states.
 */

void lak_forwardpak (uchar *da, uchar *sa, int dsap, int ssap,
		     hwidbtype *idb, int type, boolean toggle_dbit)
{
   riftype   *rifptr;
   int        riflen;
   paktype   *pak;
   uchar     *frameptr;
   tring_hdr *trframe;
   ushort     bufsize;
   trrif_t   *trh;

   /* 
    * This tells us which direction to examine... 
    */
   if (toggle_dbit) 
      rifptr = rif_lookup(sa, NULL, da, 0);
   else 
      rifptr = rif_lookup(da, NULL, sa, 0);
   if (rifptr)
      riflen = rifptr->length;
   else {
      riflen = 0;
      if (sdllc_debug)
         buginf("\nSDLLC: lak_forwardpak - riflen is 0.");
   }
   if (lack_debug_state)
	buginf("\nLOCACK: sent %s -> %e %e %02x %02x",
	       vr_op2str(type), da, sa, dsap, ssap);

   bufsize = TRING_ENCAPBYTES + riflen + SAP_HDRBYTES + TR_FCS_SIZE;
   pak     = getbuffer(bufsize);
   if (!pak) {
      return;
   }

   pak->enctype        = ET_SNAP;
   pak->if_input       = idb->firstsw;
   pak->flags         |= PAK_LACK_OUT;
   pak->datagramsize   = bufsize;
   pak->datagramstart  = pak->network_start;
   pak->llc2_sapoffset = TRING_ENCAPBYTES + riflen;

   trframe = (tring_hdr *) pak->datagramstart;

   trframe->ac = AC_PRI0;
   trframe->fc = FC_TYPE_LLC;

   trframe->daddr[0] = da[0] & (~TR_RII);
   trframe->daddr[1] = da[1];
   PUTSHORT(&trframe->daddr[2], GETSHORT(&da[2]));
   PUTSHORT(&trframe->daddr[4], GETSHORT(&da[4]));
   PUTSHORT(&trframe->saddr[0], GETSHORT(&sa[0]));
   PUTSHORT(&trframe->saddr[2], GETSHORT(&sa[2]));
   PUTSHORT(&trframe->saddr[4], GETSHORT(&sa[4]));
   if (riflen) {
      bcopy(rifptr->rif, trframe->data, riflen);
      trframe->saddr[0] |= TR_RII;
      if (toggle_dbit) {
        trh = (trrif_t *) pak->datagramstart;
        trh->rc_dlf ^= RC_D;
      }
   }
   trframe->data[1] = (trframe->data[1] & ~(RC_LFMASK)) |
     idb->sdllc_largest_token_frame;
   frameptr        = pak->datagramstart + pak->llc2_sapoffset;
  *frameptr       = dsap;
  *(frameptr + 1) = ssap;
  *(frameptr + 2) = LLC1_UI;
  vrforward_pak(pak, idb->sdllc_active, type, RSRB_RIFLF_LEAVEALONE);
}

/*
 * sdllc_input - input to sdlc from llc2.  This routine processes all llc2
 * frames. LLC2 frames which change link states (SABME, DISC/DM, FRMR) have
 * their corresponding SDLC counterparts sent to the SDLC stations.  All S/U
 * frames are processed and freed by llc2.  I-frames are forwarded by the SDLC
 * logic and freed when they receive a +ve response from the remote SDLC
 * station.
 */

static void sdllc_input (hwidbtype *inidb, paktype *pak, int opcode, hwidbtype *sdlc_idb,
		  sdlc_data_block *sdb, tr_ventrytype *vre)
{
    uchar            conch;
    trrif_t         *trh;
    int              riflen;
    uchar           *frameptr;
    leveltype        level;
    sap_hdr        *llc1;
    uchar            sa[IEEEBYTES];
    int		     bail_op;
    boolean	     lack_op = FALSE;
    char	     peerid[MAX_PEER_NAME];
    lak_t	    *lackie;
    
    trh                 = (trrif_t *)pak->datagramstart;
    riflen	       = (trh->saddr[0] & TR_RII) ? trh->rc_blth & RC_LTHMASK : 0;
    pak->llc2_sapoffset = TRING_ENCAPBYTES + riflen;
    frameptr            = pak->datagramstart + pak->llc2_sapoffset;
    llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + riflen);

    conch = llc1->control & ~P_BIT;	/* snuff out p/f bit */

    switch(opcode) {
	/*
	 * SEND_XID frames contain an XID frame of somekind that
	 * are processed in sdllc_opening_xchg()
	 */
      case RSRB_OP_SEND_XID:
	break;
      case RSRB_OP_LINKUP_REQ:
      case RSRB_OP_LINKDOWN_REQ:
      case RSRB_OP_LINKUP_PRSP:
      case RSRB_OP_LINKDOWN_PRSP:
      case RSRB_OP_QUENCH_ON:
      case RSRB_OP_QUENCH_OFF:
      case RSRB_OP_ConnectRequest:
      case RSRB_OP_ConnectPollReq:
      case RSRB_OP_ConnectPollAck:
      case RSRB_OP_ConnectPollAckAndReq:
      case RSRB_OP_ConnectWaitAck:
      case RSRB_OP_DisconnectRequest:
      case RSRB_OP_DisconnectAck:
      case RSRB_OP_LA2_QON:
      case RSRB_OP_LA2_QON_ACK:
      case RSRB_OP_LA2_QOFF:
      case RSRB_OP_LA2_QOFF_ACK:
	lack_op = TRUE;

	/* fall through */

      case RSRB_OP_NONE:
      case RSRB_OP_FORWARD:
      case RSRB_OP_FORWARDCRC:
      case RSRB_OP_SDLLC:
      case RSRB_OP_LLC2DATA:
      case RSRB_OP_FORWARDFST:
      case RSRB_OP_FORWARDFSTCRC:
	/*
	 * display all other control messages.  Don't display
	 * IFRAMES or SUPER frames (RR, RNR, REJ).
	 */
	if ((conch & LLC_SMASK) != LLC_UNNUMBERED)
	    break;
	
	/* fall through */
	
      default:
	if (sdllc_debug)
	    buginf("\nSDLLC: %s recv %s %s(%02x) %e %e %02x %02x -> %s %02x",
		   sdllc_prstate(sdb, 0), vr_op2str(opcode),
		   lack_prconch(llc1->control), llc1->control, trh->daddr, trh->saddr,
		   llc1->dsap, llc1->ssap, sdlc_idb->hw_namestring, sdb->sdlc_address);
	break;
    }

    if (lack_op) {
	lak2_process_netmsg(pak, opcode, vre, DLC_SDLC);
	return;
    }

    switch (sdb->sdllc_state) {
      case SDLLC_DISCONNECT:
      case SDLLC_DW_NET:
      case SDLLC_DW_SDLC:
      case SDLLC_AW_SDLC_PRI:
bail_on_net_msg:
	bail_op = 0;
	switch (opcode) {
	  case RSRB_OP_LINKUP_REQ:
	  case RSRB_OP_LINKUP_PRSP:
	    if (!bail_op) bail_op = RSRB_OP_LINKDOWN_REQ;

	  case RSRB_OP_LINKDOWN_REQ:
	    /*
	     * They are requesting that we go down.  Are we sleezy or just fun loving?
	     * Respond with positive response.
	     */
	    if (!bail_op) bail_op = RSRB_OP_LINKDOWN_PRSP;

	    /*
	     * Note that due to the shitty design (if one can go that far and
	     * call it a design) of this shit pot steaming pile it is possible
	     * to get lack messages when we are disconnected when it looks
	     * like we shouldn't.  Just say so and move on.  This will give
	     * our poor CE/SEs in the field half a chance.
	     */
	    if (sdllc_debug)
		buginf("\nSDLLC: %s (%d) not allowed from %e %e %02x %02x for %s %02x",
		       vr_op2str(opcode), opcode, trh->daddr, trh->saddr,
		       llc1->dsap, llc1->ssap, sdlc_idb->hw_namestring, sdb->sdlc_address);
	    /*
	     * send an appropriate response back if reasonable.
	     */
	    rif_update(NULL, 0, trh->saddr, trh->daddr,
		       (srbroute_t *) (riflen ? &trh->rc_blth : NULL), RIF_AGED);
	    bcopy(&sdlc_idb->sdlc_vr_macaddr[0], &sa[0], IEEEBYTES - 1);
	    sa[5]  = sdb->sdlc_address;
	    lak_forwardpak(&sdb->sdlc_prtnr[0], &sa[0], sdb->sdlc_dsap, 
			   sdb->sdlc_ssap, sdlc_idb->vidb_link,
			   bail_op, FALSE);
	    goto bail_and_discard;
	    
	    /*
	     * could have fallen through.
	     */
	    
	  case RSRB_OP_SDLLC:
	  case RSRB_OP_LLC2DATA:
	  case RSRB_OP_QUENCH_ON:
	  case RSRB_OP_QUENCH_OFF:
	  case RSRB_OP_LINKDOWN_PRSP:
	    /*
	     * LDPR is allowed anytime.  It says the other side is down.  Say
	     * what we did.  It's harmless.  DATA is harmless and used to get
	     * nuked anyway.  Same for Quench.
	     */
bail_and_discard:
	    if (sdllc_debug)
		buginf("\nSDLLC: %s discard %s %s(%02x) %e %e %02x %02x -> %s %02x",
		       sdllc_prstate(sdb, 0), vr_op2str(opcode),
		       lack_prconch(llc1->control), llc1->control, trh->daddr, trh->saddr,
		       llc1->dsap, llc1->ssap, sdlc_idb->hw_namestring, sdb->sdlc_address);
	    vr_nukepak(pak);
	    return;
	    
	  default:
	    break;
	}

	if ((conch & SDLC_SFRAME) == SDLC_IFRAME) {
	    vr_nukepak(pak);
	    return;
	}

	if (conch == LLC1_XID || conch == LLC_TEST) {
	    sdllc_opening_xchg(pak, inidb, sdlc_idb, sdb, vre);
	    vr_nukepak(pak);
	    return;
        }
	
	if (sdb->sdllc_llc) {
	    /*
	     * if we have one to begin with, process the incoming.  It will
	     * close and clean up under LLC control.  Nothing else to do.
	     */
	    if (sdllc_debug)
		buginf("\nSDLLC: netmsg %s(%02x) %s(%02x) in %s for %s, %02x",
		       lack_prconch(conch), conch, vr_op2str(opcode), opcode,
		       sdllc_prstate(sdb, 0), sdlc_idb->hw_namestring, sdb->sdlc_address);
	    level = raise_interrupt_level(NETS_DISABLE);
	    pak->flags |= PAK_LACK_IN;
	    reg_invoke_llc2_input(pak);
	    reset_interrupt_level(level);
	    return;
	}
	/*
	 * Can't be a LACK opcode and be in here.  So must be destined for LLC2
	 * Give it a chance to respond.  Note if we had one to begin with then
	 * it is handled upstairs.  If we end up with one below then it must have
	 * been a new one.  Tear it down immediately.  This then will close
	 * naturally.
	 */
	sdb->sdllc_lak_enable = FALSE;
	level = raise_interrupt_level(NETS_DISABLE);
	rif_update(NULL, 0, trh->saddr, trh->daddr,
		   (srbroute_t *) (riflen ? &trh->rc_blth : NULL), RIF_AGED);
	pak->flags |= PAK_LACK_IN;
	reg_invoke_llc2_input(pak);
	/*
	 * check for new llc existing and tear it down.  Only AW_NET is allowed
	 * to go forward with the network connection.
	 */
	if (sdb->sdllc_llc)
	    llc2_close(sdb->sdllc_llc);
	reset_interrupt_level(level);
	return;

      case SDLLC_AW_NET:
	switch(opcode) {
	  case RSRB_OP_LINKDOWN_REQ:
	  case RSRB_OP_LINKDOWN_PRSP:	/* something weird, bring 'er down */
	  case RSRB_OP_LINKUP_REQ:
	  case RSRB_OP_LINKUP_PRSP:
	    sdllc_net_ind_disconnect(sdlc_idb, sdb);
	    goto bail_on_net_msg;

	  case RSRB_OP_SDLLC:
	  case RSRB_OP_LLC2DATA:
	  case RSRB_OP_QUENCH_ON:
	  case RSRB_OP_QUENCH_OFF:	/* ignore */
	    goto bail_and_discard;

	  default:
	    break;
	}

	/* In the SDLLC_AW_NET state drop all I frames */

	if ((conch & SDLC_SFRAME) == SDLC_IFRAME) {
	    vr_nukepak(pak);
	    return;
	}

	if (conch == LLC1_XID || conch == LLC_TEST) {
	    sdllc_opening_xchg(pak, inidb, sdlc_idb, sdb, vre);
	    vr_nukepak(pak);
	    return;
        }
	if (conch == SABME) {
	    if (sdllc_debug)
		buginf("\nSDLLC: SABME for %s %02x in %s",
		       sdlc_idb->hw_namestring, sdb->sdlc_address,
		       sdllc_prstate(sdb, 0));
	    sdb->sdllc_lak_enable = FALSE;
	    level = raise_interrupt_level(NETS_DISABLE);
	    pak->flags |= PAK_LACK_IN;
	    rif_update(NULL, 0, trh->saddr, trh->daddr,
		       (srbroute_t *) (riflen ? &trh->rc_blth : NULL), RIF_NOAGE);
	    reg_invoke_llc2_input(pak);
	    reset_interrupt_level(level);
	    return;
	}
	sdb->sdllc_lak_enable = FALSE;
	level = raise_interrupt_level(NETS_DISABLE);
	pak->flags |= PAK_LACK_IN;
	reg_invoke_llc2_input(pak);
	reset_interrupt_level(level);
	return;

      case SDLLC_AW_SDLC_SEC:
	switch(opcode) {
	  case RSRB_OP_LINKDOWN_REQ:	/* tear down, handle */
	  case RSRB_OP_LINKDOWN_PRSP:
	  case RSRB_OP_LINKUP_REQ:
	  case RSRB_OP_LINKUP_PRSP:
	    sdllc_net_ind_disconnect(sdlc_idb, sdb);
	    goto bail_on_net_msg;

	  case RSRB_OP_SDLLC:
	  case RSRB_OP_LLC2DATA:
	  case RSRB_OP_QUENCH_ON:
	  case RSRB_OP_QUENCH_OFF:
	    goto bail_and_discard;

	  default:
	    break;
	}

	/* In the SDLLC_AW_SDLC_SEC state drop all I frames */

	if ((conch & SDLC_SFRAME) == SDLC_IFRAME) {
	    if (sdb->sdllc_lak_enable) {
		lackie = sdb2lak(sdlc_idb, sdb);
		if (lackie)
		    lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_txqhigh);
	    } else {
		level = raise_interrupt_level(NETS_DISABLE);
		llc2_stateswitch(sdb->sdllc_llc, SET_LOCAL_BUSY);
		reset_interrupt_level(level);
	    }
	    sdb->sdllc_local_busy = TRUE;
	}
	/*
	 * RSDLLC I-frames in SDLLC_AW_SDLC_SEC is possible for 2.1 
	 * support. Send a quench message to not let I-frames flood us.
	 */
	/*
	 * SDLLC_AW_SDLC_SEC and not a LACK op.
	 * Fall through and let llc2 handle it.
	 */

      case SDLLC_CONNECT:
	switch(opcode) {
	  case RSRB_OP_LINKDOWN_REQ:
	  case RSRB_OP_LINKDOWN_PRSP:	/* mondo weird, tear it all down */
	  case RSRB_OP_LINKUP_REQ:
	    sdllc_net_ind_disconnect(sdlc_idb, sdb);
	    goto bail_on_net_msg;

	  case RSRB_OP_QUENCH_ON:
	    if (sdllc_debug)
		buginf("\nSDLLC: %s processed %s %s(%02x) %e %e %02x %02x -> %s %02x",
		       sdllc_prstate(sdb, 0), vr_op2str(opcode),
		       lack_prconch(llc1->control), llc1->control, trh->daddr, trh->saddr,
		       llc1->dsap, llc1->ssap, sdlc_idb->hw_namestring, sdb->sdlc_address);
	    sdlc_set_clear_busy(sdlc_idb, sdb->sdlc_address, TRUE);
	    vr_nukepak(pak);
	    return;

	  case RSRB_OP_QUENCH_OFF:
	    if (sdllc_debug)
		buginf("\nSDLLC: %s processed %s %s(%02x) %e %e %02x %02x -> %s %02x",
		       sdllc_prstate(sdb, 0), vr_op2str(opcode),
		       lack_prconch(llc1->control), llc1->control, 
		       trh->daddr, trh->saddr,
		       llc1->dsap, llc1->ssap, sdlc_idb->hw_namestring, 
		       sdb->sdlc_address);
	    sdlc_set_clear_busy(sdlc_idb, sdb->sdlc_address, FALSE);
	    vr_nukepak(pak);
	    return;

	  case RSRB_OP_SEND_XID:
	    if (sdllc_debug)
		buginf("\nSDLLC: Tossing RSRB_OP_SEND_XID in connect state");
	  case RSRB_OP_LINKUP_PRSP:	/* you're up too, that's very nice, I'm happy to know that */
	    goto bail_and_discard;

	  case RSRB_OP_SDLLC:
	  case RSRB_OP_LLC2DATA:
	  default:
	    break;
	}

	/*
	 * In the CONNECT state accept ALL llc2 frames
	 * For non local-ack sessions, pass all llc2 frames to llc2
	 */
	if (!sdb->sdllc_lak_enable) {
	    level = raise_interrupt_level(NETS_DISABLE);
	    pak->flags |= PAK_LACK_IN;
	    reg_invoke_llc2_input(pak);
	    reset_interrupt_level(level);
	    if (!sdb->sdllc_llc) {
		/*
		 * incoming llc frame tore the LLC side completely down
		 * don't do any further processing.
		 */
		return;
	    }
	}

	/*
	 * I-frames processing has the highest priority for performance. If
	 * llc2 approves of the I-frame forward it to the SDLC station.
	 */
	if ((conch & SDLC_IMASK) == SDLC_IFRAME || opcode == RSRB_OP_LLC2DATA) {
	    if (sdb->sdllc_lak_enable) {
		pak->datagramstart += pak->llc2_sapoffset + 4;
		pak->datagramsize -= pak->llc2_sapoffset + 4;
		pak->flags |= PAK_LACK_OUT;
	    }
	    if (pak->flags & PAK_LACK_OUT) {
		/*
		 * LACK_OUT gets set immediately above or by llc2_input if
		 * llc2 accepted this I data packet.
		 */
		pak->if_output = sdlc_idb->firstsw;
		sdllc_sendframe(pak, sdb->sdlc_address);
	    } else if (sdllc_debug)
		buginf("\nSDLLC: %s %0x invalid I-frame discarded by llc2",
		       sdlc_idb->hw_namestring, sdb->sdlc_address);
	    return;
	}

	if (conch == SABME) {
	    if (sdb->sdllc_lak_enable) {
		/*
		 * Oht oh.  We are SDLLC_CONNECT, this sdb has lack turned on but we
		 * got a SABME without a LUR.  This is a bogus configuration.  Blow
		 * the connection.
		 */
		if (sdllc_debug)
		    buginf("\nSDLLC: non-LOCACK SABME from %s not allowed -> %s %02x %s, aborting connection.",
			   vrpeer_id(pak->peer_ptr, peerid),
			   sdlc_idb->hw_namestring, sdb->sdlc_address, sdllc_prstate(sdb, 0));
		sdllc_net_ind_disconnect(sdlc_idb, sdb);
		vr_nukepak(pak);
		return;
	    }
	    if (sdllc_debug)
		buginf("\nSDLLC: SABME for %s %02x in %s (no harm done)",
		       sdlc_idb->hw_namestring, sdb->sdlc_address,
		       sdllc_prstate(sdb, 0));

	    /* 
	     * Add this frame's source address to the RIF cache 
	     * If connection is out, this will already be there. If connection
	     * is in (inbound or inverse sdllc) it will be an aged rif.
	     */
	    rif_update(NULL, 0, trh->saddr, trh->daddr,
		       (srbroute_t *) (riflen ? &trh->rc_blth : NULL),
		       RIF_NOAGE);
	    /*
	     * Note.  This frame has already been seen by llc2
	     * so don't put any code here!
	     */
	    return;
	}
	/*
	 * llc has already seen this frame and eaten it.
	 * There is no need to do anything with this frame.
	 */
	return;
	
      default:
	buginf("\nSDLLC: Invalid sdllc state, %s %d", sdllc_prstate(sdb, 0), sdb->sdllc_state);
	break;
    }
}
/*
 * sdllc_output - Frames received from SDLC are now to be passed over to LLC2.
 * Called from interrupt level in sdlc_input so there is no need to mask
 * interrupts in this routine.
 *
 * sdlc_input will nuke the packet! Don't do it here!!
 */

void sdllc_output (hwidbtype *idb, paktype *pak)
{
    leveltype level;
    sdlc_frametype *frame;
    sdlc_data_block *sdb;
    dlc_oqueue_status_t rc;

    if (!idb || !pak)
	return;
    frame = (sdlc_frametype *)pak->datagramstart;
    sdb = (sdlc_data_block *)idb->sdlc_data[frame->address];
    if (!sdb)
	return;

    if (SDLC_ISIFRAME(frame)) {
	if (sdb->sdllc_state == SDLLC_CONNECT) {
	    if (sdb->sdllc_lak_enable) {
		sdllc_vrforward_data(pak, idb);
		return;
	    }
	    pak->datagramstart += SDLCHEADERBYTES;
	    pak->datagramsize  -= SDLCHEADERBYTES;
	    pak->if_output      = idb->vidb_link->firstsw;
	    pak->flags |= PAK_LACK_OUT;
	    /*
	     *  Note that whether we are going on real token ring or rsrb virtual
	     *  ring, we go through LLC2.  The RSRB hooks are below LLC2.  LLC2 
	     *  will return congested based on the depth of the llc txQ.
	     */

	    if (!sdb->sdllc_llc) {
		datagram_done(pak);
		return;
	    }
	    level = raise_interrupt_level(NETS_DISABLE);
	    rc = llc_get_oqueue_status(sdb->sdllc_llc);
	    if ( rc > DLC_OQUEUE_NORMAL){
		sdlc_set_clear_busy(idb, sdb->sdlc_address, TRUE);
		if (sdllc_debug)
		    buginf("\nSDLLC: LLC2 congested, set SDLC to busy");
	    }
	    if (rc == DLC_OQUEUE_FULL) {
		datagram_done(pak);
		buginf("\nSDLLC: failed to send I-frame, session will close");
		llc2_close(sdb->sdllc_llc); /* calls sdllc_cleanup (yuk) */
	    } else {
		llc2_send(sdb->sdllc_llc, pak, pak->datagramsize);
	    }
	    reset_interrupt_level(level);
	    return;
    } else {
	if (sdllc_debug)
	    buginf("\nSDLLC: Iframe ignored in sdllc_output, state %s",
		   sdllc_prstate(sdb, 0));
	return;
	}
    }
}



/*
 * sdllc_vrforward_data
 * Used by SDLLC local ack sessions to send SDLC data to its llc2 counterpart
 * on the other side of the backbone.  The frame currently has SDLC headers
 * which will be replaced by TR and LLC2 headers.
 */

void sdllc_vrforward_data (paktype  *pak, hwidbtype *idb)
{
   uchar       sa[IEEEBYTES];
   riftype     *rifptr;
   int         riflen;
   int         dgsize;
   int         trhdrsize;
   tring_hdr   *trframe;
   sap_hdr     *llc1;
   sdlc_frametype *frame = (sdlc_frametype *)pak->datagramstart;
   sdlc_data_block *sdb = (sdlc_data_block *)idb->sdlc_data[frame->address];

   clear_if_input(pak);
   pak->if_input = pak->if_output = idb->vidb_link->firstsw;

   /* form the src address */
   ieee_copy( &(idb->sdlc_vr_macaddr[0]), sa);
   sa[5] = frame->address; 
   sa[0] |= TR_RII;

   /* get the rif string */
   rifptr = rif_lookup( &(sdb->sdlc_prtnr[0]), NULL, &(sa[0]), 0);
   if (rifptr)
      riflen = rifptr->length;
   else
      riflen = 0;   

   /* strip off the SDLC header (to be replaced with LLC header) */
   pak->datagramstart += SDLCHEADERBYTES;
   pak->datagramsize  -= SDLCHEADERBYTES;

   trhdrsize = TRING_ENCAPBYTES + riflen + 4;  /* 4 = llc i-frame hdr size */

   /*
    * The last 4 bytes are needed because vrforward_pak() does kludgy things
    * with the TR CRC bytes.  If left out the packet will be 4 bytes short
    * when it emerges at the other end!!
    */
   dgsize =  trhdrsize + pak->datagramsize + TR_FCS_SIZE;

   pak->datagramstart -= trhdrsize; 
   pak->datagramsize   = dgsize;

   /* fill in the TR mac header */
   trframe     = (tring_hdr *)pak->datagramstart;
   trframe->ac = AC_PRI0;
   trframe->fc = FC_TYPE_LLC;

   ieee_copy( &(sdb->sdlc_prtnr[0]), &(trframe->daddr[0]));
   ieee_copy( &(sa[0]), &(trframe->saddr[0]));

   /* fill in the rif */
   bcopy( rifptr->rif, trframe->data, riflen );

   /* fill in the llc2 header */
   llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + riflen);
   llc1->dsap = sdb->sdlc_dsap;
   llc1->ssap = sdb->sdlc_ssap;

   /* send it!! */
   vrforward_pak(pak, TRUE, RSRB_OP_LLC2DATA, RSRB_RIFLF_LEAVEALONE);
}


/*
 * s d l l c _ c l e a n u p _ l l c
 * 
 * Callback routine to close a session because the LLC code told us to.
 */
void sdllc_cleanup_llc (llctype *llc)
{
    sdlc_data_block *sdb;

    if (llc == NULL) { 
        if (sdllc_debug)
	    buginf("\nSDLLC: sdllc_cleanup_llc:  null llc pointer");
        return;
    }

    if (!llc->sdllc_active) {
        if (sdllc_debug)
	    buginf("\nSDLLC: sdllc_cleanup_llc:  sdllc not active");
        return;
    }               

    if (!llc->sdllc_idb) {
	if (sdllc_debug)
	    buginf("\nSDLLC_ERR: sdllc_cleanup, no sdllc idb %02x", llc2_get_lmac(llc)[5]);
	return;
    }

    if (sdllc_debug)
	buginf("\nSDLLC: removing rif for %e %e",
	       llc2_get_rmac(llc),
	       llc2_get_lmac(llc));

    unset_rif(NULL, 0, llc2_get_rmac(llc), llc2_get_lmac(llc), FALSE);

    if (llc->sdllc_idb) {
	sdb = (sdlc_data_block *) llc->sdllc_idb->sdlc_data[llc2_get_lmac(llc)[5]];
	if (!sdb) {
	    buginf("\nSDLLC: Warning! SDLLC Cleanup with null SDB! Addr = %02x",
		   llc2_get_lmac(llc)[5]);
	    return;
	}
	/*
	 * shutting the llc, force the sdlc to reset by calling
	 * sdlc_setstate(DISCSENT).  Set sdb->sdllc_llc to NULL to prevent
	 * sdlc from calling us back.  This is very important to do this
	 * NULLing before calling sdlc_setstate.
	 *
	 * Note it is also important to set sdllc_state to DISCONNECT prior to
	 * hitting the sdlc side.  Since the network side is already down we
	 * don't want to kill it again.
	 */
	sdb->sdllc_llc = NULL;
	llc->sdllc_active = FALSE;
	if (sdb->sdllc_state >= SDLLC_DW_NET) {
	    /*
	     * If we are network disconnect phase then clean up for real.
	     * Or if we are asyncronously disconnecting (ie. llc or the network
	     * is yanking our chain).  Ie we were in a connect phase.
	     * 
	     * Otherwise we are handling the special disconnect case
	     * where we have handled an incoming LLC of somekind, have
	     * processed it, then tore the session back down.  Don't
	     * fuck with the sdllc state.
	     */
	    sdllc_setstate(llc->sdllc_idb, sdb, SDLLC_DISCONNECT, "LLC cleanup");
	    if (sdb->sdlc_state >= SDLC_DISCSENT) {
		sdlc_setstate(llc->sdllc_idb, SDLC_DISCSENT, sdb);
		sdlc_set_pause_timer(llc->sdllc_idb, sdb);
	    } else
		sdlc_setstate(llc->sdllc_idb, SDLC_DISCONNECT, sdb);
	}
	llc->sdllc_idb = NULL;
    }
}


/*
 * sdllc_fix_llc
 * wrapper for original function to allow SDLLC code to stay in SDLLC
 */
void sdllc_fix_llc (llctype *llc, paktype **ppak)
{
    paktype *pak = *ppak;	/* simplify indirection below */
    
    if (!sdllc_fix_llc_action(llc, pak)) {

	/*
	 * if sdllc_fix_llc fails it means that the sdllc outcall
	 * function doesn't want us to accept this connection.  Blow
	 * it up and send a DM in response to the SABME.
	 *
	 * Note that the llc connection block is in ADM.  Calling
	 * llc2_close will clean this up in a reasonable fashion.
	 */

	llc->adm_reason = LC_ABORTED;
	llc2_close(llc);

	llc2_respond_with_dm(pak);

	protocol_discard(pak, TRUE);
	*ppak = NULL;		/* signal error to caller */
    }
}
    
/*
 * sdllc_fix_llc_action
 * old sdllc_fix_llc(), now in wrapper function above to better
 * handle error condition (ie make readable)
 */

boolean sdllc_fix_llc_action (llctype *llc, paktype *pak)
{
   trrif_t         *trh; 
   hwidbtype         *idb;  
   boolean          match = FALSE;
   sdlc_data_block *sdb = NULL;


   trh = (trrif_t *)pak->datagramstart;

   /*
    * scan thru all the idb blocks for a match on the sdllc vr tr address.
    */
   FOR_ALL_HWIDBS(idb) {
      if (!is_sdlc(idb))
         continue;
      if (!idb->sdllc_active)
	  continue;

      if (bcmp(trh->daddr, idb->sdlc_vr_macaddr, IEEEBYTES - 1) == 0) {
	 sdb = (sdlc_data_block *) idb->sdlc_data[trh->daddr[IEEEBYTES - 1]];
	 if (!sdb) {
	     buginf("\nSDLLC:Warning: Doing a FIX LLC function when SDLC not defined for addr %x",
		    trh->daddr[IEEEBYTES - 1]);
	     return(FALSE);
	 }
         match = TRUE;
         break;
      }
   }
    if (match) {
	sdb->sdllc_llc = llc;
	llc->sdllc_address = trh->daddr[5]; /* last byte of virtual MAC address is sdlc address */
	llc->sdllc_active = TRUE;
	llc->media_llc_cleanup = sdllc_cleanup_llc;
	llc->sdllc_idb    = idb;
	if (sdb->sdllc_state == SDLLC_AW_NET) {
	    /*
	     * Only do a real state transition if this PU is in the
	     * right state for a network connect.
	     *
	     * It is only a real network connect if we are in the right
	     * state.
	     *
	     * Note to accept the connection the interface must be up.
	     * If not that is a transient down and we need to clean up.
	     * This is an artifact of interface state hysterisis.
	     */
	    if (idb->state != IDBS_UP)
		return(FALSE);
	    sdllc_net_ind_connect(idb, sdb);
	}
      }
   return(TRUE);
}




boolean sdllc_get_sdb (paktype *pak, hwidbtype **sdlc_idb, sdlc_data_block **sdb)
{
    hwidbtype     *idb;
    boolean	match = FALSE;
    int		addr;
    uchar       blah;
    trrif_t    *trh;               /* pointer to TR MAC header */
    
    trh = (trrif_t *) pak->datagramstart;
    /*
     * find which sdlc interface this packet is pointed at.
     */
    FOR_ALL_HWIDBS(idb) {
	  if (idb->enctype != ET_SDLCP && idb->enctype != ET_SDLCS)
	      continue;
	  if (!idb->sdllc_active)
	      continue;
	  if (bcmp(trh->daddr, idb->sdlc_vr_macaddr, IEEEBYTES - 1) == 0 ) {
		match = TRUE;
	    break;
	}
    }
    if (!match) {
	if (sdllc_debug)
	    buginf("\nSDLLCERR: no idb, pak dropped, da %e, sa %e, rif %s",
		   trh->daddr, trh->saddr,
		   get_rif_str((uchar *) &trh->rc_blth, trprint_rifstr));
	vr_nukepak(pak);
	return(FALSE);
    }
    *sdlc_idb = idb;
    addr = trh->daddr[IEEEBYTES - 1];
    *sdb  = (sdlc_data_block *) idb->sdlc_data[addr];
    if (!*sdb) {
	if (sdllc_debug)
	    buginf("\nSDLLCERR: no sdb block, pak dropped, da %e, sa %e, rif %s",
		   trh->daddr, trh->saddr,
		   get_rif_str((uchar *) &trh->rc_blth, trprint_rifstr));
	vr_nukepak(pak);
	return(FALSE);
    }
    /*
     * If partner address does not match discard
     */
    blah = trh->saddr[0];
    trh->saddr[0] &= ~TR_RII;
    if (!(*sdb)->sdlc_have_prtnr ||
	  !ieee_equal((*sdb)->sdlc_prtnr, trh->saddr)) {
	trh->saddr[0] = blah;
	if (sdllc_debug)
	    buginf("\nSDLLCERR: not from our partner, pak dropped, da %e, sa %e, rif %s, partner = %e",
		   trh->daddr, trh->saddr,
		   get_rif_str((uchar *) &trh->rc_blth, trprint_rifstr), (*sdb)->sdlc_prtnr);
	vr_nukepak(pak);
	return(FALSE);
    }
    (*sdb)->sdlc_idb = (*sdlc_idb);
    trh->saddr[0] = blah;
    return(TRUE);
}

 
 
/*
 * virtualp_sdllc - the virtualp vector for sdllc.
 * Returns TRUE if SDLLC macaddr matches passed in 
 * macaddr for the vre.
 */
static boolean virtualp_sdllc(tr_ventrytype *vre, uchar* macaddr)
{
    if ( (vre->type & RSRB_ET_SDLLC) &&
         !bcmp(vre->macaddr, macaddr, IEEEBYTES-1) ) {
         return TRUE;
    }
    return FALSE;
}

/*
 * sdllc_add_vring_interface - Add an "sdllc virtual ring" to a virtual 
 * ring.
 */
boolean sdllc_add_vring_interface (hwidbtype *idb, ushort local_ring,
				   uchar bridge_num, ushort target_ring)
{
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;

    vrg = vrfind_ringgroup(target_ring);
    if (!vrg)
      return(FALSE);

    vre = malloc_vre(NULL);
    if (!vre)
       return(FALSE);

    vre->virtual_idb = 
               sdllc_new_vidb(idb, local_ring, bridge_num, target_ring);

    if (!vre->virtual_idb) {
       free(vre);
       return(FALSE);
    }

    vre->bn      = bridge_num;
    vre->rn      = local_ring;
    vre->type    = RSRB_ET_SDLLC;
    vre->outhole.idb = vre->virtual_idb;

    vre->virtual_idb->tr_vring_blk = (tr_ventrytype *)&vrg->rings.qhead;
    vre->virtualp = virtualp_sdllc;
    ieee_copy(&(idb->sdlc_vr_macaddr)[0], vre->virtual_idb->bia);
    ieee_copy(&(idb->sdlc_vr_macaddr)[0], vre->virtual_idb->hardware);
    ieee_copy(&(idb->sdlc_vr_macaddr)[0], vre->macaddr);
    ieee_copy(&(idb->sdlc_vr_macaddr)[0], idb->hardware);


    enqueue(&vrg->rings, vre);
    vrg->flags |= VRGF_GENUPDATE;
    update_peers();
    return(TRUE);
}


static boolean sdllc_active (hwidbtype *idb)
{
    return (idb->sdllc_active);
}

static void sdllc_vidb_input (hwidbtype *idb, paktype *pak, uint opcode, tr_ventrytype *vre)
{
    trrif_t         *trh;               /* pointer to TR MAC header */
    int             riflen;		   /* length of RIF in TR header */
    sap_hdr         *llc1;              /* pointer to llc-1 header */
    hwidbtype       *sdlc_idb;	   /* idb of the sdlc interface */
    sdlc_data_block *sdb;

    if (!idb || !pak)
	return;
    GET_TIMESTAMP(idb->lastinput);	/* remember time of last input */
    pak->inputtime = idb->lastinput;	/* save it in packet too */

    trh = (trrif_t *) pak->datagramstart;
    riflen = (trh->saddr[0] & TR_RII) ? trh->rc_blth & RC_LTHMASK : 0;
    idb->counters.rx_cumbytes += pak->datagramsize;
    idb->counters.inputs++;
    llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + riflen);

    if (!rsrb_opcode_valid(opcode)) {
	buginf("\nSDLLC_ERR: Bad opcode %d from %e %e %02x %02x %s (%02x)",
	       opcode, trh->daddr, trh->saddr, llc1->dsap, llc1->ssap,
	       lack_prconch(llc1->control), llc1->control);
	vr_nukepak(pak);
	return;
    }

    if (!sdllc_get_sdb(pak, &sdlc_idb, &sdb)) {
        /* 
         * If returns false, then pak is already nuked.
         *     motto 08/07/95 (CSCdi38330).
         */
        return;
    }

    if (!sdlc_idb || !sdb) {
	vr_nukepak(pak);
	return;
    }

    sdllc_input(idb, pak, opcode, sdlc_idb, sdb, vre);
}



hwidbtype *sdllc_new_vidb (hwidbtype *inidb, 
			   int thisring, int bridgenum, int targetring)
{
    hwidbtype *idb;

    idb = dlc_new_vidb(inidb, "SdllcVirtualRing", 
		       thisring, bridgenum, targetring);
    if (idb == NULL)
	return(NULL);

    idb->oqueue            = rsrb_sdllc_oqueue;
    idb->iqueue            = (iqueue_t) sdllc_vidb_input;
    idb->sdllc_active  = TRUE;

    idb->sdllc_largest_token_frame   = SDLLC_DEFAULT_TLF;
    inidb->sdllc_largest_token_frame = SDLLC_DEFAULT_TLF;
    return(idb);
}


/*
 * Raw enqueue for sdllc rings
 */
boolean rsrb_sdllc_oqueue (hwidbtype *idb, paktype *pak, enum HEADTAIL dummy)
{
   clear_if_input(pak);
   pak->if_input = idb->firstsw;
   pak->flags   &= ~PAK_LACK_OUT;
   vrforward_pak(pak, TRUE, RSRB_OP_SDLLC, RSRB_RIFLF_LEAVEALONE);
   return(TRUE);
}


/*
 * sdllc_remove_vring (idb, int)
 * Destroy a virtual ring associated with an IDB
 */

boolean sdllc_remove_vring (hwidbtype *idb, int rsrb_et)
{
    srb_sb_t        *srb_sb;
    tr_vgrouptype   *vrg;
    tr_ventrytype   *vre;
    leveltype        level;

    if (!idb)
	return (FALSE);

    srb_sb = srb_get_sb_inline(idb->firstsw);
    if (!srb_sb)
	return (FALSE);

    if (!(rsrb_et == RSRB_ET_SDLLC &&
	(idb->enctype == ET_SDLCP || idb->enctype == ET_SDLCS) &&
	idb->sdllc_active))

	return(FALSE);

    level = raise_interrupt_level(NETS_DISABLE);
    vrg = vrfind_ringgroup(srb_sb->srb_targetring);
    if (vrg) {
	vre = vrfind_rn2entry(srb_sb->srb_thisring, RSRB_BN_DONTCARE,
			     (tr_ventrytype *)&vrg->rings.qhead, RSRB_ET_SDLLC);
	if (vre) {
	    unqueue(&vrg->rings, vre);
	    if (vre->virtual_idb) {
                  /*
                   * CSCdi61499: before the vidb gets freed, we have to
                   * free the llc2 ctl blk which associates with this vidb.
                   * these llc2 ctl blks are supposed to be freed when sdlc
                   * is shut down.  However, sdlc_cstate() is called in the
                   * background process.  "no sdllc traddr" might get
                   * executed before the sdlc_cstate() being called.  If the
                   * vidb is freed but llc2 ctl blks are not, router will
                   * crash if llc2->idb gets referenced.
                   */
                  vre->virtual_idb->state = 0;
                  llc2_cstate(vre->virtual_idb);
                  vidb_free(vre->virtual_idb->firstsw);
		  vre->virtual_idb = NULL;
		  idb->vidb_link = NULL;
	    }
	    free(vre);
	}
	vrg->flags        |= VRGF_GENUPDATE;
    }
    reset_interrupt_level(level);

    ieee_zero(idb->sdlc_vr_macaddr);

    srb_sb->srb_thisring   = SRB_INVALID_RING_NUM;
    srb_sb->srb_bridge_num = SRB_INVALID_BRIDGE_NUM;
    srb_sb->srb_targetring = SRB_INVALID_RING_NUM;

    idb->sdllc_active  = FALSE;

    srb_sb->srb_max_hops   = SRB_DEF_MAXHOPS;
    srb_sb->srb_maxout_hops = SRB_DEF_MAXHOPS;
    srb_sb->srb_maxin_hops  = SRB_DEF_MAXHOPS;

    srb_sb->srb_spanning_explorer = FALSE;

    return(TRUE);
}


/*
 * sdllc encaps
 * Configure SDLLC (media-translation) parameters.  All SDLLC configuration
 * commands are interface sub-commands.  They are configured for the serial
 * line which contains the sdlc station which needs to communicate with
 * its token ring partner. 
 */

void sdllcif_command (parseinfo *csb)
{
    register hwidbtype *idb;
    boolean flag = TRUE;
    int i;

    idb = csb->interface->hwptr;
    if (idb->enctype != ET_SDLCP && idb->enctype != ET_SDLCS) {
	  if (!csb->nvgen)
	      printf("\nsdllc commands only allowed on SDLC configured serial lines");
	  return;
      }
    if (!bridge_enable) {
	if (!csb->nvgen)
	    printf("\nSource Route Bridging not supported in this release");
	return;
    }
    if (csb->nvgen) {
	if (!is_sdlc(idb)) {
	    return;
	}
	switch (csb->which) {
	  case SDLLC_TRADDR:
	    if ((idb->enctype == ET_SDLCP) ||
		(idb->enctype == ET_SDLCS)) {
 		for (i = 0; i < IEEEBYTES; i++) 
 		    if (idb->sdlc_vr_macaddr[i] != 0) 
 			flag = FALSE;
 		if (flag) {
 		    return;
                } else {
                    srb_triplet_t srb_triplet;
                    srb_sb_get_triplet(idb->firstsw, &srb_triplet);
		    nv_write(TRUE, "%s %e %d %d %d", csb->nv_command,
			     idb->sdlc_vr_macaddr, 
                             srb_triplet.localRing,
			     srb_triplet.bridgeNum, 
                             srb_triplet.remoteRing);
                }
	    }
	    break;

	  case SDLLC_TRLF:
	    nv_write((idb->sdllc_largest_token_frame != SDLLC_DEFAULT_TLF),
		     "%s %u", csb->nv_command,
		     rc_code_to_bytes(idb->sdllc_largest_token_frame));
	    break;

	  case SDLLC_SAPS:
	  case SDLLC_PRTNR:
	  case SDLLC_XID:
	  case SDLLC_SDLCLF:	/* taken care of by sdlc_encaps */
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    } /* endIf     if (csb->nvgen) */

    switch (csb->which) {
      case SDLLC_TRADDR:
        sdllc_config_traddr(csb, idb);
        break;

      case SDLLC_PRTNR:
        sdlc_config_partner(csb, idb);
        break;

      case SDLLC_XID:
        sdlc_config_xid(csb, idb);
	break;

      case SDLLC_TRLF:
        sdllc_config_trlf(csb, idb);
        break;

      case SDLLC_SDLCLF:
        sdlc_config_sdlclf(csb, idb);
        break;

      case SDLLC_SAPS:
        sdlc_config_saps(csb, idb);
        break;

      default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }

}

/*
 * Configure the serial sdlc line as a virtual token ring device.  This is
 * needed to make the serial device appear to its token ring partner as a
 * token ring device.  To accomplish this the serial device is given a
 * virtual token ring address called the sdllc virtual tr address and the
 * serial line is made to appear as a virtual token ring called the sdllc
 * virtual ring number.  Both the sdllc tr address and the sdllc virtual ring
 * number need to unique across the network.
 */

static void sdllc_config_traddr (parseinfo *csb, hwidbtype *idb)
{
    int target_ring, bridge_num;
    uchar newmac[IEEEBYTES];
    ushort sdllc_vr;
    
    hwidbtype *check_idb;
    sdlc_data_block *sdb;
    int addr;
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    srb_sb_t      *srb_sb;
   
    if (!idb) {
      return;
    }
 
    if ((idb->enctype != ET_SDLCS) && (idb->enctype != ET_SDLCP)) {
	printf("\nTRADDR is only valid for SDLC-PRIMARY or SDLC-SECONDARY encapsulation");
	return;
    }

    if (!csb->sense) {
	  if (idb->state != IDBS_ADMINDOWN) {
		printf("\nInterface must be shutdown before removing this command");
		return;
	    }
	  sdllc_remove_vring(idb, RSRB_ET_SDLLC);
	  return;
      }
    
    ieee_copy((char *)GETOBJ(hwaddr,1)->addr, newmac);
    if (newmac[0] & 0x80)
    {
	printf("\nMalformed hex mac address - group bit set");
	return;
    }

    if (newmac[5] != 0x0) {
	printf("\nSorry - the last two digits of the mac address must be 00");
	return;
    }
    /*
     * get the sdllc virtual ring number.  This number is distinct from the
     * ring group number.  The serial line is made to appear as a virtual
     * ring to its remote partner and this virtual ring is the sdllc virtual
     * ring number.
     */
    sdllc_vr = (ushort) GETOBJ(int,1);
    /*
     * get the bridge number.
     */
    bridge_num = (ushort) GETOBJ(int,2);
    /*
     * get the target ring number which should be the same as the ring-group
     * number already configured with the "source-bridge ring-group" command.
     */
    target_ring = (ushort) GETOBJ(int,3);
    

    /* sanity check */
    if (target_ring == sdllc_vr) {
	  printf("\nlocal-ring and target-ring numbers must be different");
	  return;
    }

    if (vrfind_ringgroup(sdllc_vr)) {
	  printf("\nsdllc ring number must be different than the RSRB virtual ring");
	  return;
    }

    if ( !vrfind_ringgroup(target_ring) ) {
       printf("\ntarget ring should be the same as the ring-group");
       printf("\nas configured by the source-bridge ring-group command");
       return;
    }
    vrg = vrfind_ringgroup(target_ring);
    if (!vrg) {
        printf("\nsdllc target ring needs to be a defined RSRB virtual ring");
        return;
    }


      /*
       * check to see if the new mac address is duplicated anywhere in this
       * box.
       */
    FOR_ALL_HWIDBS(check_idb) {
	  if (check_idb == idb) /* currently parsing this idb */ 
	      continue;
	  if (check_idb->enctype == ET_SDLCP || check_idb->enctype == ET_SDLCS) {
	      if (bcmp(newmac, check_idb->sdlc_vr_macaddr, IEEEBYTES - 1) == 0)
		  printf("\n  Warning: Address %e is also in use by interface %s",
			 newmac, check_idb->hw_namestring);
	  } else if (check_idb->status & IDB_IEEEMASK) {
	      if (bcmp(newmac, check_idb->hardware, IEEEBYTES - 1) == 0)
		  printf("\n  Warning: Address %e is also used by physical interface %s",
			 newmac, check_idb->hw_namestring);
	  } else
	      continue;
	  if (sdllc_vr == srb_sb_get_thisring(check_idb->firstsw)) {
	      printf("\n  The ring number %d (0x%03x) is already in use by %s, aborting.",
		     sdllc_vr, sdllc_vr, check_idb->hw_namestring);
	      return;
	  }
      }

      for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
	  if (srb_sb_get_thisring(idb->firstsw) != sdllc_vr)  { 
	      if (vre->rn == sdllc_vr ) {
		  printf("\n  The ring number %d (0x%03x) is already in use by ring group %d , aborting.",
			 sdllc_vr, sdllc_vr, vrg->vrn);
		  return;
	      }
	  }
	  if ( bcmp(idb->sdlc_vr_macaddr, newmac, IEEEBYTES - 1 ) != 0 ) {
	      if (bcmp(newmac, vre->macaddr, IEEEBYTES - 1) == 0){
		  printf("\n  Warning: Address %e is also in use by virtual ring %d ; ring group %d",
			 newmac, vre->rn, vrg->vrn);
		  return;
	      }
	  }
	  for (addr = SDLC_MIN_ADDRESS; addr <= SDLC_MAX_ADDRESS; addr++) {
	      sdb = (sdlc_data_block *)idb->sdlc_data[addr];
	      if (!sdb) 
		  continue;
	      if ( sdb->sdlc_have_prtnr &&
		  ( bcmp(sdb->sdlc_prtnr, newmac, IEEEBYTES - 1 ) == 0 )) {
		  printf("\n  Warning: Address %e is also used for partner address for sdlc address = %02x ",
			 newmac, sdb->sdlc_address);
		  return;
	      }
	  }
      }


    srb_sb = srb_get_or_create_sb_inline(idb->firstsw);
    if (!srb_sb) {
        printf(nomemory);
        return;
    }

    /* Destroy old one? */
    if (srb_sb->srb_targetring) {
	if (idb->state != IDBS_ADMINDOWN) {
	    printf("\nInterface must be shutdown before issuing this command");
	    return;
	}
	if ( (srb_sb->srb_thisring   != sdllc_vr) ||
	     (srb_sb->srb_bridge_num != bridge_num) ||
	     (srb_sb->srb_targetring != target_ring) ||
	     (bcmp(idb->sdlc_vr_macaddr, newmac, IEEEBYTES - 1 ) != 0 ) ) {
	    printf("\n  New description differs from old : ");
	    printf("\n    SDLLC traddr %e %d %d %d (closing connections)", 
		   idb->sdlc_vr_macaddr,
		   srb_sb->srb_thisring, 
                   srb_sb->srb_bridge_num,
                   srb_sb->srb_targetring);
	    sdllc_remove_vring(idb, RSRB_ET_SDLLC);
	}
	else {
	    printf("\n Nothing changed in the config (!)");
	    return;
	}
    }
    /* Commit point for new or modified ring */
    bcopy(newmac, idb->sdlc_vr_macaddr, IEEEBYTES - 1);

    srb_sb->srb_thisring   = sdllc_vr;
    srb_sb->srb_bridge_num = bridge_num;
    srb_sb->srb_targetring = (ushort) target_ring;

    idb->tr_transit_oui = DEFAULT_OUI;

    sdllc_add_vring_interface(idb, sdllc_vr, bridge_num, target_ring);
    idb->sdllc_active = TRUE;
    sdllc_start();
    llc2_start();

    llc2_register(SAP_IBM_SNA, LINK_LLC2, lack_sdllc_register);
    start_srb_background();
}


static void sdllc_config_trlf (parseinfo *csb, hwidbtype *idb)
{
    int largest;

    if ((!idb->sdllc_active) && (idb->enctype != ET_SDLC)){
	printf("\nSDLLC not enabled for this interface. ");
	printf("\nUse SDLC encapsulation or a TRADDR command first");
	return;
    }

    if (!csb->sense)
        largest = SDLLC_DEFAULT_TLF;
    else {
        if (GETOBJ(int, 1) == 0) {
           largest = SDLLC_DEFAULT_TLF;
        } else {
           largest = bytes_to_rc_code(GETOBJ(int, 1));

           if (largest == -1) {
              printf("Valid largest frame sizes are %d, %d, %d, %d, %d, %d, and %d.",
                     RC_LF516_VAL, RC_LF1470_VAL, RC_LF1500_VAL, RC_LF2052_VAL,
		     RC_LF4472_VAL, RC_LF8144_VAL, RC_LF11407_VAL, RC_LF11454_VAL,
		     RC_LF17800_VAL);
               return;
           }
        } 
    }

    if (idb->vidb_link)
       idb->vidb_link->sdllc_largest_token_frame = largest;

    idb->sdllc_largest_token_frame = largest;
}


void lack_sdllc_register (llctype *cookie)
{
}


void show_sdllc(parseinfo *csb)
{
    hwidbtype *target, *idb;
    idbtype *swidb;
    int addr;
    sdlc_data_block *sdb;
    boolean onceflag;

    swidb = GETOBJ(idb, 1);
    target = swidb ? swidb->hwptr : NULL;

    onceflag = FALSE;
    automore_enable("");
    FOR_ALL_HWIDBS(idb) {
        srb_triplet_t srb_triplet;
        if (target && (target != idb))
            continue;
        if (!idb->sdllc_active || ((idb->enctype != ET_SDLCP) &&
                                   (idb->enctype != ET_SDLCS)))
                   continue;
        if (onceflag) {
            if (target != NULL)
                return;
	    automore_conditional(0);
        } else
            onceflag = TRUE;
        printf("\n%s %d is %s, ", idb->name, idb->unit,
                     print_hwidbstate(idb));
        printf(" %s ",
                     ISPSDLC(idb) ? "Primary" : "Secondary");

        srb_sb_get_triplet(idb->firstsw, &srb_triplet);
        printf(" MAC : %02x%02x.%02x%02x.%02x--, ring: %d bridge: %d, target ring: %d",
               idb->sdlc_vr_macaddr[0], idb->sdlc_vr_macaddr[1], idb->sdlc_vr_macaddr[2],
               idb->sdlc_vr_macaddr[3], idb->sdlc_vr_macaddr[4], 
               srb_triplet.localRing,
               srb_triplet.bridgeNum,
               srb_triplet.remoteRing);

printf("\n\taddr        state           xid             partner             lack");
        for (addr = SDLC_MIN_ADDRESS; (addr <= SDLC_MAX_ADDRESS); addr++) {
              sdb = idb->sdlc_data[addr];
              if (!sdb)
                  continue;
              printf("\n\t%x", sdb->sdlc_address);
              printf("\t%s", sdllc_prstate(sdb, 0));
              if (sdb->sdlc_xid_configured)
                  printf("\t%02x%02x%02x%02x", sdb->sdlc_xid[0],
                         sdb->sdlc_xid[1], sdb->sdlc_xid[2],
                         sdb->sdlc_xid[3]);
              else
                  printf("\tDisabled");
              if (sdb->sdlc_have_prtnr)
                  printf("\t%e", sdb->sdlc_prtnr);
              else
                  printf("\tDisabled");
              if (sdb->sdllc_lak_enable)
                  printf("\t    Enabled");
              else
                  printf("\t    Disabled");
          }
    }
    automore_disable();
}


/*
 * s d l l c _ i n i t i a t e _ p u
 *
 * Send a TEST frame to establish RIF.
 */

int sdllc_initiate_pu ( hwidbtype *idb, sdlc_data_block *sdb)
{
    uchar macaddr[IEEEBYTES];
    riftype *rif;
    paktype *xid_pak, *test_pak;
    char sdlc_rifstr[SRB_MAXRIF*2+SRB_MAXRIF/2];

    if (!idb || !sdb || !sdb->sdlc_have_prtnr || sdb->sdllc_state != SDLLC_AW_NET)
        return(0);

    ieee_copy(idb->sdlc_vr_macaddr, macaddr);
    macaddr[5] = sdb->sdlc_address;

    /* 
     * Don't do anything for inverse SDLLC.  The t/r device "dials in" as if 
     * connected to a FEP or 3174.
     * Ditto for configured "inbound" connection.  
     * We will respond to incoming test and xid in sdllc_opening_xchg.
     * NEW: sdb->outbound can override inverse SDLLC default behavior.  It allows
     * inverse SDLLC to explore for downstream devices.
     */
    if (idb->enctype == ET_SDLCP && sdb->sdlc_inbound)  
	return(0);

    if (idb->enctype == ET_SDLCS && !sdb->sdlc_outbound)
	return(0);

    /*
     * clear any rif that might be there in the wrong direction.  This can
     * happen from FRMR or a local-ack packet.
     */ 
    unset_rif(0, 0, macaddr, sdb->sdlc_prtnr, FALSE);

    if ( (rif = rif_lookup(sdb->sdlc_prtnr, NULL, macaddr, 0)) ) {
        if (sdb->sdlc_xid_count++ > idb->sdlc_n2) {
	    sdb->sdlc_xid_count = 0;
	    if (sdllc_debug)
		buginf("\nSDLLC: sdlc n2 expired for XID, exploring again, dst %e, src %e", 
		       sdb->sdlc_prtnr, macaddr);
	    unset_rif(NULL, 0, sdb->sdlc_prtnr, macaddr, FALSE);
	    return(0); 
        } 

	xid_pak = tr_setup_xid(sdb->sdlc_prtnr, macaddr, idb->vidb_link,
			       rif->rif, TRUE, sdb->sdlc_address, sdb->sdlc_dsap, 
			       sdb->sdlc_ssap, TRUE);
	if (xid_pak) {
	    if (sdllc_debug)
		buginf("\nSDLLC: O xid(0T2), %e %e %x %x [%s]",
		       sdb->sdlc_prtnr, macaddr, sdb->sdlc_dsap, sdb->sdlc_ssap,
		       get_rif_str((uchar *) rif->rif, sdlc_rifstr));
	    sdllc_datagram_out((paktype *) xid_pak, RSRB_OP_SEND_XID);
	    return(LLC1_XID);
	} else if (sdllc_debug)
	    buginf("\nSDLLC: no buf xid, %e %e %x %x [%s]",
		   sdb->sdlc_prtnr, macaddr, sdb->sdlc_dsap, sdb->sdlc_ssap,
		   get_rif_str((uchar *) rif->rif, sdlc_rifstr));
	return(0);
    } else {
        /* No RIF. Send out an explorer frame */
        test_pak = llc1_gen_explr(sdb->sdlc_prtnr, macaddr, 
				  SAP_NULL, sdb->sdlc_ssap, idb->vidb_link);
        if (test_pak) {
	    sdb->sdlc_xid_count = 0;
            if (sdllc_debug)
		buginf("\nSDLLC: O TEST, dst %e src %e dsap %x ssap %x",
		       sdb->sdlc_prtnr, macaddr, SAP_NULL, sdb->sdlc_ssap);
            test_pak->flags |= PAK_SRB_IN;
            test_pak->if_input = idb->vidb_link->firstsw;
            srb_enq(srb_explorerQ, test_pak);
            return(LLC1_TEST);
        } else if (sdllc_debug)
            buginf("\nSDLLC: no buf TEST, %e %e %x %x",
                   sdb->sdlc_prtnr, macaddr, SAP_NULL, sdb->sdlc_ssap);
    }
    return(0);
}

void sdllc_sec_conn_tear_down (void)
{
    hwidbtype *idb;
    sdlc_data_block *sdb;
    int addr;
	/*
	 * CONNECTION BRING DOWN for local as well as remote lack case.
	 * It tears the connection down after a time out interval
         * This happens when we decide to start a connection based 
	 * on a SNRM seen
	 * and the FEP gives up on us after we start the TR side.
	 */
    FOR_ALL_HWIDBS(idb) {
	if ( (idb->enctype != ET_SDLCS) ||
	    (!idb->sdllc_active) )
	    continue;
	for (addr = SDLC_MIN_ADDRESS; addr <= SDLC_MAX_ADDRESS; addr++) {
	    sdb = (sdlc_data_block *)idb->sdlc_data[addr];
	    if (!sdb || sdb->sdllc_state != SDLLC_AW_NET)
		continue;
	    if (CLOCK_OUTSIDE_INTERVAL(sdb->sdlc_sec_linkup_time, SDLC_SEC_LINKUP_DEFAULT)) 
		sdllc_ind_disconnect(idb, sdb);
	}
    }
}

void sdllc_setstate (
    hwidbtype *idb,
    sdlc_data_block *sdb,
    sdllc_state_t newstate,
    char *why)
{
    sdllc_state_t prevstate;
    uchar saddr[IEEEBYTES];

    prevstate = sdb->sdllc_state;
    if (prevstate != newstate) {
        sdb->sdllc_state = newstate;
        if (newstate == SDLLC_DISCONNECT || newstate == SDLLC_AW_NET) {
            sdb->sdllc_lak_enable = FALSE;
            sdb->sdllc_local_busy = FALSE;
            TIMER_STOP(sdb->time_linkup_sent);
	    ieee_copy(idb->sdlc_vr_macaddr, saddr);
   	    saddr[5] = sdb->sdlc_address;
    	    unset_rif(0, 0, sdb->sdlc_prtnr, saddr, FALSE);
   	    unset_rif(0, 0, saddr, sdb->sdlc_prtnr, FALSE);
        } else if (newstate == SDLLC_CONNECT) {
            TIMER_STOP(sdb->time_linkup_sent);
	    errmsg(&msgsym(ACT_LINK, SDLLC), idb->hw_namestring, 
			sdb->sdlc_address, why) ;
        }
        if (prevstate >= SDLLC_CONNECT && newstate < SDLLC_CONNECT)
	    errmsg(&msgsym(DACT_LINK, SDLLC), idb->hw_namestring, sdb->sdlc_address, why);
        if (sdllc_debug)
	    buginf("\nSDLLC_STATE: %s %02x %s -> %s", idb->hw_namestring, sdb->sdlc_address,
		    sdllc_prstate(NULL, prevstate), sdllc_prstate(sdb, 0));
        reg_invoke_sdllc_trap (sdb);
    }
}


/*
 * s d l l c _ p r s t a t e
 *
 * Return a string describing the current state of an
 * sdllc component or for a state value.
 *
 * input:       sdb     sdlc descriptor pointer.  If non-null then
 *                      return state string for sdb->sdllc_state.
 *              state   if sdb is null return the state string for
 *                      this state value.
 * output:      char *  pointer to a string that is the state.
 */

char *sdllc_prstate (sdlc_data_block *sdb, sdllc_state_t state)
{
    if (sdb)
        state = sdb->sdllc_state;
    switch (state) {
      case SDLLC_DISCONNECT:    return("DISCONNECT    ");
      case SDLLC_DW_NET:        return("NET DISC WAIT ");
      case SDLLC_DW_SDLC:       return("SDLC DISC WAIT");
      case SDLLC_AW_SDLC_PRI:   return("SDLC PRI WAIT ");
      case SDLLC_AW_NET:        return("NET UP WAIT   ");
      case SDLLC_AW_SDLC_SEC:   return("SDLC SEC WAIT ");
      case SDLLC_CONNECT:       return("CONNECT       ");
      default:                  return("sdllc_bogus   ");
    }
}

/*
 * s d l l c _ i n d _ d i s c o n n e c t
 *
 * sdlc is indicating that it has shut down a PU.
 * Allow the upper layers to do what they need to do.
 */

void sdllc_ind_disconnect (hwidbtype *idb, sdlc_data_block *sdb)
{
    lak_t       *lackie;
    uchar sa[IEEEBYTES];

    if (idb->enctype == ET_SDLCP || idb->enctype == ET_SDLCS) {
        /*
         * Primary or Secondary SDLC interface.  Assume SDLLC.
         */
        if (sdb->sdllc_state >= SDLLC_AW_NET || sdb->sdllc_state == SDLLC_DW_SDLC) {
            if (sdb->sdllc_lak_enable) {
                lackie = sdb2lak(idb, sdb);
                if (lackie)
                    lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_closed);
                sdllc_setstate(idb, sdb, SDLLC_DISCONNECT, "SDLC disconnect (locack)");
                sdb->sdllc_lak_enable = FALSE;
                ieee_copy(idb->sdlc_vr_macaddr, sa);
                sa[5] = sdb->sdlc_address;
                force_rif_flag(sdb->sdlc_prtnr, sa, RIF_AGED);
            } else {
                /*
                 * sdlc side is going down.  If there is an LLC open close it.
                 * When this finishes (can take some time) the sdllc_state
                 * will get properly set.
                 *
                 * If there is no LLC connection simply go right to DISCONNECT.
                 */
                if (sdb->sdllc_llc) {
                    sdllc_setstate(idb, sdb, SDLLC_DW_NET, "SDLC disconnect (net wait)");
                    llc2_close(sdb->sdllc_llc);
                } else
                    sdllc_setstate(idb, sdb, SDLLC_DISCONNECT, "SDLC disconnect");
            }
        } else if (sdb->sdllc_state < SDLLC_DW_NET) {
            /*
             * DW_NET isn't convered by this section.  When in DW_NET
             * there is something actually happening.  Leave it alone.
             *
             * Other states, DISCONNECT, DW_SDLC, and AW_SDLC_PRI just
             * reset back to DISCONNECT.
             */
            sdllc_setstate(idb, sdb, SDLLC_DISCONNECT, "SDLC disconnect (not up)");
        }

        /*
         * Detect closing transition and do appropriate actions.
         * We check explicitly for ET_SDLCP rather than using ISPSDLC because
         * ISPSDLC also will return true if ET_STUN && stun_sdlc_encap is ET_SDLCP.
         * We only want to fire this side up if we are pure SDLC Primary.  This makes
         * the assumption that pure SDLC Primary is only turned on because we are
         * using SDLLC.
         */
        if (idb->enctype == ET_SDLCP && sdb->sdlc_state == SDLC_DISCONNECT &&
              sdb->sdllc_state == SDLLC_DISCONNECT && idb->state == IDBS_UP) {
            /*
             * We are a Primary SDLC link and we are doing SDLLC.  We
             * are in DISCONNECT.  Go right back into SNRMSENT.
             */
            sdllc_setstate(idb, sdb, SDLLC_AW_SDLC_PRI, "SDLC disconnect (coming up)");
            sdlc_setstate(idb, SDLC_SNRMSENT, sdb);
            sdlc_set_pause_timer(idb, sdb);
        }
    }
}


/*
 * s d l l c _ i n d _ c o n n e c t
 *
 * sdlc is indicating that it has gone from a disconnected state
 * to a connected state.  Tell the upper layers and let them do what
 * they need to do.
 */

void sdllc_ind_connect (hwidbtype *idb, sdlc_data_block *sdb)
{
    lak_t *lackie;
    leveltype level;

    if (!sdb)
        return;
    if (sdllc_pid && idb->sdllc_active) {
        /*
         * This interface is running because of SDLLC.
         */
        switch(sdb->sdllc_state) {
          case SDLLC_AW_SDLC_PRI:
            sdllc_setstate(idb, sdb, SDLLC_AW_NET, "SDLC connect (net wait)");
            sdlc_set_clear_busy(idb, sdb->sdlc_address, TRUE);
            sdllc_initiate_pu(idb, sdb);
            break;

          case SDLLC_AW_SDLC_SEC:
            sdllc_setstate(idb, sdb, SDLLC_CONNECT, "SDLC connect (secondary)");
            if (sdb->sdllc_lak_enable) {
	    lackie = sdb2lak(idb, sdb);
	    if (lackie)
		lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_txqlow);
            } else {
                if (sdb->sdllc_llc) {
                    level = raise_interrupt_level(NETS_DISABLE);
                    llc2_stateswitch(sdb->sdllc_llc, CLEAR_LOCAL_BUSY);
                    reset_interrupt_level(level);
                } else
		    errmsg(&msgsym(NOPOINTER, SDLC));
            }
	    sdb->sdllc_local_busy = FALSE;
            break;

          case SDLLC_DISCONNECT:
          case SDLLC_DW_NET:
          case SDLLC_DW_SDLC:
          case SDLLC_AW_NET:
          case SDLLC_CONNECT:
          default:
            if (sdllc_debug)
                buginf("\nSDLLC_ERR: sdlc_ind_connect in sdllc state %s: %s %02x",
                       sdllc_prstate(sdb, 0), idb->hw_namestring, sdb->sdlc_address);
            sdllc_setstate(idb, sdb, SDLLC_DISCONNECT, "SDLC connect (illegal state)");
            sdlc_setstate(idb, SDLC_DISCONNECT, sdb);
            break;
        }
    }
}

