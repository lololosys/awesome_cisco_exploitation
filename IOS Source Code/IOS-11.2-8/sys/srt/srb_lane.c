/* $Id: srb_lane.c,v 3.1.6.6 1996/08/03 23:38:31 fox Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb_lane.c,v $
 *------------------------------------------------------------------
 * Token Ring LANE related functions
 *
 * April 1996, Badri Ramaswamy
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srb_lane.c,v $
 * Revision 3.1.6.6  1996/08/03  23:38:31  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.1.6.5  1996/07/11  00:49:35  bbenson
 * CSCdi61915:  TR Lane doesnt check return value of pak_duplicate()
 * Branch: California_branch
 * Continuation of first commit.  Modified lane registries.
 *
 * Revision 3.1.6.4  1996/07/03  18:15:57  bbenson
 * CSCdi61915:  TR Lane doesnt check return value of pak_duplicate()
 * Branch: California_branch
 * Now checking return of pak_duplicate.  Also moved more the code into
 * the registered function(s) in srt/srb_lane.c, where it belongs.
 *
 * Revision 3.1.6.3  1996/06/28  23:26:32  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/05/13  22:02:03  rbadri
 * CSCdi57412:  Token Ring lane subsystem should be only int 4500/RSP J
 * images
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/05/09  14:46:53  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.4.1  1996/04/27  07:26:35  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.2.3  1996/04/21  01:17:30  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Create a new sub-system for TRLANE.
 *
 * Revision 3.1.2.2  1996/04/19  03:00:41  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Initialize TR-LANE from (lane)parser, if not already done.
 *
 * Revision 3.1.2.1  1996/04/17  23:15:24  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Convert TokenRing LANE related functions to registries
 * Move contents of lec_srb* srt/srb_lane.[ch], to facilitate
 * creation of new sub-system.
 *
 * Revision 3.1  1996/04/17  23:00:12  rbadri
 * Placeholder for TokenRing LANE sub-system support.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "mgd_timers.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "subsys.h"
#include "../lane/parser_defs_lane.h"
#include "name.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "../ui/debug.h"

#include "../if/atm.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../if/rif_util.h"
#include "../if/network.h"
#include "../srt/rsrb.h"
#include "../srt/srb.h"
#include "../srt/lack_externs.h"
#include "../srt/srb_lane.h"

#include "../llc2/llc2.h"

#include "../atm/atmsig_api.h"
#include "../atm/atmsig_public.h"
#include "../atm/ilmi_api.h"

#include "../lane/lane.h"
#include "../lane/lane_private.h"
#include "../lane/lec_private.h"

#include "../lane/lane_signal.h"
#include "../lane/lec_fsm.h"
#include "../lane/lec_pdu.h"
#include "../lane/lane_debug.h"
#include "../lane/lane_ilmi.h"
#include "../lane/lane_signal.h"
#include "../lane/lane_registry.h"

#include "../lane/lecmib_util.h"
#include "../lane/lecmib_if.h"


boolean		lec_srb_running = FALSE; /* lec token ring srb is operational */



/*
 * tr_lane_start
 *
 */
void tr_lane_start (void)
{
    lec_srb_running = TRUE;
    reg_invoke_lec_tr_lane_present(lec_srb_running);
    llc2_start();
    
    /* llc2_register(SAP_IBM_SNA, LINK_LLC2, ?)  */
    start_srb_background();
    
    return;
}


void tr_lane_vidb_input (hwidbtype *idb, paktype *pak, 
			 ushort opcode, tr_ventrytype *vre)
{

    trrif_t	*trh;		/* pointer to TR MAC header */
    int   	riflen = 0;	/* length of RIF in TR header */
    sap_hdr	*llc1;		/* pointer to llc-1 header */
    uchar       *frameptr;

    int         xidoffset;
    boolean     is_response;
    uchar       conch;
    long        dhost;
    hwidbtype   *trlanehwidb; 
    idbtype     *trlaneswidb;
    lec_info_t 	*lec_info;


    trh = (trrif_t *) pak->datagramstart;
    riflen = (trh->saddr[0] & TR_RII) ? trh->rc_blth & RC_LTHMASK : 0;

    pak->llc2_sapoffset = TRING_ENCAPBYTES + riflen;
    frameptr    = pak->datagramstart + pak->llc2_sapoffset;

    llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + riflen);
    xidoffset   = (frameptr +2) - pak->datagramstart;
    is_response = (llc1->ssap & SAP_RESPONSE);
    conch = llc1->control & ~P_BIT;     /* snuff out p/f bit */

    GET_TIMESTAMP(idb->lastinput);		/* remember time of last input */
    GET_TIMESTAMP(pak->inputtime);		/* save it in packet too       */

    idb->counters.rx_cumbytes += pak->datagramsize;
    idb->counters.inputs++;

    if (lec_state_debug) {
	buginf("\nTR LANE: rx %s %s, da %e, sa %e, rif %s",
              conch == LLC_TEST ? "explorer" : "other" ,
              is_response ? "response" : "command",
              trh->daddr, trh->saddr,
              get_rif_str(&trh->rc_blth, trprint_rifstr));
    }

    /* send Explorer CMD to ELAN
     */
    dhost = GETLONG(trh->daddr);
    pak->rif_start =  &trh->rc_blth;
    /* DAD - We need to serialize the following at some point
     */
    if (lec_srb_running) {
	FOR_ALL_HWIDBS(trlanehwidb) {
	    FOR_ALL_SWIDBS_ON_HW(trlanehwidb, trlaneswidb) {
		if (lec_enabled(trlaneswidb)) {
		    lec_info = lec_info_from_swidb(trlaneswidb);    
		    if (!lec_info){
			buginf("\n TR LANE: lec_info could not be found");
		    } else {
			if (lec_info->lec_is_srb){
			    pak->if_output = lec_info->swidb;
			    reg_invoke_lane_data_vencap(pak, dhost); 
			    datagram_out(pak);
			    return;
			}
		    }
		}     
	    }
	}
    }
}


void tr_lane_srb_arp_req (lec_info_t *lec_info, paktype *pak)
{
    trrif_t	*trh;	        /* pointer to TR MAC header */
    int            riflen = 0;	/* length of RIF in TR header */
    sap_hdr	*llc1;	        /* pointer to llc-1 header */
    uchar          *frameptr;
    int            xidoffset;
    boolean        is_response;
    uchar          conch;
 
    if (lec_info->lec_is_srb){

	trh = (trrif_t *) (pak->datagramstart + 2);
	riflen = (trh->saddr[0] & TR_RII) ? trh->rc_blth & RC_LTHMASK : 0;
	pak->llc2_sapoffset = TRING_ENCAPBYTES + riflen;

	/* 
	 * we currently do not do anything of interest with frameptr
	 */
	frameptr    = pak->datagramstart + pak->llc2_sapoffset;
	llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + riflen);

	/* we currently do not do anything of interest with xidoffset
	 */
	xidoffset   = (frameptr +2) - pak->datagramstart;
	is_response = (llc1->ssap & SAP_RESPONSE);
	conch = llc1->control & ~P_BIT;     /* snuff out p/f bit */

	/* send any Explorer CMDs to the srb virtual ring */
	if ((conch == LLC_TEST) && (!is_response))
	    vrforward_explorer(pak, 0, NULL, 32);
    }  
}

 
/*
 * Raw enqueue for TR LANE rings
 */
boolean rsrb_tr_lane_oqueue (hwidbtype *idb, paktype *pak, enum HEADTAIL dummy)
{
    pak->if_input = idb->firstsw;
    pak->flags   &= ~PAK_LACK_OUT;
    vrforward_pak(pak, TRUE, RSRB_OP_TR_LANE, RSRB_RIFLF_LEAVEALONE);
    return(TRUE);
}

const blli_type tr_blli_token_ring_data = {
    LEC_BLLI_LENGTH,
    {
	0x60 + 11,		/* Layer 3 id + ISO/EIC TR 9577 */
	0x40, 0x80,		/* 9577 IPI = SNAP              */
	0x80,			/* SNAP ID                      */
	0x00, 0xa0, 0x3e,	/* ATM Forum OUI                */
	0x00, 0x03		/* LE 802.5 Data Direct		*/
    }
};

const blli_type tr_blli_token_ring_mcast = {
    LEC_BLLI_LENGTH,
    {
	0x60 + 11, 		/* Layer 3 id + ISO/EIC TR 9577 */
	0x40, 0x80,		/* 9577 IPI = SNAP              */
	0x80,			/* SNAP ID                      */
	0x00, 0xa0, 0x3e, 	/* ATM Forum OUI                */
	0x00, 0x05		/* LE 802.5 Multicast VCC	*/
    }
};


boolean 
tr_lane_add_vring_interface (hwidbtype *inidb, idbtype *swidb, 
			     ushort elan_rn, uchar srb_bn,
			     ushort vr_rn)
{

    tr_vgrouptype    *vrg;
    tr_ventrytype    *vre;
    hwidbtype           *idb;
    lec_info_t          *lec_info;

    vrg = vrfind_ringgroup(vr_rn);
    if (!vrg){
	printf("\n SRB Ring Number could not be found");
	return(FALSE);
    }

    vre = malloc(sizeof(tr_ventrytype));
    if (!vre){
	printf("\n no space available for adding another LANE SRB Ring Group");
	return(FALSE);
    }

    idb = dlc_new_vidb(inidb, "TRLANEVirtualRing",
                       elan_rn, srb_bn, vr_rn);

    if (!idb){
	printf("\n could allocate a new idb");
	return(FALSE);
    }

    idb->oqueue            = rsrb_tr_lane_oqueue;
    idb->iqueue            = (iqueue_t)tr_lane_vidb_input;

 /* We may need to add something that tags the idb
    for TR LANE SRB use, like the following
  */

    idb->tr_lane_active = TRUE;
    vre->virtual_idb = idb;
    vre->bn       = srb_bn;
    vre->rn       = elan_rn;
    vre->type     = RSRB_ET_TR_LANE;
    vre->outhole.idb = vre->virtual_idb;

    lec_info = lec_info_from_swidb(swidb);
    if (!lec_info) {
       lec_info = reg_invoke_lec_init_idb(swidb);
    }

    if (!lec_info){
	printf("\n a LEC could neither be found nor created");
	return(FALSE);
    }


    /*
     * Set up the ARP stuff in the swidb
     */
    swidb->arp_arpa 	= swidb->arp_arpa_def = FALSE;
    swidb->arp_snap	= swidb->arp_snap_def = TRUE;
    
    /*
     * Set all of the enctypes for the subinterface.
     */
    swidb->ip_enctype	= ET_SNAP;
    reg_invoke_media_enctype_setup(swidb, ET_SNAP);

    lec_info->lec_is_srb = TRUE;
    lec_info->registered_w_les = FALSE;
    lec_info->srb_idb = idb;
/* set the mcast send/forward correctly since they defaulted to EN */ 
    lec_info->mcast_send.blli = &tr_blli_token_ring_mcast;
    lec_info->mcast_forward.blli = &tr_blli_token_ring_mcast;
 
 /* need to copy the LECs MAC address here */

    ieee_copy(lec_info->normalMac, vre->virtual_idb->bia);
    ieee_copy(lec_info->normalMac, vre->virtual_idb->hardware);
    ieee_copy(lec_info->normalMac, vre->macaddr);

    vre->virtual_idb->tr_vring_blk = (tr_ventrytype *)&vrg->rings.qhead;

    enqueue(&vrg->rings, vre);
    vrg->flags |= VRGF_GENUPDATE;
    update_peers();
    return(TRUE);

}

/*
 * Registry function to call srb_enq
 */
void
tr_lane_srb_enq (lec_info_t *lec_info, paktype *pak)
{
    paktype *newpak;

    newpak = pak_duplicate(pak);
    if (newpak) {
	newpak->if_input = lec_info->srb_idb->firstsw;
	newpak->flags |= PAK_SRB_IN;
	srb_enq(srb_explorerQ, newpak);
    }
}

/*
 * Registry function to call  token_getlink
 */
long
tr_lane_tring_getlink (paktype *pak)
{
    return(tring_getlink(pak));
}


/*
 * Registry function to call  tring_parse_packet
 */

iqueue_t
tr_lane_tring_parse_packet (hwidbtype *hwidb, 
			   paktype *pak, 
			   uchar *sniff_ptr)
{
    return(tring_parse_packet(hwidb, pak, sniff_ptr));
}


/*
 * Registry function to call  vrforward_pak; if you insist on passing the
 * correct paramters from above, then change the registry definition.
 */
void
tr_lane_vrforward_pak (lec_info_t *lec_info, paktype *pak)
{
    paktype *newpak;

    newpak = pak_duplicate(pak);
    if (newpak) {
	newpak->if_input = lec_info->srb_idb->firstsw;
	vrforward_pak(newpak,
		      FALSE, 
		      RSRB_OP_LLC2DATA, 
		      RSRB_RIFLF_LEAVEALONE);
    }
}


/*
 * Registry function to call  tr_vencap
 * 
 */

boolean
lane_tr_vencap (paktype *pak, long address)
{
    return(tr_vencap(pak, address));
}

/*
 * Registry function to call  vrfind_ringgroup
 * 
 */
tr_vgrouptype *
tr_lane_vrfind_ringgroup (ushort vrn)
{
    return (vrfind_ringgroup(vrn));
}

boolean
tr_lane_lec_srb_running(void)
{
    return(lec_srb_running);
}

/*
 * tr_lane_init
 *
 * Eventually some sub-system call will call this. For now, this will be
 * called by lec_bridge_init()
 */
void tr_lane_init (subsystype *subsys)
{
    tr_lane_start();
    reg_add_tr_lane_add_vring_interface(tr_lane_add_vring_interface,
					"tr_lane_add_vring_interface");
    reg_add_tr_lane_srb_arp_req(tr_lane_srb_arp_req, "tr_lane_srb_arp_req");
    reg_add_tr_lane_srb_enq(tr_lane_srb_enq, "tr_lane_srb_enq");
    reg_add_tr_lane_tring_getlink(tr_lane_tring_getlink,
				  "tr_lane_tring_getlink");
    reg_add_tr_lane_vrforward_pak(tr_lane_vrforward_pak,
				  "tr_lane_vrforward_pak");
    reg_add_lane_tr_vencap(lane_tr_vencap, "lane_tr_vencap");

    reg_add_tr_lane_tring_parse_packet(tr_lane_tring_parse_packet,
				       "tr_lane_tring_parse_packet");
    reg_add_tr_lane_vrfind_ringgroup(tr_lane_vrfind_ringgroup,
				     "tr_lane_vrfind_ringgroup");
    reg_add_tr_lane_lec_srb_running(tr_lane_lec_srb_running, 
				    "tr_lane_lec_srb_running");
}


/*
 * TR-LANE subsystem header
 */

#define TR_LANE_MAJVERSION      1
#define TR_LANE_MINVERSION      0
#define TR_LANE_EDITVERSION     0

/*
 * Sequenced and required initialization.
 */
SUBSYS_HEADER(tr_lane, TR_LANE_MAJVERSION, TR_LANE_MINVERSION,
	      TR_LANE_EDITVERSION, tr_lane_init, SUBSYS_CLASS_PROTOCOL,
              "seq: srb", 
	      NULL);

