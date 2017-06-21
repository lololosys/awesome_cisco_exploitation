/* $Id: lack.c,v 3.10.8.7 1996/08/26 15:12:11 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/lack.c,v $
 *------------------------------------------------------------------
 * lack.c -- RSRB Local Acknowledgement Support 
 *
 * October 4 1991, Percy P. Khabardar 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lack.c,v $
 * Revision 3.10.8.7  1996/08/26  15:12:11  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.10.8.6  1996/08/07  09:02:24  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.10.8.5  1996/07/23  18:48:50  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.10.8.4  1996/07/09  06:05:16  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.10.8.3  1996/06/27  09:40:24  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.10.8.2  1996/05/17  12:12:43  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.2.5  1996/04/26  15:23:04  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.2.4  1996/04/26  07:58:03  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.8.2.3  1996/04/03  21:58:40  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.8.2.2  1996/03/17  18:49:32  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.8.2.1  1996/03/01  16:31:25  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.10.8.1  1996/03/18  22:10:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.4  1996/03/16  07:37:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.3  1996/03/13  02:01:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.2  1996/03/07  10:50:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  18:46:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/03/01  00:49:19  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up correctly
 * Restore CSCdi47275 w/fix to netbios_as.c
 *
 * Revision 3.9  1996/02/28  09:34:00  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.8  1996/02/16  18:09:37  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Backout CSCdi47275 temporarily-- it breaks igs-c-l build
 *
 * Revision 3.7  1996/02/15  01:38:24  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Fix cleanup and change appl. cleanup from registry to vector
 *
 * Revision 3.6  1996/02/13  08:16:17  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.5  1996/01/22  07:16:17  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/11/21  19:35:03  sberl
 * CSCdi42181:  More than 1000 LLC2 sessions causes excessive tracebacks
 * Make llc2_starttimer() a real function instead of an inline. It was way
 * too big. No longer need the external declarations in lack.c.
 *
 * Revision 3.3  1995/11/17  18:54:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:41:50  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:31:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/10/03  07:19:49  kmoberg
 * CSCdi37823:  Changing bridge protocol with DLSW enabled causes crash.
 * dlsw bridge-group command broken.
 *
 * Revision 2.10  1995/09/15  05:53:49  ppearce
 * CSCdi39706:  Bad p_enqueue message output when using RSRB/TCP local-ack
 *   Missed condition when LLS_NORMAL but unexpected NS received
 *
 * Revision 2.9  1995/09/05  03:03:23  ppearce
 * CSCdi39706:  Bad p_enqueue message output when using RSRB/TCP local-ack
 *   LAK shouldn't vr_nukepak() if already nuke'd by LLC
 *
 * Revision 2.8  1995/08/25  11:45:12  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.7  1995/08/08 16:49:10  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.6  1995/08/07 05:43:36  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *              move dlsw_work to its own file, create libibm_util.a for
 *              utility routines used by dlsw and rsrb
 *
 * Revision 2.5  1995/07/13  20:37:59  jacobt
 * CSCdi36703:  replace malloc() by malloc_named()
 *
 * Revision 2.4  1995/06/21  03:29:33  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.3  1995/06/20  07:18:58  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:21:22  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:00:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "interface_private.h" /* -> _generic when subblock work done */
#include "packet.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "config.h"

#include "rsrb.h"
#include "srb_multiring.h"
#include "srb_sb_private.h"
#include "../if/network.h"
#include "../if/arp.h"
#include "../if/if_vidb.h"
#include "../if/rif_inline.h"
#include "../srt/srt_debug.h"
#include "../cls/clsi_msg.h"
#include "../cls/dlc_public.h"
#include "../cls/dlc.h"
#include "../llc2/llc2.h"
#include "../llc2/llc2_sb.h"
#include "../llc2/llc2_inline.h"
#include "../ip/ip_registry.h"
#include "../srt/srt_registry.h"

#include "../srt/lack_externs.h"  /* addd from 9.1(8) port */
#include "../srt/lack2fsm.h"      /* addd from 9.1(8) port */
#include "../srt/lack2_externs.h" /* addd from 9.1(8) port */
#include "../llc2/lltimers.h"

/*
 * Global Storage
 */
queuetype  lack_linkupQ;
boolean    lack_running = FALSE;
static int lack_linkup_pid;
static int lack_back_pid;
ulong      lack_connect_timeout;         
lak_t      *lack_hash_freelist;
lak_t      *lack_timer_queue;
ulong	   lack_hash_freecount;
lak_t      *lack_hash_table[LACK_HASH_SIZE];
int        lack_passthru_rings[LACK_MAX_PASSTHRU_RINGS];


/*
 * Externs
 */
extern ulong vring_tcpqmax;
extern void tr_setencaps(parseinfo *);

extern dlc_handle_t llc2_dlc_handle;

extern cls_lak2_sendData(lak_t *lackie, paktype *pak);

boolean lack_find_passthru_ring (paktype *pak, tr_vgrouptype *vrg)
{
   trrif_t       *tr_rif;
   boolean       found = FALSE;
   int           i;
   tr_ventrytype *local_vre;

   tr_rif = (trrif_t *) pak->datagramstart;
   local_vre = vrfind_rn2entry(RIF_START_RING(tr_rif), RSRB_BN_DONTCARE,
			       (tr_ventrytype *)&vrg->rings.qhead, RSRB_ET_EITHER);
   if ((local_vre) && (local_vre->type & (RSRB_ET_SDLLC |
					  RSRB_ET_QLLC  |
					  RSRB_ET_LOCAL_VIRTUAL |
					  RSRB_ET_REMSDLLC |
					  RSRB_ET_REMQLLC)))
       return(TRUE);

   for (i = 0; i < LACK_MAX_PASSTHRU_RINGS; ++i) {
      if ((RIF_DEST_RING(tr_rif) == lack_passthru_rings[i]) ||
	  (RIF_START_RING(tr_rif) == lack_passthru_rings[i])) {
         found = TRUE;
         break;
      }
   }
   return(found);
}


/*
 * This function checks: 
 * a) if the "peer" is set for local ack and for
 * b) llc type 1 and 2 frames.
 * llc type 1 frames are allowed to passthru.  llc type 2 frames
 * are sent to llc2_local_term for further examination. 
 *
 * Returns LAK2_PASSTHRU  if the caller should pass this thru
 * returns LAK2_XID       if its an XID and special processing is needed
 * returns LAK2_TERMINATE if the intercept should be invoked.
 *
 * The ORDER of evaluation below is quite important.
 */

lak_pass_t lack_passthru (tr_ventrytype *vre, tr_vpeertype *vrp, paktype *pak)
{
    tring_hdr  *trh;
    int         riflen;
    sap_hdr *llc1;
 
    /* local, local and remote sdllc and qllc frames passthru */
    if (vre->type &  (
		      RSRB_ET_SDLLC | 
		      RSRB_ET_QLLC  |
		      RSRB_ET_LOCAL_VIRTUAL |
		      RSRB_ET_REMSDLLC |
		      RSRB_ET_REMQLLC))
	return(LAK2_PASSTHRU);

    /*
     * If packet has PAK_LACK_OUT set, then accept as such and indicate local
     * term
     */
    if (pak->flags & PAK_LACK_OUT)
	return(LAK2_TERMINATE);

    if (vrp->local_ack) {
        trh   = (tring_hdr *) pak->datagramstart;
	riflen = ((trh->saddr[0] & TR_RII) ? (trh->data[0] & RC_LTHMASK) : (0));
	llc1 = (sap_hdr *) (pak->datagramstart + TRING_ENCAPBYTES + riflen);

	/* Lan Network Manager sessions always passthru */
	if ((llc1->dsap == SAP_IBMNM) ||
	    (lack_find_passthru_ring(pak, vrp->vrg))) {
	    return(LAK2_PASSTHRU);
	}

	switch (llc1->control & ~LLC1_P) {
	  case LLC1_UI:
	  case LLC1_TEST:
	      return(LAK2_PASSTHRU);
  	  case LLC1_XID:
	      if ( vrp->vrp_version == RSRB_LACK_BC_VERSION )
		  return(LAK2_PASSTHRU);
	      else
		  return(LAK2_XID);		
	
	  default:
	    return(LAK2_TERMINATE);
	}
    }
    return(LAK2_PASSTHRU);
} 

/*
 * lak_peergroup_framecount
 * Utility routine to count the number of frames on the tcp queue 
 * or sum of all frames on the priority queues.  Used for flow 
 * control of local-ack sessions.
 */
static int lak_peergroup_framecount (tr_vpeertype *rootpeer)
{
    int i=0, tcpq_frame_count=0;

    tcpq_frame_count = reg_invoke_ip_tcpdriver_oqlen(rootpeer->t);

    if (rootpeer->priority) 
	for (i = 1; i <= RSRB_PRIORITY_COUNT; i++) 
	    if (rootpeer->priority_peers[i])
		tcpq_frame_count += reg_invoke_ip_tcpdriver_oqlen(rootpeer->priority_peers[i]->t);

    return(tcpq_frame_count);
}

/*
 * l a k 2 _ l o c a l _ t e r m
 *
 * Handle local termination of a lack entity using Lack2
 * terminates a llc2 virtual circuit at the router.   In other
 * words we are taking input from the MEDIA side.
 *
 * Returns result = TRUE if an llc2 frame goes no further.
 * 		    (we consumed it)
 *         result = FALSE if sent to remote end.
 * 		    (caller must deal with it)
 */

boolean lak2_local_term (paktype *pak, uchar *type, tr_vpeertype *vrp,
			 int local_ring, int rem_ring)
{
    paktype *newpak;
    uchar     *frameptr;
    trrif_t   *trh;
    lak_t     *lackie;
    uchar      conch;
    sap_hdr   *llc1;
    int        riflen;
    uchar      addr[14];
    leveltype  level;
    hwidbtype  *hwidb;
    
    lackie = trmac2lak(pak, addr, TRMAC2LAK_FROM_MEDIA);

    hwidb = hwidb_or_null(pak->if_input);
    if (hwidb != NULL)
    {
	(*hwidb->parse_packet)(hwidb, pak, NULL);
    }
    else
    {
	buginf("\nLOCACK: NULL if_input: frame dropped");
	vr_nukepak(pak);
	return(TRUE);
    }
    
    frameptr = pak->datagramstart + pak->llc2_sapoffset;
    llc1 = (sap_hdr *)(frameptr);
    conch = llc1->control & ~P_BIT; /* remove p/f bit */
    if (lack_debug_packet) {
	buginf("\nLOCACK: recv %s, %s, state %s", lack_prconch(llc1->control),
	       lack_id(lackie), lak2_prstate(lackie, 0));
    }

    /*
     * XID, TEST frames passthru unaltered.
     */
    if (conch == XID) {
	/*
	 * This maybe a special cleanup frame but how can we ever
	 * be here?  lack_passthru always punts XIDs from getting here.
	 * Same is true for TEST.
	 */
	/* This is the special "cleanup" frame */
	if (pak->flags & PAK_LACK_OUT) 
	    *type = RSRB_OP_LINKDOWN_REQ;
	return(FALSE);
    } else if (conch == LLC_TEST) {
	return(FALSE);
    } else if (conch == LLC_SIM) {
	return(FALSE);
    }
    

    switch (lak2_getstate(lackie)) {
      case l2st_undefined:
      case l2st_disconnect:
	/* Drop I/S Frames */
	if ((conch & LLC_IMASK) == LLC_IFRAME ||
	    (conch & LLC_SMASK) == LLC_SUPERVISORY ||
	    conch == UA ){
	    /*
 	     * Passthru I and S Frames since we are disconnected.
 	     * Let endstations handle error conditions or Remote NCP load
 	     * CSCdi31427 Remote NCP load doesn`t work with local ack
 	     * (due to LLC2 SIM frame)
  	     */
	    if (lack_debug_err)
		buginf("\nLOCACK: %s passthru, %s %s",
		       lack_prconch(llc1->control), lack_id(lackie),
		       lak2_prstate(lackie, 0));
	    return(FALSE);
	}
	if (conch == SABME) {
	    /*
	     * SABME and we are disconnected.  Create new lack entity.
	     */
	    lack2_pak2addr(pak, addr, TRMAC2LAK_FROM_MEDIA);
	    lackie = lack_create_entry(addr, vrp, local_ring, rem_ring);
	    if (!lackie) {
		if (lack_debug_err)
		    buginf("\nLOCACK: Couldn't create local connection (R SABME)");
		vr_nukepak(pak);
		return(TRUE);
	    }
	    lackie->dlc_db.type = DLC_LLC2;
	    lackie->dlc_db.ptr.llc = NULL;
	    lackie->llc = NULL;
	    lackie->dlc_handle = &llc2_dlc_handle;
	    newpak = pak_clip(pak, pak->datagramsize);
	    if (!newpak) {
		if (lack_debug_err)
		    buginf("\nLOCACK: No memory for duplicate SABME frame!");
		lack_remove_hash(lackie);
		vr_nukepak(pak);
		return(TRUE);
	    }
	    newpak->flags |= PAK_LACK_IN;
	    newpak->peer_ptr = vrp;
	    if (lack_debug_state)
		buginf("\nLOCACK: (local) created entity for %s",
		       lack_id(lackie));
	    /*
	     * Add this frame's source address to the RIF cache.
	     */
	    trh = (trrif_t *)pak->datagramstart;
	    riflen = (trh->saddr[0] & TR_RII) ? trh->rc_blth & RC_LTHMASK : 0;
	    /* llc local open ; local addr ---- rem addr */
	    rif_update(NULL, 0, trh->saddr, trh->daddr,
		       (srbroute_t *) (riflen ? &trh->rc_blth : NULL), RIF_NOAGE);
	    lackie->rif.len = riflen;
	    if (riflen) {
		bcopy((uchar *) &trh->rc_blth, lackie->rif.data, riflen);
		((srbroute_t *) lackie->rif.data)->rc_dlf ^= RC_D;
	    }
	    reg_invoke_llc2_input(newpak);
	    lackie->piggie_pak = pak;
	    lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_ext_conn);
	    return(TRUE);
	}
	pak->flags |= PAK_LACK_IN;
	reg_invoke_llc2_input(pak);		/* consume it */
	return(TRUE);

      case l2st_localdiscwait:
      case l2st_remdiscwait:
	/* Drop I/S Frames and SABMEs */
	if ((conch & LLC_IMASK) == LLC_IFRAME || (conch & LLC_SMASK) == LLC_SUPERVISORY
	      || conch == SABME) {
	    /*
	     * Drop I and S Frames since we are disconnected.
	     */
	    if (lack_debug_err)
		buginf("\nLOCACK: %s frame dropped, %s %s",
		       lack_prconch(llc1->control),
		       lack_id(lackie), lak2_prstate(lackie, 0));
	    vr_nukepak(pak);
	    return(TRUE);
	}
	pak->flags |= PAK_LACK_IN;
	reg_invoke_llc2_input(pak);		/* consume it */
	return(TRUE);

      case l2st_remwait:
	/* Drop I Frames */
	if ((conch & LLC_IMASK) == LLC_IFRAME) {
	    if (lack_debug_err)
		buginf("\nLOCACK: %s frame dropped, %s %s",
		       lack_prconch(llc1->control), lack_id(lackie),
		       lak2_prstate(lackie, 0));
	    vr_nukepak(pak);
	    return(TRUE);
	}
	if ((conch & LLC_SMASK) == LLC_SUPERVISORY) {
	    pak->flags |= PAK_LACK_IN;
	    reg_invoke_llc2_input(pak);		/* consume it */
	    return(TRUE);
	}
	switch (conch) {
	  case DISC:
  	  case DM:
   	  case FRMR:
  	  case SABME:
	    /*
	     * An abort condition.
	     *
	     * We may want to relearn the rif hear incase it is a
	     * SABME.
	     */
	    pak->flags |= PAK_LACK_IN;
	    reg_invoke_llc2_input(pak);	     /* consumes newpak */
#ifdef	handled_by_fsm_machine
	    if (lackie->llc)
		lackie->llc->l2_lakptr = NULL;
	    lackie->piggie_pak = pak;
	    lak2_fsm(lackie, l2ma_dlc,
		     (conch == SABME) ? l2mi_dlc_ind_ext_conn : l2mi_dlc_ind_failure);
#endif
	    return(TRUE);

	  default:
	    pak->flags |= PAK_LACK_IN;
	    reg_invoke_llc2_input(pak);		/* consume it */
	    return(TRUE);
	}
	if (lack_debug_err)
	    buginf("\nLOCACK: %s frame dropped, %s %s", lack_prconch(llc1->control),
		   lack_id(lackie), lack_prstate(lackie, 0));
	vr_nukepak(pak);
	return(TRUE);

      case l2st_localwait:
	if ((conch & LLC_IMASK) == LLC_IFRAME) {
	    if (lack_debug_err) {
		trh = (trrif_t *)pak->datagramstart;
		frameptr = pak->datagramstart + TRING_ENCAPBYTES;
		buginf("\nLOCACK: %s frame dropped, %s %s",
		       lack_prconch(llc1->control), lack_id(lackie),
		       lak2_prstate(lackie, 0));
		buginf("\n  %e %e %08x %08x %08x %08x %08x %08x %08x %08x",
		       trh->daddr, trh->saddr,  ((ulong *) frameptr)[0], ((ulong *) frameptr)[1],
		       ((ulong *) frameptr)[2], ((ulong *) frameptr)[3], ((ulong *) frameptr)[4],
		       ((ulong *) frameptr)[5], ((ulong *) frameptr)[6], ((ulong *) frameptr)[7]);
	    }
	    vr_nukepak(pak);
	    return(TRUE);
	}
	switch (conch) {
	  case DISC:
	  case DM:
	  case FRMR:
	    /*
	     * Ugh.  We are still waiting for llc2_open to return, but not there
	     * yet.
	     */
	    if (lack_debug_state)
		buginf("\nLOCACK: abort request %s from local, %s",
		       lack_prconch(llc1->control), lack_id(lackie));
	    lackie->llc2_open_ok = FALSE;
	    if (lackie->llc)
		((llctype *) lackie->llc)->lack_llc2_open_ok = FALSE;
	    if (lackie->llc2_open_pak)
		lackie->llc2_open_pak->flags &= ~(PAK_LACK_OUT);
	    pak->flags |= PAK_LACK_IN;
	    reg_invoke_llc2_input(pak);		/* consume it */
	    lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_failure);
	    return(TRUE);

	  default:
	    break;
	}
	pak->flags |= PAK_LACK_IN;
	reg_invoke_llc2_input(pak);		/* process and consume it */
	return(TRUE);

      case l2st_connectpending:
      case l2st_connect:
      case l2st_remqonwait:
      case l2st_remqoffwait:
      case l2st_quench:
 	/*
 	 * rnr_disable feature requires that we
 	 * drop the first I-frame before giving to LLC2.
 	 * This is so the partner side LLC2 could retry.
 	 * If we ack and buffer and send it to the other router
 	 * it will drop it anyway. The end-station has to do an
 	 * application level retry instead of LLC2 level retry.
 	 */
	if ((conch & LLC_IMASK) == LLC_IFRAME) {
	    /*
	     * LLC2 input will set PAK_LACK_OUT if frame is good to forward
	     */
 	    if (lak2_getstate(lackie) == l2st_connectpending) {
 		if (lack_debug_err)
 		    buginf("\nLOCACK: %s frame dropped, %s %s", 
 			   lack_prconch(llc1->control),
   			   lack_id(lackie), lak2_prstate(lackie, 0));
   		vr_nukepak(pak);
	    }
	    else {
		pak->flags |= PAK_LACK_IN;
		reg_invoke_llc2_input(pak);		/* llc2 doesn't consume I frames */
		if (pak->flags & PAK_LACK_OUT) {
		    if (lack_debug_packet)
			buginf("\nLOCACK: %s frame sent, %s %s",
			       lack_prconch(llc1->control),
			       lack_id(lackie),
			       lak2_prstate(lackie, 0));
		    lackie->piggie_pak = pak;
		    lak2_send_netmsg(lackie, RSRB_OP_LLC2DATA);
		} else {
 		    if (lack_debug_state && lackie->local_busy == FALSE)
 			buginf("\nLOCACK: %s frame dropped, %s %s", 
			       lack_prconch(llc1->control),
			       lack_id(lackie),
			       lak2_prstate(lackie, 0));
                    /*
                     * Usually, llc2_input() doesn't discard I-frames
                     *   and LAK will discard the I-frame here
                     *
                     * However, there always seems to be an exception
                     *
                     *   llc2_input() _will_ discard the I-frame
                     *     if the local LLC state is:
                     *       LLS_BUSY
                     *       LLS_AWAIT_BUSY
                     *       LLS_REJECT
                     *       LLS_AWAIT_REJ
                     *       ...or...
                     *     if the local LLC state is LLS_NORMAL
                     *       but unexpected NS count was received
                     *
                     *   LAK should not discard the pak 
                     *     because it has already been nuked
                     *
                     */
                    switch(lackie->llc->state) {
                        case LLS_NORMAL:
                          if (!lackie->llc->unexp_n_s) {
                            vr_nukepak(pak);
                          }
                          break;
                        case LLS_BUSY:
                        case LLS_AWAIT_BUSY:
                        case LLS_REJECT:
                        case LLS_AWAIT_REJ:
                          break;
                        default:
                          vr_nukepak(pak);
                          break;
                    }
		} /* End else - PAK_LACK_OUT not set in pak */
	    }
 	    if ((100 * lak_peergroup_framecount(lackie->vrp) / vring_tcpqmax) > LACK_TCPQ_THRESHOLD_HIGH) {
		level = raise_interrupt_level(NETS_DISABLE);
		llc2_stateswitch(lackie->llc, SET_LOCAL_BUSY);
		reset_interrupt_level(level);
		lackie->local_busy = TRUE;
	    }
	    return(TRUE);
	}

	if ((conch & LLC_SMASK) == LLC_SUPERVISORY) {
	    if ( lackie->local_busy )  {
 		if ((100 * lak_peergroup_framecount(lackie->vrp) / vring_tcpqmax) < 
  		    LACK_TCPQ_THRESHOLD_LOW) {		
		    level = raise_interrupt_level(NETS_DISABLE);
		    llc2_stateswitch(lackie->llc, CLEAR_LOCAL_BUSY);
		    reset_interrupt_level(level);
		    lackie->local_busy = FALSE;
		}
	    }
        }
	  pak->flags |= PAK_LACK_IN;
   	  reg_invoke_llc2_input(pak);		/* consume it */
	  return(TRUE);
	switch(conch) {
  	  case DISC:
	  case DM:
	  case FRMR:
	  case SABME:
	    if (lack_debug_state)
		buginf("\nLOCACK: local %s, %s %s",
		       lack_prconch(llc1->control),
		       lak2_prstate(lackie, 0), lack_id(lackie));
#ifdef handled_by_fsm_machine
	    if (lackie->llc)
		lackie->llc->l2_lakptr = NULL;
#endif
	    pak->flags |= PAK_LACK_IN;
	    reg_invoke_llc2_input(pak);		/* consume pak */
	    return(TRUE);

  	  default:
	    pak->flags |= PAK_LACK_IN;
	    reg_invoke_llc2_input(pak);		/* consume it */
	    return(TRUE);
	}
	break;

      default:
	buginf("\nLOCACK: illegal lack state %d, %s",
	       lackie->lak2_state, lack_id(lackie));
	vr_nukepak(pak);
	return(TRUE);
    }
    if (lack_debug_err)
	buginf("\nLOCACK: %s frame dropped, %s %s", lack_prconch(llc1->control),
	       lack_id(lackie), lack_prstate(lackie, 0));
    vr_nukepak(pak);
    return(TRUE);
}

void lack2_rsdllc_initiate_rem_conn (
    paktype *pak,
    sdlc_data_block *sdb,
    tr_ventrytype *vre)
{
    lak_t	    *lackie;

    pak_lock(pak);
    if (!(lackie = lack2_build_reverse_entry(pak, vre, sdb, DLC_SDLC)) )
	return;
	
    lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_ext_conn);
    return;
}

/*
 * lack2_build_reverse_entry
 *
 * common routine to establish lackie for reverse startup 
 * (currently used in RSDLLC and RQLLC)
 *
 * Validates the the request
 *
 * makes sure that peer exists, that lackie not already there, and
 * 	rif makes sense
 *
 * Then create the lackie and fills in critical fields.
 * This is the counterpart to lak2_process_netmsg as far as creating the 
 * lackie is concerned.
 * Although both SDLLC and QLLC then go on to call
 *	 lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_ext_conn),
 * that call doesn't belong in this routine.
 */

lak_t *lack2_build_reverse_entry
	(paktype *pak, tr_ventrytype *vre, 
	 void *control_block, dlc_t dlc_type) 
{
    trrif_t   	*trh;
    lak_t	*lackie;
    ushort    	rem_rn, rem_bn;
    char      	*why;
    uchar      	addr[14];
    int        	riflen;
    int	     	offset;

    /* 
     * Test for valid peer 
     */
    if (!(pak->peer_ptr)) {
	if (lack_debug_err)
	    buginf("\nLOCACK: no peer for this XID msg");
	return(NULL);
    }
    trh = (trrif_t *)pak->datagramstart;
    riflen = (trh->rc_blth & RC_LTHMASK);
    if (!riflen) {
	if (lack_debug_err)
	    buginf("\nLOCACK: no rif on XID msg");
	return(NULL);
    }

    /* Make the D bit in the rif correct for reverse qllc */
    if (dlc_type == DLC_QLLC)
      trh->rc_dlf ^= RC_D;  /* toggle direction bit */ 


    /* 
     * make sure that lackie doesn't exist yet 
     */
    lackie = trmac2lak(pak, addr, TRMAC2LAK_FROM_NET);
    if (lackie) {
	if (lack_debug_err)
	    /*
	     * should never happen
	     */
	    buginf("\nLOCACK: lackie already exists for this long XID pak");
	return(NULL);
    }

    /*
     * scan the rif 
     *
     * This packet is coming in from the network heading towards the media.
     * the rif searcher is built to scan rifs coming in from the media going
     * towards other media or the network.  So we have to futz the RIF to
     * make it appear like it is going out the network side.
     */
    offset = rif_match_offset((srbroute_t *) &trh->rc_blth, vre->rn, vre->bn,
		((tr_vpeertype *) pak->peer_ptr)->vrg->vrn, &rem_rn, 
		 &rem_bn, &why);
    if (!offset) {
	if (lack_debug_err) {
	    buginf("\nLOCACK: couldn't find remote ring on XID msg, %s", why);
	buginf("\n\t ring no = %d, bridge no = %d, rem rn = %d, rem bn = %d", 
	       vre->rn, vre->bn, rem_rn, rem_bn);
	}
    return(NULL);
    }

    /*
     * Validation completed.
     *
     * Create the lackie
     */
    lack2_pak2addr(pak, addr, TRMAC2LAK_FROM_NET);
    lackie = lack_create_entry(addr, pak->peer_ptr, vre->rn, rem_rn);
    if (!lackie) {
	if (lack_debug_err)
	    buginf("\nLOCACK: No memory for XID msg lackie!");
	return(NULL);
    }
    if (lack_debug_state)
	buginf("\nLOCACK: (remote) created entity for %s",
	       lack_id(lackie));
    /*
     * copy in the rif too (if it exists).
     */
    lackie->rif.len = riflen;
    bcopy((uchar *) &trh->rc_blth, lackie->rif.data, riflen);

    /* 
     * locaddr rem open ; local addr ---- rem addr 
     */
    rif_update(NULL, 0, trh->saddr, trh->daddr, 
	       (srbroute_t *) &trh->rc_blth, RIF_NOAGE);

    lackie->dlc_db.type = dlc_type;
    lackie->dlc_db.ptr.control_block = control_block;
    
 	    lackie->llc = NULL;
    lackie->dlc_handle = reg_invoke_lack_get_dlc_handle(dlc_type);
    lackie->piggie_pak = pak;

    return(lackie);
}

static forktype lack_background (void)
{
    lak_t     *lackie, *next;
    leveltype level;

    edisms(systeminitBLOCK, 0);
    while (TRUE) {
	level = raise_interrupt_level(NETS_DISABLE);
	/*
	 * Replenish lack entry buffers.  Entries are used to store lack state
	 */
        while (lack_hash_freecount < LACK_HASHQ_SIZE) {
	    lackie = malloc(sizeof(lak_t));
            if (lackie == NULL)
		break;
	    lackie->next = lack_hash_freelist;
	    lack_hash_freelist = lackie;
	    lack_hash_freecount++; 
	}
	/*
	 * Clean up any connections which are in process of coming up or down
	 */
	for (lackie = lack_timer_queue; lackie != NULL; lackie = next) {
	    /*
	     * lackie may get deleted in loop. Learn 'next' now...
	     */
	    next = lackie->timer_queue_next;
	    if (AWAKE(lackie->timer_start)) {
		lak2_StopAckTimer(lackie);
		if ((--lackie->retry_count) > 0)
		    lak2_fsm(lackie, l2ma_timer, l2mi_timer_expired);
		else 
		    lak2_fsm(lackie, l2ma_timer, l2mi_timer_abort);
	    }
	}			/* endFor lackie loop */

	reset_interrupt_level(level);
	process_sleep_for(ONESEC * 30);
    }
}

/*
 * lack_start
 * Get lack process running.  Called from configurator. 
 */
void lack_start (void)
{
    int       i;
    lak_t     *lackie = NULL;
    leveltype status;

    if (lack_running)
	return;

    
    if (!lack_linkup_pid) {
	lack_linkup_pid = cfork(lack_linkup_proc, 0L, 0, "LOCACK LinkUp", 0);
	if (lack_linkup_pid == NO_PROCESS) {
	    lack_linkup_pid = 0;
	    return;
	}
    }
    if (!lack_back_pid) {
	lack_back_pid = cfork(lack_background, 0L, 0, "LOCACK Backgnd", 0);
	if (lack_back_pid == NO_PROCESS) {
	    lack_back_pid = 0;
	    return;
	}
    }

    reg_add_media_llc_create(lack_fix_llc, "lack_fix_llc");

    status = raise_interrupt_level(NETS_DISABLE);

    lack_timer_queue    = NULL;
    lack_hash_freelist = NULL;
    
    for (i = 0; i < LACK_HASH_SIZE; i++)
	lack_hash_table[i] = (lak_t *)NULL;

    for (i = 0; i < LACK_MAX_PASSTHRU_RINGS; i++)
        lack_passthru_rings[i] = 0;

    for (i = 0; i < LACK_HASHQ_SIZE; i++) {
	lackie = malloc_named(sizeof(lak_t), "LACK CB");
	if (!lackie) {
	    reset_interrupt_level(status);
	    return; 
	}
	lackie->next = lack_hash_freelist;
	lack_hash_freelist = lackie;
    }
    lack_hash_freecount = LACK_HASHQ_SIZE;

    reset_interrupt_level(status);

    lack_running = TRUE;
}



/*
 * lack_enter_passthru_ring
 */
void lack_enter_passthru_ring (int ring_num)
{
   int i;

   for (i = 0; i < LACK_MAX_PASSTHRU_RINGS; i++) {
      if (lack_passthru_rings[i] == 0) {
         lack_passthru_rings[i] = ring_num;
         break;
      }
   }

   if (i == LACK_MAX_PASSTHRU_RINGS)
      printf("\nno entry available for passthru ring %d", ring_num);
}
    

/*
 * lack_delete_passthru_ring
 */
void lack_delete_passthru_ring (int ring_num)
{
   int i;

   for (i = 0; i < LACK_MAX_PASSTHRU_RINGS; i++) {
      if (lack_passthru_rings[i] == ring_num) {
         lack_passthru_rings[i] = 0;
         break;
      }
   }

   if (i == LACK_MAX_PASSTHRU_RINGS)
      printf("\nring %d not previously configured for passthru", ring_num);
}


/*
 * linkupBLOCK
 * Scheduler test for the link up process.  If packet present on 
 * linkupQ, unblock linkup process else it remains blocked.
 */
static boolean linkupBLOCK (void)
{

   if (lack_linkupQ.qhead != NULL)
      return(FALSE);
   return(TRUE);
}


forktype lack_linkup_proc (void)
{
    paktype   *pak;
    llctype   *llc2;
    lak_t     *lackie;
    leveltype  level;
    uchar      addr[14];

    edisms(systeminitBLOCK, 0);

    for (;;) {
	  edisms((blockproc *)linkupBLOCK, 0);
	  while ((pak = pak_dequeue(&lack_linkupQ)) != NULL) {
	    lackie = trmac2lak(pak, addr, TRMAC2LAK_FROM_NET);
	    if (!(pak->flags & PAK_LACK_OUT) || !lackie ||
		  !lackie->llc || !lackie->llc2_open_ok) {
		datagram_done(pak);
		continue;
	    }
	    if (lak2_getstate(lackie) == l2st_localwait) {
		/*
		 * llc2_open2 blocks.  While we were blocking something could have
		 * changed and destroyed us.
		 */
		llc2 = llc2_open2(lackie->llc, TRUE);
		level = raise_interrupt_level(NETS_DISABLE);
		if ((lackie->llc2_open_ok == FALSE) ||
		    (llc2 && (llc2->lack_llc2_open_ok == FALSE))) {
		    /*
		     * make sure that there are no fsm out calls from llc2_close.
		     */
		    if (lackie->llc)
			lackie->llc->l2_lakptr = NULL;
		    llc2_close(llc2);
		    lackie->llc = llc2 = NULL;
		}
		if (llc2) {
		    lackie->llc = llc2;
		    llc2->l2_lakptr = lackie;
                    llc2->media_llc_cleanup = lack_cleanup_llc;
		    lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_opened);
		} else {
		    if (lack_debug_err)
			buginf("\nLAK2: llc2 open unsuccessful, %s", lack_id(lackie));
		    lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_failure);
		}
		reset_interrupt_level(level);
	    }
	    datagram_done(pak);
	    continue;

	}
    }
}


/* 
 * lack_data has been received from TCP for a local ack RSRB 
 * session.  Remote could be SDLLC or LLC2.
 */
void lack_data (paktype *pak, int length, tr_ventrytype *vre)
{
    lak_t *lackie;
    leveltype  level;
    uchar addr[14];		/* Holds old lackie's address */
    int state;
    dlc_oqueue_status_t llc_status;

    lackie = trmac2lak(pak, addr, TRMAC2LAK_FROM_NET);

    state = lak2_getstate(lackie);
    if (state >= l2st_connect) {
	if (lack_debug_packet)
	    buginf("\nLOCACK: recv LLC2DATA, %s", lack_id(lackie));
	pak->flags |= PAK_LACK_OUT;
	level = raise_interrupt_level(NETS_DISABLE);

	switch(lackie->dlc_db.type) {
	  case DLC_LLC2:

            /*
             * if we're in connected state
             *   (i.e. haven't sent a flow control message) then
             *   check queue length to flow control remote peer via
             *   RSRB_OP_QUENCH_ON message
             */
            llc_status = llc_get_oqueue_status(lackie->llc);
            if ((llc_status > DLC_OQUEUE_NORMAL) &&
                ((state == l2st_connect) || (state == l2st_remqoffwait))) {
                /* signal we've reached high mark */
                lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_txqhigh);
            }

	    if (llc_status != DLC_OQUEUE_FULL) {
		llc2_send(lackie->llc, pak, length);
	    } else {
		vr_nukepak(pak);
		buginf("\nLOCACK: llc2 send failed, check llc txQ and buffers");
	    }
	    break; 
          case DLC_CLS:
	    cls_lak2_sendData(lackie, pak);
	    break;
	  default:
	    buginf("\nLOCACK: lack_data, wrong dlc_db type");
	    vr_nukepak(pak);
	    break;
	}
	reset_interrupt_level(level); 
    } else {
	if (lack_debug_packet || lack_debug_state)
            buginf("\nLOCACK: data while not connected, data discarded");
	vr_nukepak(pak);
    }
}



/*
 * l a c k _ c r e a t e _ e n t r y
 *
 * Create a new hashtable entry, filling in all needed parameters,
 * and then placing it into the hash table
 */

lak_t *lack_create_entry (uchar *addr, tr_vpeertype *vrp, int local_ring, int rem_ring)
{
    lak_t	*lackie;
    leveltype	level;
    int		n;

    /*
     * This protects against on the fly deconfig of vrg/vrp
     * check for poison or null pointer
     */
    if ( !vrp || ((ulong) vrp & 0xffff0000 ) == 0x0d0d0000) {
	buginf("\nLOCACK: Bad vrp pointer to lack create entry %08x", vrp);
	return(NULL);
    }
    level = raise_interrupt_level(NETS_DISABLE);
    if (lack_hash_freelist != NULL) {
	lackie = lack_hash_freelist;
	lack_hash_freelist = lack_hash_freelist->next;
	lack_hash_freecount--;
    } else {
	if (lack_debug_err)
	    buginf("\nLOCACK: No free Lack structure available");
	reset_interrupt_level(level);
	return(NULL);
    }
    lackie->lak2_state	     = l2st_disconnect;
    lackie->llc              = NULL;
    lackie->local_busy       = FALSE;
    lackie->next             = NULL;
    lackie->vrp		     = vrp;
    lackie->local_ring	     = local_ring;
    lackie->remote_ring	     = rem_ring;
    lackie->lak2_fsm_level   = 0;
    lackie->fsm_ptr = (vrp->vrp_version == RSRB_LACK_BC_VERSION) ? lak2bc_fsm_table :
	lak2_fsm_table;
    TIMER_STOP(lackie->timer_start);
    lackie->llc2_open_ok     = FALSE;
    lackie->llc2_open_pak    = NULL;
    lackie->piggie_pak       = NULL;
    lackie->timer_queue_next = NULL;
    lackie->rif.len	     = 0;
    bcopy(addr, lackie->address, 14);
    n = calc_lack_hash(lackie->address);
    lackie->next = lack_hash_table[n];
    lack_hash_table[n] = lackie;
    p_enqueue(&vrp->lackie, lackie);
    reset_interrupt_level(level);
    return(lackie);
}


/*
 * lack_addr_in_use
 *
 * Return TRUE if DA, SA tuple is still being referenced by any
 * LACK entities.
 */

boolean lack_addr_in_use (uchar *addr)
{
    int n;
    lak_t *lackie;

    for (n = 0; n < LACK_HASH_SIZE; n++) {
	lackie = lack_hash_table[n];
	while (lackie) {
	    if (bcmp(lackie->address, addr, 12) == 0)
	      return(TRUE);
	    lackie = lackie->next;
	}
    }
    return(FALSE);
}

lak_t *lack_by_machdr (uchar *addr)
{
   int       n;
   lak_t *index;
   leveltype level;

   if (!addr)
       return(NULL);
   n = calc_lack_hash(addr);
   level = raise_interrupt_level(NETS_DISABLE);
   index = lack_hash_table[n];
   while (index) {
      if (bcmp(index->address, addr, 14) == 0) {
         reset_interrupt_level(level);
         return(index);
      }
      index = index->next;
   }
   reset_interrupt_level(level);
   return(NULL);
}


/*
 * l a c k _ c l e a n u p _ l l c
 *
 * Callback routine to close a session because the LLC code told us to
 */

void lack_cleanup_llc (llctype *llc)
{
    lak_t *lackie;
    leveltype level;

    if (llc == NULL) {
	if (lack_debug_state)
	    buginf("\nLOCACK: lack_cleanup_llc:  null llc pointer");
	return;
    }

    lackie = llc->l2_lakptr;

    if (lackie == NULL) {
        if (lack_debug_state)
	    buginf("\nLOCACK: lack_cleanup_llc:  llc->l2_lakptr is null");
	return;               
    }

    if (lack_debug_state) {
	buginf("\nLOCACK: llc cleanup for %s, %s",
	       lack_id(lackie), lak2_prstate(lackie, 0));
    }
    level = raise_interrupt_level(NETS_DISABLE);
    switch (llc->adm_reason) {
      case LC_DM:	/* rx'd DM from local media   */
      case LC_DISC:	/* rx'd DISC from local media */
      case LC_DISC_LAN:/* rx'd DISC from local media */
      case LC_DISC_CNF:
      case LC_RESET:	/* rx'd SABME in d_conn state */
	lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_closed);
	break;

      case LC_TIMEOUT:		/* n2 expired, no response to cmd frame */
      case LC_INTDOWN:		/* interface went down */
      case LC_FRMR:
	lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_failure);
	break;

      case LC_ABORTED:
	break;

      default:
	if (lack_debug_err)
	    buginf("\nLOCACK: Invalid adm reason [%s (%d)] for %s in %s",
		   llc2_prreason(llc->adm_reason), llc->adm_reason,
		   lack_id(lackie), lak2_prstate(lackie, 0));
	lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_failure);
	break;
    }
    lackie->llc = lackie->dlc_db.ptr.llc = NULL;
    reset_interrupt_level(level);
}

/*
 * l a c k _ f i x _ l l c
 * call out routine from LLC when a new external connection
 * has been accepted.
 */

void lack_fix_llc (llctype *llc, paktype **pak)
{
   lak_t     *lackie;
    uchar addr[14];	/* Holds old lackie's address */   

   lackie = trmac2lak(*pak, addr, TRMAC2LAK_FROM_MEDIA);
   if (lackie) {
       lackie->llc = llc;
       lackie->dlc_db.type = DLC_LLC2;
       lackie->dlc_db.ptr.llc = llc;
       llc->l2_lakptr = lackie;
       llc->media_llc_cleanup = lack_cleanup_llc;
       return;
   }
   return;
}


void lack_remove_hash (lak_t *target)
{
   int n;
   lak_t *index, **prev;
   leveltype level;

   if ( checkqueue(&(target->vrp->lackie), target) )
	p_unqueue(&(target->vrp->lackie), target);
   n = calc_lack_hash(target->address);
   level = raise_interrupt_level(NETS_DISABLE);
   prev = &lack_hash_table[n];
   index = *prev;

   while (index) {
      if (index == target) {
         *prev = index->next;
         index->next = lack_hash_freelist;
	 if (index->llc)
	     index->llc->l2_lakptr = NULL;
	 lack_hash_freelist = index;
	 lack_hash_freecount++;
         reset_interrupt_level(level);
         return;
      }
      prev = &index->next;
      index = index->next;
   }
   reset_interrupt_level(level);
}

int calc_lack_hash (uchar *string)
{
   int i, n = 0;

   for (i = 0; i < 14; i++)
      n += *(string + i);
   n &= LACK_HASH_MASK;
   return (n);
}


char *lack_prconch (uint cbyte)
{
    if ((cbyte & LLC_IMASK) == LLC_IFRAME)
	return("IFRAME");
    if ((cbyte & LLC_SMASK) == LLC_SUPERVISORY)
	cbyte &= (P_BIT | 0xf);
    switch (cbyte) {
      case UI:			return("UI");
      case LLC_SIM:		return("RIM/SIM");
      case DM:			return("DM");
      case LLC_UP:		return("UP");
      case DISC:		return("DISC/RD");
      case UA:			return("UA");
      case SABME:		return("SABME");
      case SNRM:		return("SNRM");
      case FRMR:		return("FRMR");
      case XID:			return("XID");
      case CFGR:		return("CFGR");
      case SNRME:		return("SNRME");
      case LLC_TEST:		return("TEST");
      case BCN:			return("BCN");
      case RR:			return("RR");
      case RNR:			return("RNR");
      case REJ:			return("REJ");

      case (UI    | P_BIT):	return("UI P/F");
      case (LLC_SIM | P_BIT):	return("RIM/SIM P/F");
      case (DM    | P_BIT):	return("DM P/F");
      case (LLC_UP| P_BIT):	return("UP P/F");
      case (DISC  | P_BIT):	return("DISC/RD P/F");
      case (UA    | P_BIT):	return("UA P/F");
      case (SABME | P_BIT):	return("SABME P/F");
      case (SNRM  | P_BIT):	return("SNRM P/F");
      case (FRMR  | P_BIT):	return("FRMR P/F");
      case (XID   | P_BIT):	return("XID P/F");
      case (CFGR  | P_BIT):	return("CFGR P/F");
      case (SNRME | P_BIT):	return("SNRME P/F");
      case (LLC_TEST| P_BIT):	return("TEST P/F");
      case (BCN   | P_BIT):	return("BCN P/F");
      case (RR    | P_BIT):	return("RR P/F");
      case (RNR   | P_BIT):	return("RNR P/F");
      case (REJ   | P_BIT):	return("REJ P/F");

      default:			return("unknown");
    }
}


/*
 * show_lack
 * Show information about LACK virtual circuits.
 * local addr      remote addr     lrg  rrg  lsap  dsap   state
 * 4000.3745.1000  4000.1000.2000  001  004  04    E4     REM DISC WAIT
 */

#define NLACK_LINES   3
static const char show_lack_hdr[] = 
            "\nlocal addr      remote addr     lrg  rrg  lsap  dsap   state\n";

void show_lack (void)
{
    int        n;
    lak_t      *lackie;
    boolean    first = TRUE;
    int        i;
    int        line_break = 0; 

    automore_enable(NULL);
    for ( i = 0; i < LACK_MAX_PASSTHRU_RINGS; ++i) {
	if ( lack_passthru_rings[i] ) {
	    if (first == TRUE) {
		printf("\n\nPassthrough Rings: ");
		first = FALSE;
	    }
	    printf( "%d ", lack_passthru_rings[i] );
	    ++line_break;
	    if (line_break == 5) {
		printf("\n");
		line_break = 0;
	    }
	}
    }
    printf(show_lack_hdr);
    for (n = 0; n < LACK_HASH_SIZE; n++) {
	lackie = lack_hash_table[n];
	while (lackie) {
	    automore_conditional(NLACK_LINES);
	    printf("%e  %e  %03x  %03x  %02x    %02x     %s\n",
		   &(lackie->address[0]), &(lackie->address[6]),
		   lackie->local_ring, lackie->remote_ring,
		   lackie->address[12], lackie->address[13],
		   lak2_prstate(lackie, 0));
	    lackie = lackie->next;
	}
    }
    automore_disable();
}


/*
 * lack_toss_connections
 * Terminate all sessions associated with this remote peer
 */
void lack_toss_connections (tr_vpeertype *vrp)
{
    lak_t *lack;

    while((lack = p_dequeue(&vrp->lackie))) 
	lak2_fsm(lack, l2ma_net, l2mi_net_abort);
}

/*
 * l a c k _ i d
 *
 * return a string describing this lack connection.
 */

char *lack_id (lak_t *lackie)
{
    char *str = string_getnext();

    /*
     * Check implementation of string_getnext() before adding
     * more junk to the format string below.  Current code
     * gives you a pointer to 80 bytes of space -- BEWARE.
     */

    if (!lackie)
	return("bogus");
    sprintf(str, "%e %e %02x %02x", &lackie->address[0],
	    &lackie->address[6], lackie->address[12], lackie->address[13]);
    return(str);
}

/*
 * t r m a c 2 l a k
 *
 * Given a TR formated packet return the corresponding Lack entity.  If
 * FromNet is TRUE, then the packet is coming from the Network heading towards
 * the node (use da, sa, dsap, ssap).  If false the packet is coming from the
 * node towards the network (use sa, da, ssap, dsap).
 */

lak_t	*trmac2lak (paktype *pak, uchar *entity, boolean fromnet)
{
    lack2_pak2addr(pak, entity, fromnet);
    return(lack_by_machdr(entity));
}
/*
 * l a c k 2 _ p a k 2 a d d r
 *
 * Given a TR formated packet return the corresponding Lack entity.  If
 * FromNet is TRUE, then the packet is coming from the Network heading towards
 * the node (use da, sa, dsap, ssap).  If false the packet is coming from the
 * node towards the network (use sa, da, ssap, dsap).
 */
void lack2_pak2addr(paktype *pak, uchar entity[], boolean fromnet)
{
    trrif_t	*trh;
    uchar *p;
    int              riflen;
    trh = (trrif_t *)pak->datagramstart;
    riflen	       = (trh->saddr[0] & TR_RII) ? trh->rc_blth & RC_LTHMASK : 0;
    pak->llc2_sapoffset = TRING_ENCAPBYTES + riflen;
    p = pak->datagramstart + pak->llc2_sapoffset;
    if (fromnet) {
	ieee_copy(trh->daddr, entity);  /* da */
	ieee_copy(trh->daddr+6, entity+6);  /* sa */
	entity[12] = ((llc1_hdr_t *) p)->dsap;
	entity[13] = ((llc1_hdr_t *) p)->ssap;
	entity[6] &= ~((uchar) TR_RII);
	entity[13] &= ~((uchar) SAP_RESPONSE);
    }
    else {
	/*
	 * Extract da,sa,dsap,ssap from packet which uniquely identifes the 
	 * two end points on the network.  As this frame is from the media
	 * side, sa is copied first.
	 */
	ieee_copy(trh->saddr, entity);		/* sa first */
	ieee_copy(trh->daddr, entity + 6);	/* da next  */
	entity[0] &= ~((uchar) TR_RII);
	entity[13] = ((llc1_hdr_t *) p)->dsap; /* dsap */
	entity[12] = ((llc1_hdr_t *) p)->ssap; /* ssap */
	entity[12] &= ~((uchar) SAP_RESPONSE);
    }
}

/*
 * remove_vre (vrg, vre, type)
 * Destroy a virtual ring associated with a vre 
 */

void remove_vre ( tr_vgrouptype *vrg, tr_ventrytype *vre, int rsrb_et)
{
    hwidbtype *idb;
    FOR_ALL_HWIDBS(idb) {
          srb_triplet_t srb_triplet;
          srb_sb_get_triplet(idb->firstsw, &srb_triplet);
	  if ((srb_triplet.remoteRing != vrg->vrn) || 
              (srb_triplet.localRing != vre->rn)) {

	      continue;
          }
	  reg_invoke_rsrb_remove_vring(idb, rsrb_et);
    }
}

/*
 * dlc_local_ack_this
 *
 * Should this frame start a Secondary (SDLLC or QLLC) Local Ack Session
 */

boolean dlc_local_ack_this 
    (paktype *pak, hwidbtype *idb, tr_ventrytype *vre)
{
  int i;

  tr_vpeertype  *vrp;
  tr_vgrouptype *vrg;
  srb_triplet_t srb_triplet;

  if (!idb) {
    return(FALSE);
  }
  srb_sb_get_triplet(idb->firstsw, &srb_triplet);
  vrg = vrfind_ringgroup(srb_triplet.remoteRing);
  if (!vrg)
      return(FALSE);

  vrp = pak->peer_ptr;
  if ( (!vrp) || (!(vrp->local_ack)) )
      return(FALSE);

  /* If passthru ring enabled for this ring, don't do it */
  for (i = 0; i < LACK_MAX_PASSTHRU_RINGS; ++i) {
      if (srb_triplet.localRing == lack_passthru_rings[i])
         return(FALSE);
  }

  /* Finally, assume YES */
  return(TRUE);
}

/*
 * dlc_new_vidb
 *
 * Common routine for SDLLC and QLLC to use to create a virtual idb
 */

hwidbtype *dlc_new_vidb(hwidbtype *inidb, char *name,
			int thisring, int bridgenum, int targetring)
{
    idbtype *swidb;
    hwidbtype *idb;
    srb_sb_t *srb_sb;
 

    if ((swidb = vidb_malloc()) == NULL)
	return(NULL);
    idb = swidb->hwptr;

    idb->vencap     = tr_vencap;
    idb->name       = name;

    /*
     * vidb_malloc() is nice enough to provide us with a unique,
     * incrementing number which it stores in the idb->unit field.  We
     * then proceed to destroy this unique number by over-writing a
     * ring number in the field.  The unique identifier is useful for
     * CLS, so rather than creating another counting mechanism, move
     * the 'unit' number somewhere more useful before destroying it.
     * Therefore DlcToIdb will use the 'hw_if_index' to discriminate
     * different vidb's...  It is not practical to use ring numbers as
     * unit numbers within DLSw because of array data structures
     * (dlsw_glob_ring_access) would need to grow too large to support
     * max ring number.
     */
    idb->hw_if_index = idb->unit;
    idb->unit       = thisring;
    idb_init_names(swidb, TRUE);
    idb->state      = IDBS_UP;
    idb->status     = IDB_VIRTUAL | IDB_TR;
    idb->maxmtu     = TR_MTU;
    idb->enctype    = idb->enctype_def = idb->enctype_cfg = 
                                swidb->ip_enctype = ET_SNAP;

    reg_invoke_media_enctype_setup(swidb, ET_SNAP);
    idb->nokeepalive       = FALSE;
    idb->reliability       = 255;
    idb->tr_3com_hack      = FALSE;
    swidb->arp_arpa          = swidb->arp_arpa_def = FALSE;
    swidb->arp_probe         = swidb->arp_probe_def = FALSE;
    swidb->arp_snap          = swidb->arp_snap_def = TRUE;
    swidb->arp_timeout       = ARP_MAXAGE;

    srb_sb = srb_create_sb(swidb);
    if (!srb_sb) {
        printf(nomemory);
        return(NULL);
    }

    srb_sb->srb_thisring   = thisring;
    srb_sb->srb_bridge_num = bridgenum;
    srb_sb->srb_targetring = targetring;
    srb_sb->srb_spanning_explorer = TRUE;

    srb_multiring_enable_all(swidb);

    idb->deencap           = NULL;
    idb->soutput           = (soutput_t)return_nothing;
    idb->oqueue_dequeue    = (oqueue_dequeue_t)return_null;
    idb->reset             = (reset_t)return_nothing;
    idb->shutdown          = (shutdown_t)return_nothing;
    idb->linestate	   = (linestate_t) return_true;
    idb->getlink           = tring_getlink;
    idb->broadcast         = tr_bcast;
    idb->periodic          = (periodic_t)return_nothing;
    idb->counters.resets      = 0;	/* clear reset counter */
    idb->counters.transitions = 0;
    /*
     * Currently 08/94 ;) vidb's act like token-ring interfaces.  Go
     * to tring_idb_init(), and see how things are initialized.  New
     * code depends on having these [required] parameters set, so...
     */
    idb->extract_hwaddr = ieee_extract_hwaddr;
    idb->parse_packet = tring_parse_packet;
    idb->extract_snpa = tr_extract_addr;    


    /*
     * All current 'legacy' code {SD,Q}LLC, SR/TLB, RSRB, etc all
     * eventually have their interfaces tied to some real interface.
     * The only semi-'legacy' (at this time 8/94) application which
     * violates this is AST, which has a 'floater' vidb.
     *
     * CLS over RSRB will also implement a 'floater' vidb, which isn't
     * linked to any real interface.  Therefore, it is possilbe to
     * pass in NULL as the 'anchor' idb.
     *
     * DEBUG note, set the vidb->vidb_link to (long)-1 [RSRB_INVALID_ADDRESS]
     *   to catch any judgement errors ;) */
    if (inidb)
    {				/* legacy code */
	inidb->vidb_link = idb;
	idb->vidb_link   = inidb;
    } else
    {
	idb->vidb_link = (struct hwidbtype_ *) RSRB_INVALID_ADDRESS;
    }
    
    llc2_init_idb(swidb, TRUE);
    return(idb);
}

