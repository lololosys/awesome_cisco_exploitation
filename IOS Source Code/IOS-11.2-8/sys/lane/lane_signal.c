/* $Id: lane_signal.c,v 3.2.58.10 1996/09/10 14:34:00 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_signal.c,v $
 *------------------------------------------------------------------
 * lane_signal.c: interface between LAN Emulation & ATM signaling
 *
 * January 1995, Steve Jay
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_signal.c,v $
 * Revision 3.2.58.10  1996/09/10  14:34:00  rbadri
 * CSCdi62648:  Pulling the media cable on one ATM card can bring down
 * other LECs
 * Branch: California_branch
 * Decrement count of multipoint VC drops
 *
 * Revision 3.2.58.9  1996/08/28  12:54:47  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.58.8  1996/08/05  22:40:04  michellw
 * CSCdi65167:  All other clients are dropped when one client drops
 * Branch: California_branch
 * Decrment the counter of outstanding DROP (drops_out) in
 * lane_discard().
 *
 * Revision 3.2.58.7  1996/07/25  00:16:36  rbadri
 * CSCdi63919:  LANE Client can invalidate fast cache entries too often
 * Branch: California_branch
 * New flag to facilitate better cache invalidation when VCs go down.
 *
 * Revision 3.2.58.6  1996/07/21  00:56:39  cakyol
 * CSCdi63405:  LANE LES: lec gets stuck in state T
 * Branch: California_branch
 *
 * Revision 3.2.58.5  1996/06/26  08:49:38  cakyol
 * CSCdi61253:  pvc cleanup in LANE left some stuff around (lane_call
 * (...))
 * Branch: California_branch
 *
 * Revision 3.2.58.4  1996/06/17  22:48:02  shj
 * CSCdi59010:  LANE-3-LANE_SIG_INV_MSG - unexpected CONNECT_ACK
 * Branch: California_branch
 * Can begin RELEASE before CONNECT_ACK comes back, so allow for
 * CONNECT_ACK in RELEASING state.
 *
 * Revision 3.2.58.3  1996/05/31  01:08:31  shj
 * CSCdi46584:  LANE signaling needs to be smarter about ADD_PARTY
 * Branch: California_branch
 * Queue & retry ADD_PARTY's that bounce with "too many ADD_PARTY's.
 *
 * CSCdi55191: LANE can leave <stranded> p2mp svc - does DROP instead of
 * RELEASE
 * Add count of outstanding DROP's, so we can tell for sure when a leaf is
 * the last one.
 *
 * CSCdi55975: LANE: %LANE-3-LANE_SIG_INV_MSG: invalid msg received
 * (3 BD3FA4 8)
 * Make sure we zero out vcd of p2mp vc when last leaf is gone, so we
 * don't later think we already have an established vc.
 *
 * Revision 3.2.58.2  1996/05/09  14:32:36  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.58.1.18.2  1996/05/03  21:45:53  kjadams
 * CSCdi56624:  Incorrect BLLI for Token Ring LANE Data Direct VCC
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * confine blli change to lane.  Use LANE_BLLI_MAX_PID when looping
 * thru PIDs to see if there is a valid match on the blli.  Don't use
 * the ATM_SIG_MAX_BLLI unless you're really looking at the max number of
 * blli ie's in a setup message.
 *
 * Revision 3.2.58.1.18.1  1996/04/27  07:06:29  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.58.1.8.4  1996/04/16  02:44:25  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.58.1.8.3  1996/04/09  02:16:08  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Easter bunny had a field day last weekend through the code.  Non-cisco
 * coding styles squashed.  Dangling else's now attached to }'s, else's
 * now attached to if's, 10 character tabs removed...
 *
 * Revision 3.2.58.1.8.2  1996/04/08  14:51:46  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.58.1.8.1  1996/04/08  01:56:56  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.58.1  1996/03/18  20:32:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:03:06  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:38:20  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2.62.1  1996/03/22  22:54:31  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.1  1996/03/05  06:43:00  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:10:20  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:46:21  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:06:41  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:50:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.13  1995/09/01  00:59:32  shj
 * CSCdi39584:  set sdu sizes per connection - part 1
 * Mods for LES/BUS setting sdu size in SETUP's.
 *
 * Revision 2.12  1995/08/31  18:03:19  shj
 * CSCdi38903:  LANE signal code should set idb ptr in all msgs to
 * atmSig_api
 *
 * Revision 2.11  1995/07/29  01:10:15  mordock
 * CSCdi37882:  currekt missed-spelled wurds
 * Corrected spelling mistakes in various messages.
 *
 * Revision 2.10  1995/07/20  23:35:44  shj
 * CSCdi37356:  LANE signaling uses too much stack space
 * 2nd try:  static variables bad, malloc good.
 *
 * Revision 2.9  1995/07/20  18:18:13  shj
 * CSCdi37356:  LANE signaling uses too much stack space
 * Use one static sig_api_msg structure instead of several automatic's.
 *
 * Revision 2.8  1995/07/19  21:57:20  shj
 * CSCdi37300:  LANE signaling debug msgs need more & better info
 *
 * Add interface to messages, show called/calling atm-addresses on
 * SETUP & ADD_PARTY, show sent & recvd blli's.
 *
 * Revision 2.7  1995/07/18  21:58:49  shj
 * CSCdi37270:  LANE Server has clients hung in Terminate state
 *
 * Make sure vcc_info's with deferred ADD's get set to INITIALIZED state
 * when RELEASE_COMPLETE comes in on 1st party of p2mp vcc.
 *
 * Revision 2.6  1995/07/17  17:42:41  shj
 * CSCdi37185:  LANE signaling can mishandle p2mp vcs
 *
 * Make sure vcd is cleared out when p2mp vcc is RELEASE'd.
 *
 * Revision 2.5  1995/07/07  01:37:52  shj
 * CSCdi36794:  LANE confusion caused by multiple pending signaling actions
 *
 * Handle RELEASE/DROP_PARTY/CANCEL while previous SETUP/ADD_PARTY/LISTEN
 * still in progress.
 *
 * Revision 2.4  1995/06/26  23:21:58  nfinn
 * CSCdi35235:  LANE config server access sequencing incorrect
 *
 * Revision 2.3  1995/06/23  18:48:06  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/19  06:45:54  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:19:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <ciscolib.h>
#include "config.h"
#include "interface_private.h"
#include "address.h"
#include "logger.h"
#include "../lane/msg_lane.c"		/* Not a typo, see logger.h */

#include "../if/atm.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../atm/atmsig_api.h"

#include "lane.h"
#include "lane_private.h"
#include "lane_signal.h"
#include "parser_defs_lane.h"

#include "../ui/debug.h"
#include "lane_debug.h"


#define RELEASE_COLLIDE_BUG

#define LANE_SIG_BUGINF(x) if (lane_signal_debug) \
                                 {lane_sig_buginf_head(); buginf x;}
#define SET_LANE_SIG_BUGINF_IDB(x) lane_sig_buginf_idb = (x)

static idbtype *lane_sig_buginf_idb;

static void
lane_sig_buginf_head (void)
{
    buginf("\nLSIG %s: ", lane_sig_buginf_idb ?
           lane_sig_buginf_idb->namestring : "");
}


static void
lane_sig_buginf_addrs (uchar *called, uchar *calling)
{
    char        nsapstr[ATM_MAX_NSAP_STRING];

    if (called) {
        lane_sig_buginf_head();
        atm_printnsap(called, nsapstr);
        buginf("called  %s", nsapstr);
    }
    if (calling) {
        lane_sig_buginf_head();
        atm_printnsap(calling, nsapstr);
        buginf("calling %s", nsapstr);
    }
}

#define LANE_BLLI_LENGTH                9
#define LANE_BLLI_FIXED_PART_LENGTH     7
#define LANE_BLLI_PID_OFFSET            7

/*
 * See section 5.4.5.9 of ATM Forum UNI 3.0 spec and section 3.3.2.7.4
 * of the LAN Emulation 1.0 spec for what this gobbledegook means.
 */
static uchar    lane_sig_blli[LANE_BLLI_LENGTH] = {
    0x60 + 11,                  /* Layer 3 id + ISO/EIC TR 9577 */
    0x40, 0x80,                 /* 9577 IPI = SNAP              */
    0x80,                       /* SNAP ID                      */
    0x00, 0xa0, 0x3e,           /* ATM Forum OUI                */
    0x00, 0x00                  /* PID (lane_blli_t) goes here  */
};

/*
 * The entries in this array have to match the order of the lane_vc_type_t
 * enum in lane.h.
 */
static uchar    lane_type_to_blli_pid[LANE_MAX_VC_TYPE + 1] = {
    0,                          /* 0 LANE_VC_TYPE_UNKNOWN */
    LANE_BLLI_CONTROL,          /* 1 LANE_LEC_DIRECT      */
    LANE_BLLI_CONTROL,          /* 2 LANE_LEC_DISTRIBUTE  */
    LANE_BLLI_CONTROL,          /* 3 LANE_LEC_CONFIGURE   */
    LANE_BLLI_ETHER_MULTI,      /* 4 LANE_LEC_SEND        */
    LANE_BLLI_ETHER_MULTI,      /* 5 LANE_LEC_FORWARD     */
    LANE_BLLI_ETHER_DATA,       /* 6 LANE_LEC_DATA        */
    LANE_BLLI_CONTROL,          /* 7 LANE_LES_DIRECT      */
    LANE_BLLI_CONTROL,          /* 8 LANE_LES_DISTRIBUTE  */
    LANE_BLLI_ETHER_MULTI,      /* 9 LANE_BUS_SEND        */
    LANE_BLLI_ETHER_MULTI,      /* 10 LANE_BUS_FORWARD     */
    LANE_BLLI_CONTROL,          /* 11 LANE_CONFIGURE       */
    LANE_BLLI_TR_DATA,          /* 12 LANE_LEC_TR_DATA     */
    LANE_BLLI_TR_MULTI,         /* 13 LANE_LEC_TR_SEND  mcast */
    LANE_BLLI_TR_MULTI,         /* 14 LANE_LEC_TR_FORWARD  */
    LANE_BLLI_TR_MULTI,         /* 15 LANE_BUS_TR_SEND     */
    LANE_BLLI_TR_MULTI          /* 16 LANE_BUS_TR_FORWARD  */
};

static void
lane_sig_buginf_blli(char *tag, blli_type *b)
{
    int   n;

    lane_sig_buginf_head();
    buginf("%s blli len %d", tag, b->length);
    for(n = 0; n < LANE_BLLI_LENGTH; n++)
        buginf(" %02x", b->string[n]);
}


/*
 * lane_sig_make_blli
 *
 * Construct LAN Emulation signaling Broadband Lower Layer Information (blli)
 * block.  Definition is in LANE spec.  Consists of fixed part + connection
 * type in last bytes.

 */
void
lane_sig_make_blli (blli_type *b, lane_vc_type_t type)
{
    ushort pid = lane_type_to_blli_pid[type];

    b->length                           = LANE_BLLI_LENGTH;
    bcopy(lane_sig_blli, b->string, LANE_BLLI_LENGTH);
    b->string[LANE_BLLI_PID_OFFSET]     = (pid >> 8) && 0xff;
    b->string[LANE_BLLI_PID_OFFSET + 1] = pid & 0xff;

    return;
}

/*
 * lane_sig_blli_match
 *
 * Check an array of n blli_type's for one matching the LAN Emulation
 * blli for the specified lane_vc_type.

 */
boolean
lane_sig_blli_match (int n, blli_type *b, lane_vc_type_t type)
{
    int   pid;

    while(n--) {
        pid = (b->string[LANE_BLLI_PID_OFFSET] << 8) |
               b->string[LANE_BLLI_PID_OFFSET + 1];

        if ((b->length == LANE_BLLI_LENGTH) &&
            !bcmp(lane_sig_blli, b->string, LANE_BLLI_FIXED_PART_LENGTH) &&
            (pid == lane_type_to_blli_pid[type])) {
              return TRUE;
        }
        b++;
    }

    return FALSE;

}


/*
 * lane_send_release_comp
 *
 * Send RELEASE_COMPLETE with specified call_id & cause.
 */
static void
lane_sig_send_release_comp (void                *call_id,
                            sig_api_cause_code   cause,
                            idbtype             *swidb)
{
    sig_api_msg         *omsg;

    omsg = malloc(sizeof(sig_api_msg));
    if (omsg == NULL)
        return;
    LANE_SIG_BUGINF(("rel_comp %d", cause));
    omsg->opcode                 = ATM_RELEASE_COMPLETE;
    omsg->idb                    = swidb;
    omsg->u.release_comp.call_id = call_id;
    omsg->u.release_comp.cause   = cause;

    process_enqueue(atmSig_outputQ, omsg);
}


/*
 * lane_add_vcc_to_sig_chain
 *
 * Add a lane_vcc_info_t to head of lane_signal chain if
 * not already in chain.
 */
static void
lane_add_vcc_to_sig_chain (lane_vcc_info_t *lv)
{
    lane_info_t   *info;

    if (lv->in_sig_chain)
        return;

    info                = lv->swidb->lane_info;
    lv->next_s        = info->signal_chain;
    info->signal_chain  = lv;
    lv->in_sig_chain    = TRUE;
    LANE_SIG_BUGINF(("add %x -> %x", lv, lv->next_s));  
}

/*
 * lane_remove_vcc_from_sig_chain
 *
 * Remove a lane_vcc_info_t from signal chain, if it's on it.
 */
static void
lane_remove_vcc_from_sig_chain (lane_vcc_info_t *tlv)
{
    lane_vcc_info_t       *lv;
    lane_vcc_info_t       *nlv;
    lane_info_t           *info;
    
    if (!tlv->in_sig_chain)
        return;
    
    nlv               = tlv->next_s;
    tlv->next_s     = NULL;
    tlv->in_sig_chain = FALSE;
    tlv->vcc_state    = LANE_VCC_INITIALIZED;
    
    info = tlv->swidb->lane_info;
    if (tlv == info->signal_chain) {
        info->signal_chain = nlv;
        LANE_SIG_BUGINF(("remove %x, head %x",
                         tlv, info->signal_chain));
        return;
    } else {
        FOR_ALL_LANE_SIG_VCCS(lv, info) {
            if (lv->next_s == tlv) {
                lv->next_s = nlv;
                LANE_SIG_BUGINF(("remove %x,  new %x -> %x",
                                 tlv, lv, lv->next_s));
                return;
            }
        }
        errmsg(&msgsym(LANE_SIG_CHAIN, LANE), "sig", tlv, info);
    }
    
}

/*
 * lane_add_to_mcgroup
 *
 * Add a lane_vcc_info_t to an mcgroup chain.
 */
static inline void
lane_sig_add_to_mcgroup (lane_vcc_info_t *lv, lane_signal_mcgroup_t *mcg)
{
    lv->is_multicast = TRUE;
    lv->mc_head      = mcg;
    lv->next_mc      = mcg->root_chain;
    mcg->root_chain  = lv;
    mcg->n_members++;
}

/*
 * lane_sig_remove_from_mcgroup
 *
 * Remove a lane_vcc_info_t from mcgroup chain.
 */
static void
lane_sig_remove_from_mcgroup (lane_vcc_info_t *tlv, lane_signal_mcgroup_t *mcg)
{
    lane_vcc_info_t       *lv;
    lane_vcc_info_t       *nlv;
    boolean		   found;

    found	      = FALSE;
    nlv               = tlv->next_mc;
    tlv->next_mc      = NULL;
    tlv->mc_head      = NULL;
    tlv->is_multicast = FALSE;
    
    if (tlv == mcg->root_chain) {
        mcg->root_chain = nlv;
        found = TRUE;
    } else {
        FOR_ALL_LANE_MCGROUP_VCCS(lv, mcg) {
            if (lv->next_mc == tlv) {
                lv->next_mc = nlv;
                found = TRUE;
		break;
            }
        }
    }

    /* tlv SHOULD be in mcg */
    if (! found) {
        errmsg(&msgsym(LANE_SIG_CHAIN, LANE), "mcgroup", tlv, mcg);
	return;
    }

    if (--mcg->n_members <= 0) mcg->n_members = 0;
    if (mcg->root_chain == NULL)
	lane_create_mcgroup(mcg, mcg->lane_info);
}

/*
 * lane_sig_mc_release
 *
 * RELEASE or RELEASE_COMPLETE received on multicast vc.
 * Notify user of each leaf.
 */

static void
lane_sig_mc_release (lane_signal_mcgroup_t *mcg, sig_api_opcode op)
{
    lane_vcc_info_t             *lv;
    boolean			 done = FALSE;

    while ((lv = mcg->root_chain)) {

        LANE_SIG_BUGINF(("mc RELEASE %x %d %x %d",
                         mcg, mcg->n_members, lv, lv->vcc_state));

	if (--mcg->n_members < 0)
	    mcg->n_members = 0;

        mcg->root_chain  = lv->next_mc;
        lv->next_mc      = NULL;
        lv->mc_head      = NULL;
        lv->is_multicast = FALSE;
        lv->vcc_state    = LANE_VCC_INITIALIZED;
	lv->vcd		 = 0;
	lv->call_id	 = NULL;

	if (mcg->root_chain == NULL) {
	    lane_create_mcgroup(mcg, mcg->lane_info);
	    done = TRUE;
	}

        lane_remove_vcc_from_sig_chain(lv);
        (lv->callback)(lv, LANE_CALL_RELEASE_COMPLETE);

	if (done) break;
    }
    
    /*
     * do NOT access the mcg structure after returning
     * from processing the LAST leg of the multicast.
     * This is becoz the user callback may delete the 
     * mcg structure and we do NOT want to access it 
     * in this case.
     */
}

/*
 * lane_sig_do_deferred_adds
 *
 * If we were requested to do an ADD_PARTY while 1st party SETUP was
 * outstanding, ADD wasn't sent out.  When CONNECT comes back, we
 * need to send out any ADD's that were deferred.
 *
 * Deferred ADD's are queued as lane_vcc_info_t's chained to the
 * mcgroup in state ADD_SENT, but NOT linked into the sig chain.
 */

static void
lane_sig_do_deferred_adds(lane_signal_mcgroup_t *mcg)
{
    lane_vcc_info_t     *lv,
                        *nlv;
    sig_api_msg         *omsg;

    lv = mcg->root_chain;
    while (lv) {

        nlv = lv->next_mc;
        if (!lv->in_sig_chain) {

            switch(lv->vcc_state) {
            case LANE_VCC_ADD_SENT:
                omsg = malloc(sizeof(sig_api_msg));
                if (omsg) {
                    omsg->opcode                    = ATM_ADD_PARTY;
                    omsg->idb                       = lv->swidb;
                    omsg->u.add_party.call_id       = mcg->call_id;
                    omsg->u.add_party.endpoint_id   =
                        atmSig_api_allocate_endpoint_id(mcg->call_id);
                    omsg->u.add_party.called_party  = lv->called_addr;
                    lv->call_id    = omsg->u.add_party.endpoint_id;
                    lv->vcd        = mcg->vcd;
                    lv->svc        = TRUE;
                    lv->vcc_state  = LANE_VCC_ADD_SENT;
                    mcg->adds_out++;
                    lane_add_vcc_to_sig_chain(lv);
                    LANE_SIG_BUGINF(("deferred ADD_PARTY %d vcd %d %x %x %x %x",
                                    mcg->adds_out, mcg->vcd, mcg, lv,
                                    mcg->call_id,
                                    omsg->u.add_party.endpoint_id));
                    process_enqueue(atmSig_outputQ, omsg);
                } else {
                    lane_sig_remove_from_mcgroup(lv, mcg);
                    lv->vcc_state = LANE_VCC_INITIALIZED;
                    (lv->callback)(lv, LANE_CALL_ADD_FAILED);
                }
                break;

            case LANE_VCC_DROPPING:
                lane_sig_remove_from_mcgroup(lv, mcg);
                lv->vcc_state = LANE_VCC_INITIALIZED;
                LANE_SIG_BUGINF(("deferred DROP %x %d", lv, mcg));
                (lv->callback)(lv, LANE_CALL_DROP_COMPLETE);
                break;

            default:
                break;
            }  /* end switch(lv->vcc_state) */

        } /* end if (!lv->in_sig_chain) */

        lv = nlv;

    }  /* end while (lv) */

}


/*
 * lane_sig_find_call
 *
 * Find a lane_vcc_info_t on the signal chain with the specified call_id.
 *
 * For p2mp svc's, the call_id is in the multicast group structure.
 */

static lane_vcc_info_t *
lane_sig_find_call (void *call, idbtype *swidb)
{
    lane_vcc_info_t       *lv;

    FOR_ALL_LANE_SIG_VCCS(lv, swidb->lane_info) {
        if (lv->is_multicast) {
            if (call == lv->mc_head->call_id)
                return lv;
        } else {
            if (call == lv->call_id)
                return lv;
        }
    }
    return NULL;
}

/*
 * lane_sig_find_endpoint
 *
 * Find lane_vcc_info with specified call_id (found in multicast group
 * structure) and endpoint.  endpoint is stored in vcc_info->call_id
 * on p2mp vcc.
 */

static lane_vcc_info_t *
lane_sig_find_endpoint (void *call, void *endpoint, idbtype *swidb)
{
    lane_vcc_info_t     *lv;

    FOR_ALL_LANE_SIG_VCCS(lv, swidb->lane_info) {
        if (lv->is_multicast && (lv->call_id == endpoint) &&
            (lv->mc_head->call_id == call))

            return lv;
    }
    return NULL;
}

/*
 * lane_sig_api_callback
 *
 * Process callbacks from signaling code.
 *
 * The idea is search the chain of lane_vcc_info's for a call_id matching
 * the one in the incoming message, and then take the action appropriate
 * for the incoming message & the state of the vcc.  This usually means
 * calling whoever called us.
 */

static void
lane_sig_api_callback (void *m)
{
    sig_api_msg           *imsg = (sig_api_msg *)m;
    sig_api_msg           *omsg;
    lane_vcc_info_t       *lv;
    lane_vcc_info_t       *nlv;
    lane_vcc_info_t        vc_setup;
    lane_vcc_state_t       code;
    ushort                sdu_size;
    boolean		  drop;

    SET_LANE_SIG_BUGINF_IDB(imsg->idb);
    
    switch(imsg->opcode) {

    case ATM_LISTEN_NSAP:
        
        LANE_SIG_BUGINF(("recv LISTEN %x %d",
                         imsg->u.listen.user_handle, imsg->u.listen.result));
        lv = imsg->u.listen.user_handle;
        switch(lv ? lv->vcc_state : LANE_VCC_NOT_FOUND) {
        case LANE_VCC_LISTEN_SENT:
        case LANE_VCC_CANCEL_SENT:
            break;                      /* valid cases */
        default:
            errmsg(&msgsym(LANE_SIG_INV_MSG, LANE), ATM_LISTEN_NSAP, lv,
                   lv ? lv->vcc_state : 0);
            return;
        }
        if (imsg->u.listen.result == ATM_SIGAPI_OK) {
            if (lv->vcc_state == LANE_VCC_CANCEL_SENT) {
                /*
                 * If in CANCEL_SENT state, user sent CANCEL while LISTEN was
                 * pending.  Ignore the LISTEN done msg, expect the CANCEL
                 * done msg to come back later.
                 */
                return;
            }
            lv->vcc_state = LANE_VCC_LISTENING;
            (lv->callback)(lv, LANE_CALL_LISTEN_COMPLETE);
        } else {
            code = (lv->vcc_state == LANE_VCC_LISTEN_SENT) ?
                LANE_CALL_LISTEN_FAILED : LANE_CALL_CANCEL_COMPLETE;
            lv->vcc_state = LANE_VCC_INITIALIZED;
            (lv->callback)(lv, code);
        }

        return;


    case ATM_CANCEL_NSAP:

        LANE_SIG_BUGINF(("recv CANCEL %x %d",
                         imsg->u.cancel.user_handle, imsg->u.cancel.result));
        lv = imsg->u.cancel.user_handle;
        if ((lv == NULL) || (lv->vcc_state != LANE_VCC_CANCEL_SENT)) {
            errmsg(&msgsym(LANE_SIG_INV_MSG, LANE), ATM_CANCEL_NSAP, lv,
                   lv ? lv->vcc_state : 0);
            return;
        }
        lane_remove_vcc_from_sig_chain(lv);
        (lv->callback)(lv, LANE_CALL_CANCEL_COMPLETE);
        return;
    

    case ATM_SETUP:

        LANE_SIG_BUGINF(("recv SETUP %x %d",
                         imsg->u.setup.call_id, imsg->u.setup.vcd));
        if (lane_signal_debug)
            lane_sig_buginf_addrs(imsg->u.setup.called_party.addr,
                                  imsg->u.setup.calling_party.addr);

        if (imsg->u.setup.multipoint) {
            LANE_SIG_BUGINF(("reject p2mp"));
            goto bounce_call;
        }

        FOR_ALL_LANE_SIG_VCCS(lv, imsg->idb->lane_info) {
            
            if ((lv->vcc_state == LANE_VCC_LISTENING) &&
                lane_addr_equal(&lv->called_addr,
                                &imsg->u.setup.called_party))
            {
		/* If not LANE BLLI, reject call */

                if (!lane_sig_blli_match(ATM_MAX_BLLI_NUM,
                                         &imsg->u.setup.blli[0],
                                         lv->vc_type)) {
                    lane_sig_send_release_comp(imsg->u.setup.call_id,
                                               ATM_INCOMPAT_DEST, lv->swidb);
                    return;
                }
		/*
		 * If there is a SDU size mis match, reject the call
		 * Since the BLLI match is already done, we can trust
		 * the following conversion to get the PID in BLLI.
		 */
		switch( lane_type_to_blli_pid[lv->vc_type]) {
		case LANE_BLLI_CONTROL:
		    sdu_size = LANE_SDU_SIZE_1516;
		    break;
		default:
		    sdu_size = lane_frame_to_sdu_size(lv->swidb->
						   lane_info->max_data_frame);
		    break;
		}
		if ((sdu_size != imsg->u.setup.fwd_sdu_size) ||
		    (sdu_size != imsg->u.setup.bwd_sdu_size)) {
		    if (imsg->idb->hwptr->atm_db->mtu_reject_call) {
			errmsg(&msgsym(LANE_SIG_INV_SDU, LANE), 
			       &imsg->u.setup.calling_party,
			       imsg->u.setup.fwd_sdu_size,
			       imsg->u.setup.bwd_sdu_size,
			       lv->swidb->namestring, sdu_size);
			lane_sig_send_release_comp(imsg->u.setup.call_id,
                                                   ATM_AAL_PARAMS_NOT_SUPP,
                                                   lv->swidb);
                        return;
 
                     }
                 }


                /* Notify listener */

                LANE_SIG_BUGINF(("notify %x", lv));
                vc_setup              = *lv;            /*structure copy */
                vc_setup.in_sig_chain = FALSE;
                vc_setup.is_multicast = FALSE;
                vc_setup.calling_addr = imsg->u.setup.calling_party;
                vc_setup.call_id      = imsg->u.setup.call_id;
                vc_setup.vcd          = imsg->u.setup.vcd;
                nlv = (lv->callback)(&vc_setup, LANE_CALL_SETUP_RECEIVED);

                /* If listener accepts call, send back connect, add this  */
                /* vcc to our list.                                       */

                if (nlv) {
                    omsg = malloc(sizeof(sig_api_msg));
                    if (omsg) {
                        omsg->opcode                 = ATM_CONNECT;
                        omsg->idb                    = nlv->swidb;
                        omsg->u.connect.call_id      = nlv->call_id;
                        omsg->u.connect.flags        = ATM_VC_AAL5LANE;
                        if (nlv->no_svc_timeout)
                            omsg->u.connect.flags   |= (VC_FLAG_NOIDLETIME |
							VC_FLAG_NOBUMP_CACHE);
                        omsg->u.connect.user_vc_type = nlv->vc_type;
                        lane_sig_make_blli(&omsg->u.connect.blli,
                                           nlv->vc_type);
                        process_enqueue(atmSig_outputQ, omsg);
                        LANE_SIG_BUGINF(("accept %x", nlv));
                        lane_add_vcc_to_sig_chain(nlv);
                        nlv->vcc_state = LANE_VCC_CONNECT_SENT;
                        return;
                    } else {
                        nlv->vcc_state = LANE_VCC_INITIALIZED;
                        (*nlv->callback)(nlv, LANE_CALL_CALL_FAILED);
                        goto bounce_call;
                    }
                } else {
		    goto bounce_call;
		}
                
            } /* end if (listening && nsap match) */
            
        } /* end FOR_ALL_LANE_SIG_VCCS */
        
        /*
         * We don't have matching listen, or some other error occurred.
         */
    bounce_call:
        lane_sig_send_release_comp(imsg->u.setup.call_id, ATM_CALL_REJECTED,
                                   imsg->idb);
        
        return;
        
        
    case ATM_CONNECT:        
	drop = FALSE;
        LANE_SIG_BUGINF(("recv CONNECT %x %d", \
                         imsg->u.connect.call_id, imsg->u.connect.vcd));
        lv = lane_sig_find_call(imsg->u.connect.call_id, imsg->idb);
        switch(lv ? lv->vcc_state : LANE_VCC_NOT_FOUND) {
        case LANE_VCC_RELEASING:
        case LANE_VCC_DROPPING:
	    drop = TRUE;
            break;
        case LANE_VCC_SETUP_SENT:
            /* normal case, fall through to process CONNECT */
            break;
        default:
            errmsg(&msgsym(LANE_SIG_INV_MSG, LANE), ATM_CONNECT,
                   lv, lv ? lv->vcc_state : (int)imsg->u.connect.call_id);
            return;
        }
        if (lane_signal_debug)
            lane_sig_buginf_blli("recvd", &imsg->u.connect.blli);
        if ((imsg->u.connect.blli.length != 0) &&
            !lane_sig_blli_match(1, &imsg->u.connect.blli, lv->vc_type)) {
#if !defined(LANE_NO_CHECK_CONNECT_BLLI)
            /*
             * blli doesn't match what we sent.  Send release, leave
             * state as "SETUP_SENT".  This will cause us to notify
             * user of failed call when RELEASE_COMPLETE comes in.
             */
            errmsg(&msgsym(LANE_SIG_ERR, LANE), "bad blli in connect");
	    drop = TRUE;
#else
            LANE_SIG_BUGINF(("inv blli"));
#endif
        }

	/* record these ALWAYS even if we will drop */
        lv->vcd       = imsg->u.connect.vcd;
        if (lv->is_multicast) {
            /*
             * For p2mp, returned endpoint becomes id for this leaf.
             * call_id used for all parties is already in mcg->call_id.
             */
            lv->call_id      = imsg->u.connect.endpoint_id;
            lv->mc_head->vcd = lv->vcd;
	}

	/* drop now if you want to */
	if (drop) {
            omsg = malloc(sizeof(sig_api_msg));
            if (omsg) {
                omsg->opcode            = ATM_RELEASE;
                omsg->idb               = imsg->idb;
                omsg->u.release.call_id = imsg->u.connect.call_id;
                omsg->u.release.cause   = ATM_NORMAL_UNSPECIFIED;
                process_enqueue(atmSig_outputQ, omsg);
            }
            return;
	}

        lv->vcc_state = LANE_VCC_CONNECTED;
	if (lv->is_multicast) {
            (lv->callback)(lv, LANE_CALL_ADD_COMPLETE);
            lane_sig_do_deferred_adds(lv->mc_head);
        } else {
            (lv->callback)(lv, LANE_CALL_CALL_COMPLETE);
        }
        
        return;
        
        
    case ATM_CONNECT_ACK:
        
        lv = lane_sig_find_call(imsg->u.connect_ack.call_id, imsg->idb);
        LANE_SIG_BUGINF(("recv CONNECT_ACK %x %x %x", \
                         imsg->u.connect_ack.call_id, lv,
                         lv ? lv->vcc_state : 0));
        switch (lv ? lv->vcc_state : LANE_VCC_NOT_FOUND) {
        case LANE_VCC_RELEASING:
	    omsg = malloc(sizeof(sig_api_msg));
	    if (omsg) {
		omsg->opcode            = ATM_RELEASE;
		omsg->idb               = imsg->idb;
		omsg->u.release.call_id = imsg->u.connect_ack.call_id;
		omsg->u.release.cause   = ATM_NORMAL_UNSPECIFIED;
		process_enqueue(atmSig_outputQ, omsg);
	    }
            return;

        case LANE_VCC_CONNECT_SENT:
            break;                      /* normal case */

        default:
            errmsg(&msgsym(LANE_SIG_INV_MSG, LANE), ATM_CONNECT_ACK,
                   lv, lv ? lv->vcc_state : (int)imsg->u.connect_ack.call_id);
            return;
        }
        
        lv->vcc_state = LANE_VCC_CONNECTED;
        (lv->callback)(lv, LANE_CALL_CALL_COMPLETE);
        
        return;
        
        
    case ATM_RELEASE:

        LANE_SIG_BUGINF(("recv RELEASE %x %d",
                         imsg->u.release.call_id, imsg->u.release.cause));

        /* always respond with RELEASE_COMPLETE  */

        lane_sig_send_release_comp(imsg->u.release.call_id,
                                   ATM_NORMAL_UNSPECIFIED, imsg->idb);        

        lv = lane_sig_find_call(imsg->u.release.call_id, imsg->idb);
        if (!lv) {
            LANE_SIG_BUGINF(("no call"));
            return;
        }        
        /*
         * If multicast vc going away, handle special case & exit.
         */
        if (lv->is_multicast) {
            LANE_SIG_BUGINF(("mc vcd %d %d",
                             lv->mc_head->vcd,
                             lv->mc_head->n_members));
            lane_sig_mc_release(lv->mc_head, ATM_RELEASE);
            return;
        }
                  
        switch(lv->vcc_state) {
        case LANE_VCC_CONNECTED:
        case LANE_VCC_SETUP_RECEIVED:
        case LANE_VCC_CONNECT_SENT:
            code = LANE_CALL_RELEASE_RECEIVED;
            break;
        case LANE_VCC_SETUP_SENT:
            code = LANE_CALL_CALL_FAILED;
            break;
        case LANE_VCC_RELEASING:
        case LANE_VCC_DROPPING:
            /*
             * If we've sent release, do nothing with incoming release
             * we'll finish up when release complete comes back.  Percy
             * promises we'll get one.  State may also be DROPPING, as
             * we do RELEASE on last party of p2mp, but callers of
             * this code (Norm) expect to see DROPPING state.
             */
            LANE_SIG_BUGINF(("already releasing"));
#if defined(RELEASE_COLLIDE_BUG)
            if (lv->is_multicast) {
                lane_sig_remove_from_mcgroup(lv, lv->mc_head);
                code = LANE_CALL_DROP_COMPLETE;
            } else {
                code = LANE_CALL_RELEASE_COMPLETE;
            }
            break;
#else
            return;
#endif
        default:
            errmsg(&msgsym(LANE_SIG_INV_MSG, LANE), ATM_RELEASE,
                   lv, lv->vcc_state);
            return;
        }

        LANE_SIG_BUGINF(("%x %d %d", lv, lv->vcc_state, code));
        lane_remove_vcc_from_sig_chain(lv);
        (lv->callback)(lv, code);
        
        return;

        
    case ATM_RELEASE_COMPLETE:

        LANE_SIG_BUGINF(("recv RELEASE_COMPLETE %x %d", \
                         imsg->u.release_comp.call_id,
                         imsg->u.release_comp.cause));
        lv = lane_sig_find_call(imsg->u.release_comp.call_id, imsg->idb);
        if (!lv) {
            LANE_SIG_BUGINF(("no call"));
            return;
        }        
        /*
         * If multicast vc going away handle special case & exit.
         */
        if (lv->is_multicast) {
            LANE_SIG_BUGINF(("mc vcd %d %d",
                             lv->mc_head->vcd,
                             lv->mc_head->n_members));
            lane_sig_mc_release(lv->mc_head, ATM_RELEASE_COMPLETE);
            return;
        }
                  
        switch(lv->vcc_state) {
        case LANE_VCC_CONNECTED:
        case LANE_VCC_RELEASING:
        case LANE_VCC_DROPPING:
        case LANE_VCC_CONNECT_SENT:
            if (lv->is_multicast) {
                lane_sig_remove_from_mcgroup(lv, lv->mc_head);
                code = LANE_CALL_DROP_COMPLETE;
            } else {
                code = LANE_CALL_RELEASE_COMPLETE;
            }
            break;
        case LANE_VCC_SETUP_SENT:
            if (lv->is_multicast) {
                lane_sig_remove_from_mcgroup(lv, lv->mc_head);
                code =  LANE_CALL_ADD_FAILED;
            } else {
                code = LANE_CALL_CALL_FAILED;
            }
            break;
        default:
            errmsg(&msgsym(LANE_SIG_INV_MSG, LANE), ATM_RELEASE_COMPLETE,
                   lv, lv->vcc_state);
            return;
        }
        
        LANE_SIG_BUGINF(("%x %d %d", lv, lv->vcc_state, code));
        lane_remove_vcc_from_sig_chain(lv);
        (lv->callback)(lv, code);

        return;


    case ATM_ADD_PARTY:

        /* We never accept ADD_PARTY */

        LANE_SIG_BUGINF(("recv ADD_PARTY %x %x %x", imsg->idb,
                         imsg->u.add_party.call_id,
                         imsg->u.add_party.endpoint_id));
        omsg = malloc(sizeof(sig_api_msg));
        if (omsg) {
            omsg->opcode                      = ATM_ADD_PARTY_REJ;
            omsg->idb                         = imsg->idb;
            omsg->u.add_party_rej.call_id     = imsg->u.add_party.call_id;
            omsg->u.add_party_rej.endpoint_id = imsg->u.add_party.endpoint_id;
            omsg->u.add_party_rej.cause       = ATM_CALL_REJECTED;
            process_enqueue(atmSig_outputQ, omsg);
        }
        return;

        
    case ATM_ADD_PARTY_ACK:

        LANE_SIG_BUGINF(("recv ADD_PARTY_ACK %x %x", \
                         imsg->u.add_party_ack.call_id,
                         imsg->u.add_party_ack.endpoint_id));
        lv = lane_sig_find_endpoint(imsg->u.add_party_ack.call_id,
                                    imsg->u.add_party_ack.endpoint_id,
                                    imsg->idb);
        if (lv) {
            lv->mc_head->adds_out--;
            LANE_SIG_BUGINF(("%x %x %d", lv, lv->mc_head,
                             lv->mc_head->adds_out));
            lane_sig_do_deferred_adds(lv->mc_head);
        }
        switch(lv ? lv->vcc_state : LANE_VCC_NOT_FOUND) {
        case LANE_VCC_ADD_SENT:
            /* normal case, fall through to process ACK */
            break;
        case LANE_VCC_DROPPING:
            /* User asked for drop while ADD_PARTY out, do DROP now. */
            LANE_SIG_BUGINF(("do deferred DROP %x %d",
                             lv, lv->mc_head->adds_out));
            omsg = malloc(sizeof(sig_api_msg));
            if (omsg) {
                omsg->opcode                    = ATM_DROP_PARTY;
                omsg->idb                       = lv->swidb;
                omsg->u.drop_party.call_id      = lv->mc_head->call_id;
                omsg->u.drop_party.endpoint_id  = lv->call_id;
                omsg->u.drop_party.cause        = ATM_NORMAL_UNSPECIFIED;
                process_enqueue(atmSig_outputQ, omsg);
		lv->mc_head->drops_out++;
            }
            return;
        default:
            errmsg(&msgsym(LANE_SIG_INV_MSG, LANE), ATM_ADD_PARTY_ACK, lv,
                           lv ? lv->vcc_state :
                                (int)imsg->u.add_party_ack.endpoint_id);
            return;
        }
        lv->vcc_state = LANE_VCC_CONNECTED;
        (lv->callback)(lv, LANE_CALL_ADD_COMPLETE);

        return;
        
        
    case ATM_ADD_PARTY_REJ:

        lv = lane_sig_find_endpoint(imsg->u.add_party_rej.call_id,
                                    imsg->u.add_party_rej.endpoint_id,
                                    imsg->idb);
        if (lv) {
            LANE_SIG_BUGINF(("recv ADD_PARTY_REJ %x %x %d %d %d",
                             imsg->u.add_party_rej.call_id,
                             imsg->u.add_party_rej.endpoint_id,
                             imsg->u.add_party_rej.cause,
                             lv->vcc_state, lv->mc_head->adds_out));
            lv->mc_head->adds_out--;
            lane_sig_do_deferred_adds(lv->mc_head);
        }
        switch(lv ? lv->vcc_state : LANE_VCC_NOT_FOUND) {
        case LANE_VCC_ADD_SENT:
            /*
             * If we get a REJ with cause code == ATM_TOO_MANY_ADDS
             * or ATM_TEMP_FAILURE,
             * the switch most likely couldn't handle the add while
             * one or more other adds were out.  The add will probably
             * succeed after the other add(s) are done.  So, rather
             * than declare a failure, which forces a LANE client to 
             * drop all svc's and start over, we queue the add, to be
             * re-tried when other addD(s) are done.
             *
             * If we don't have any other adds out right now, we have
             * to give up.
             *
             * A queued add is one that's chained to the mcgroup, but
             * not in the signaling chain.
             */
            if (((imsg->u.add_party_rej.cause == ATM_TOO_MANY_ADDS) ||
                 (imsg->u.add_party_rej.cause == ATM_TEMP_FAILURE)) &&
                (lv->mc_head->adds_out > 0))
            {
                LANE_SIG_BUGINF(("defer"));
                lane_remove_vcc_from_sig_chain(lv);
                lv->vcc_state = LANE_VCC_ADD_SENT;
                return;
            }
            code = LANE_CALL_ADD_FAILED;
            break;
        case LANE_VCC_DROPPING:
            code = LANE_CALL_DROP_COMPLETE;
            break;
        default:
            errmsg(&msgsym(LANE_SIG_INV_MSG, LANE), ATM_ADD_PARTY_REJ, lv,
                   lv ? lv->vcc_state :
                        (int)imsg->u.add_party_rej.endpoint_id);
            return;
        }

        lane_remove_vcc_from_sig_chain(lv);
        lane_sig_remove_from_mcgroup(lv, lv->mc_head);
        (lv->callback)(lv, code);
        return;
        
        
    case ATM_DROP_PARTY:                /* very much like RELASE */
        
        LANE_SIG_BUGINF(("recv DROP_PARTY %x %x %d", \
                         imsg->u.drop_party.call_id,
                         imsg->u.drop_party.endpoint_id,
                         imsg->u.drop_party.cause));

        /* Always respond with DROP_ACK  */

        omsg = malloc(sizeof(sig_api_msg));
        if (omsg) {
            omsg->opcode                        = ATM_DROP_PARTY_ACK;
            omsg->idb                           = imsg->idb;
            omsg->u.drop_party_ack.call_id      = imsg->u.drop_party.call_id;
            omsg->u.drop_party_ack.endpoint_id  =
                imsg->u.drop_party.endpoint_id;
            omsg->u.drop_party_ack.cause        = ATM_NORMAL_UNSPECIFIED;
            LANE_SIG_BUGINF(("drop ack %d", omsg->u.drop_party_ack.cause));
            process_enqueue(atmSig_outputQ, omsg);
        }

        lv = lane_sig_find_endpoint(imsg->u.drop_party.call_id,
                                    imsg->u.drop_party.endpoint_id,
                                    imsg->idb);
        if (!lv) {
            LANE_SIG_BUGINF(("no endpoint"));
            return;
        }       

        switch(lv->vcc_state) {
        case LANE_VCC_CONNECTED:
            code = LANE_CALL_RELEASE_RECEIVED;
            break;
        case LANE_VCC_ADD_SENT:
            code = LANE_CALL_ADD_FAILED;
            break;
        case LANE_VCC_DROPPING:
            LANE_SIG_BUGINF(("already dropping"));
#if defined(RELEASE_COLLIDE_BUG)
	    if (--lv->mc_head->drops_out < 0) {
		lv->mc_head->drops_out = 0;
	    }
            code = LANE_CALL_DROP_COMPLETE;
#else
            return;
#endif
            break;
        default:
            errmsg(&msgsym(LANE_SIG_INV_MSG, LANE), ATM_DROP_PARTY, lv,
                   lv->vcc_state);
            return;
        }

        lane_sig_remove_from_mcgroup(lv, lv->mc_head);
        lane_remove_vcc_from_sig_chain(lv);
        (lv->callback)(lv, code);
        
        return;
        
        
    case ATM_DROP_PARTY_ACK:

        LANE_SIG_BUGINF(("recv DROP_PARTY_ACK %x %x %d", \
                         imsg->u.drop_party_ack.call_id,
                         imsg->u.drop_party_ack.endpoint_id,
                         imsg->u.drop_party_ack.cause));
        lv = lane_sig_find_endpoint(imsg->u.drop_party_ack.call_id,
                                imsg->u.drop_party_ack.endpoint_id,
                                imsg->idb);
        if (!lv) {
            LANE_SIG_BUGINF(("no endpoint"));
            return;
        }

        switch(lv->vcc_state) {
        case LANE_VCC_DROPPING:
	    if (--lv->mc_head->drops_out < 0)
		lv->mc_head->drops_out = 0;
	    /* fall thru */
        case LANE_VCC_CONNECTED:
            code = LANE_CALL_DROP_COMPLETE;
            break;
        case LANE_VCC_ADD_SENT:
            code = LANE_CALL_ADD_FAILED;
            break;
        default:
            errmsg(&msgsym(LANE_SIG_INV_MSG, LANE), ATM_DROP_PARTY_ACK, lv,
                   lv->vcc_state);
            return;
        }

        LANE_SIG_BUGINF(("%x %d %d %d", lv, lv->vcc_state, code,
                         lv->mc_head->drops_out));
        lane_sig_remove_from_mcgroup(lv, lv->mc_head);
        lane_remove_vcc_from_sig_chain(lv);
        (lv->callback)(lv, code);
        return;

    default:

        errmsg(&msgsym(LANE_SIG_INV_MSG, LANE), imsg->opcode, imsg->idb, 0);
        return;
        
        
    }
  
}

static sig_api_msg *
lane_sig_format_setup (lane_vcc_info_t *lv)
{
    sig_api_msg         *omsg;
    int                  n;
    unsigned             sdu_size;

    omsg = malloc(sizeof(sig_api_msg));
    if (omsg) {
        omsg->opcode                    = ATM_SETUP;
        omsg->idb                       = lv->swidb;
        omsg->u.setup.call_id           = atmSig_api_allocate_call_id();
        omsg->u.setup.sig_user_callback = lane_sig_api_callback;
        omsg->u.setup.called_party      = lv->called_addr;
        omsg->u.setup.calling_party     = lv->calling_addr;
        omsg->u.setup.flags             = ATM_VC_AAL5LANE;
        switch(lane_type_to_blli_pid[lv->vc_type]) {
        case LANE_BLLI_CONTROL:
            /* sdu size for control is always 1516. */
            sdu_size = LANE_SDU_SIZE_1516;
            break;
        default:
            /* sdu size for data depends on frame size for elan. */
            sdu_size = 
                lane_frame_to_sdu_size(lv->swidb->lane_info->max_data_frame);
            break;
        }
        omsg->u.setup.fwd_sdu_size =
        omsg->u.setup.bwd_sdu_size = sdu_size;
        if (lv->no_svc_timeout)
            omsg->u.setup.flags        |= (VC_FLAG_NOIDLETIME |
					   VC_FLAG_NOBUMP_CACHE);
        atm_params_init(&omsg->u.setup.params);
        lane_sig_make_blli(&omsg->u.setup.blli[0], lv->vc_type);
        if (lane_signal_debug) {
            lane_sig_buginf_addrs(lv->called_addr.addr, lv->calling_addr.addr);
            lane_sig_buginf_blli("sending", &omsg->u.setup.blli[0]);
        }
        for (n = 1; n < ATM_MAX_BLLI_NUM; n++)
            omsg->u.setup.blli[n].length  = 0;
        omsg->u.setup.user_vc_type      = lv->vc_type;

    }
    return omsg;
}


/*
 * lane_listen
 *
 * Post a listen on the specified nsap.
 */
lane_signal_return_t
lane_listen (lane_vcc_info_t    *tlv)
{
    sig_api_msg         *omsg;
    
    SET_LANE_SIG_BUGINF_IDB(tlv->swidb);

    /* Post the listen. */
    
    LANE_SIG_BUGINF(("listen %x  type %d", tlv, tlv->vc_type));
    omsg = malloc(sizeof(sig_api_msg));
    if (omsg) {
        omsg->opcode                     = ATM_LISTEN_NSAP;
        omsg->idb                        = tlv->swidb;
        omsg->u.listen.nsap              = tlv->called_addr;
        omsg->u.listen.sig_user_callback = lane_sig_api_callback;
        omsg->u.listen.user_handle       = tlv;
        process_enqueue(atmSig_outputQ, omsg);
    } else {
        return LANE_SIG_FAILURE;
    }
    
    tlv->svc            = TRUE;
    tlv->vcc_state      = LANE_VCC_LISTEN_SENT;
    tlv->in_sig_chain   = FALSE;
    tlv->is_multicast   = FALSE;
    lane_add_vcc_to_sig_chain(tlv);

    return LANE_SIG_CALLBACK;
  
}

/*
 * lane_no_listen
 *
 * Cancel listen for specified nsap.
 *
 * Note that the listen is not cancelled immediately (atmSig_handleUserRequest
 * just queue's a message for later processing), so there's a possibility that
 * a SETUP might appear at lane_sig_api_callback after we've done this.  But,
 * in that case, the lane_vcc isn't in our chain anymore, so our callback
 * routine will just bounce the SETUP with a RELEASE_COMPLETE.  No harm,
 * no foul.
 */
lane_signal_return_t
lane_no_listen (lane_vcc_info_t *lv)
{
    sig_api_msg         *omsg;

    SET_LANE_SIG_BUGINF_IDB(lv->swidb);
    omsg = malloc(sizeof(sig_api_msg));
    if (omsg) {
        LANE_SIG_BUGINF(("cancel listen  type %d", lv->vc_type));
        omsg->opcode               = ATM_CANCEL_NSAP;
        omsg->idb                  = lv->swidb;
        omsg->u.cancel.nsap        = lv->called_addr;
        omsg->u.cancel.user_handle = lv;
        process_enqueue(atmSig_outputQ, omsg);
        lv->vcc_state = LANE_VCC_CANCEL_SENT;
        return LANE_SIG_CALLBACK;
    } else {
        return LANE_SIG_FAILURE;
    }
}

/*
 * lane_release
 *
 * terminate a call.
 */
lane_signal_return_t
lane_release (lane_vcc_info_t *lv)
{
    sig_api_msg         *omsg;

    SET_LANE_SIG_BUGINF_IDB(lv->swidb);
    LANE_SIG_BUGINF(("release %x  type %d,  vcd %d", \
                     lv, lv->vc_type, lv->vcd));
   
    switch(lv->vcc_state) {
        
    case LANE_VCC_INITIALIZED:
	return LANE_SIG_SUCCESS;

    case LANE_VCC_NOT_FOUND:
    case LANE_VCC_LISTEN_SENT:
    case LANE_VCC_LISTENING:
    case LANE_VCC_CANCEL_SENT:
    case LANE_VCC_ADD_SENT:
    case LANE_VCC_DROPPING:
    case LANE_VCC_SETUP_RECEIVED:
        errmsg(&msgsym(LANE_SIG_INV_STATE, LANE),
               "release", lv->vcc_state, lv);
        return LANE_SIG_FAILURE;

    /* already releasing */
    case LANE_VCC_RELEASING:
	return LANE_SIG_CALLBACK;

    case LANE_VCC_SETUP_SENT:
        lv->vcc_state = LANE_VCC_RELEASING;
        LANE_SIG_BUGINF(("waiting for SETUP resp"));
        return LANE_SIG_CALLBACK;

    case LANE_VCC_CONNECT_SENT:
    case LANE_VCC_CONNECTED:
        break;          /* All other states ok */
    }

    omsg = malloc(sizeof(sig_api_msg));
    if (omsg) {
	omsg->opcode            = ATM_RELEASE;
	omsg->idb               = lv->swidb;
	omsg->u.release.call_id = lv->call_id;
	omsg->u.release.cause   = ATM_NORMAL_UNSPECIFIED;
	lv->vcc_state           = LANE_VCC_RELEASING;
	process_enqueue(atmSig_outputQ, omsg);
	LANE_SIG_BUGINF(("RELEASE %x", lv->call_id));
	return LANE_SIG_CALLBACK;
    } else {
	return LANE_SIG_FAILURE;
    }

}

lane_signal_return_t
lane_create_mcgroup (lane_signal_mcgroup_t *mcg, lane_info_t * info)
{
    SET_LANE_SIG_BUGINF_IDB(info->swidb);

    LANE_SIG_BUGINF(("create mcgroup %x, type %d", mcg, mcg->vc_type));
    mcg->mcgroup_state = LANE_VCC_CONNECTED;
    mcg->root_chain    = NULL;
    mcg->n_members     = 0;
    mcg->adds_out      = 0;
    mcg->lane_info     = info;
    mcg->vcd           = 0;
    mcg->svc           = TRUE;
    return LANE_SIG_SUCCESS;
}

lane_signal_return_t
lane_delete_mcgroup (lane_signal_mcgroup_t *mcg)
{
    SET_LANE_SIG_BUGINF_IDB(mcg->lane_info ? mcg->lane_info->swidb : NULL);

    LANE_SIG_BUGINF(("delete mcgroup %x", mcg));
    if ((mcg->n_members != 0) || (mcg->root_chain != NULL)) {
        errmsg(&msgsym(LANE_SIG_ERR, LANE), "delete non-empty mcgroup"); 
        return LANE_SIG_FAILURE;
    }
    mcg->mcgroup_state = LANE_VCC_INITIALIZED;
    mcg->svc           = FALSE;
    return LANE_SIG_SUCCESS;
}

lane_signal_return_t
lane_add (lane_signal_mcgroup_t *mcg, lane_vcc_info_t *lv)
{
    lane_info_t         *info;
    sig_api_msg         *omsg;

    SET_LANE_SIG_BUGINF_IDB(lv->swidb);

    info = lv->swidb->lane_info;
    lv->is_multicast = TRUE;

    LANE_SIG_BUGINF(("add party %x %x", mcg, lv));

    if (mcg->n_members <= 0) {
       
        /* First party on this group */
	mcg->vcd = 0;

        LANE_SIG_BUGINF(("first party type %d", lv->vc_type));
        omsg = lane_sig_format_setup(lv);
        if (omsg) {
            omsg->u.setup.multipoint = TRUE;
            lv->call_id              = 0;
            lv->svc                  = TRUE;
            lv->vcc_state            = LANE_VCC_SETUP_SENT;
            mcg->call_id             = omsg->u.setup.call_id;
            LANE_SIG_BUGINF(("SETUP %x", mcg->call_id));
        } else {
            return LANE_SIG_FAILURE;
        }

    } else {
        
        /* additional party on this group.  */

        /*
         * If vcd is zero in mcgroup, SETUP for 1st party must still
         * be outstanding.  UNI doesn't allow ADD_PARTY until 1st party
         * is active, so don't try it now.  All deferred ADD_PARTY's
         * will be sent when CONNECT for 1st party comes back.  Setting
         * state to ADD_SENT flags this vcc_info as one that needs an
         * ADD_PARTY sent.
         *
         * If the SETUP fails (RELEASE_COMPLETE instead of CONNECT), all
         * the deferred ADD's will be correctly handled by
         * lane_sig_mc_release().
         */
        if (mcg->vcd == 0) {
            lv->vcc_state  = LANE_VCC_ADD_SENT;
            lv->in_sig_chain = FALSE;           /* being paranoid */
            lane_sig_add_to_mcgroup(lv, mcg);
            LANE_SIG_BUGINF(("defer ADD_PARTY"));
            return LANE_SIG_CALLBACK;
        }
        omsg = malloc(sizeof(sig_api_msg));
        if (omsg) {
            omsg->opcode                    = ATM_ADD_PARTY;
            omsg->idb                       = lv->swidb;
            omsg->u.add_party.call_id       = mcg->call_id;
            omsg->u.add_party.endpoint_id   =
                atmSig_api_allocate_endpoint_id(mcg->call_id);
            omsg->u.add_party.called_party  = lv->called_addr;
            if (lane_signal_debug)
                lane_sig_buginf_addrs(lv->called_addr.addr, NULL);
            lv->call_id    = omsg->u.add_party.endpoint_id;
            lv->vcd        = mcg->vcd;
            lv->svc        = TRUE;
            lv->vcc_state  = LANE_VCC_ADD_SENT;
            mcg->adds_out++;
            LANE_SIG_BUGINF(("ADD_PARTY %x %x vcd %d, %d %d",
                             mcg->call_id, lv->call_id, lv->vcd,
                             mcg->n_members, mcg->adds_out));
        } else {
            return LANE_SIG_FAILURE;
        }
   
    }
    
    process_enqueue(atmSig_outputQ, omsg);
    lane_sig_add_to_mcgroup(lv, mcg);
    lane_add_vcc_to_sig_chain(lv);
    return LANE_SIG_CALLBACK;
    
}

lane_signal_return_t
lane_drop (lane_signal_mcgroup_t *mcg, lane_vcc_info_t *lv)
{
    sig_api_msg         *omsg;

    SET_LANE_SIG_BUGINF_IDB(lv->swidb);

    switch(lv->vcc_state) {
        
    case LANE_VCC_INITIALIZED:
	return LANE_SIG_SUCCESS;

    case LANE_VCC_LISTEN_SENT:
    case LANE_VCC_LISTENING:
    case LANE_VCC_CANCEL_SENT:
    case LANE_VCC_RELEASING:
    case LANE_VCC_SETUP_RECEIVED:
    case LANE_VCC_NOT_FOUND:
        errmsg(&msgsym(LANE_SIG_INV_STATE, LANE),
               "drop", lv->vcc_state, lv);
        return LANE_SIG_FAILURE;

    /* already dropping */
    case LANE_VCC_DROPPING:
	return LANE_SIG_CALLBACK;

    case LANE_VCC_ADD_SENT:
    case LANE_VCC_SETUP_SENT:
        LANE_SIG_BUGINF(("drop %x with ADD/SETUP out %d",
                         lv, lv->vcc_state));
        lv->vcc_state = LANE_VCC_DROPPING;
        return LANE_SIG_CALLBACK;

    case LANE_VCC_CONNECT_SENT:
    case LANE_VCC_CONNECTED:
        break;          /* All other states ok */
    }
        
    LANE_SIG_BUGINF(("drop %x %x %x %x vcd %d, %d mem %d drops", \
                     lv, mcg, mcg->call_id, lv->call_id,
                     mcg->vcd, mcg->n_members, mcg->drops_out));

    omsg = malloc(sizeof(sig_api_msg));
    if (omsg) { 
        if ((mcg->n_members - mcg->drops_out) <= 1) {
            omsg->opcode            = ATM_RELEASE;
            omsg->u.release.call_id = mcg->call_id;
            omsg->u.release.cause   = ATM_NORMAL_UNSPECIFIED;
            LANE_SIG_BUGINF(("RELEASE"));
        } else {
            omsg->opcode                    = ATM_DROP_PARTY;
            omsg->u.drop_party.call_id      = mcg->call_id;
            omsg->u.drop_party.endpoint_id  = lv->call_id;
            omsg->u.drop_party.cause        = ATM_NORMAL_UNSPECIFIED;
            mcg->drops_out++;
            LANE_SIG_BUGINF(("DROP %d", mcg->drops_out));
        }
        /*
         * We set state to DROPPING even though it may really be RELEASING.
         * This is because users of this code (like lane_server) expect
         * to see DROPPING state after calling lane_drop().
         */
        omsg->idb     = lv->swidb;
        lv->vcc_state = LANE_VCC_DROPPING;
        process_enqueue(atmSig_outputQ, omsg);
        return LANE_SIG_CALLBACK;      
    } else {
        return LANE_SIG_FAILURE;
    }
}
