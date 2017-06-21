/* $Id: dlsw_local.c,v 3.6.42.4 1996/07/19 03:55:27 athippes Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_local.c,v $
 *------------------------------------------------------------------
 * DLSw Local Switching Support File
 * This file contains routines that compliment the DLSw local
 * switching state machine. 
 *
 * August 1995, Frank Bordonaro
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dlsw_local.c,v $
 * Revision 3.6.42.4  1996/07/19  03:55:27  athippes
 * CSCdi60758:  dlsw : duplicate circuit identifier
 * Branch: California_branch
 *
 * Revision 3.6.42.3  1996/05/30  14:43:37  sbales
 * CSCdi59064:  DLSw+: show commands need to be more usable
 *              modify options for "sh dls reach", "sh dls cir", "sh dls
 *              loc", modify formatting for "show dls peer"
 * Branch: California_branch
 *
 * Revision 3.6.42.2  1996/05/17  10:45:51  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.30.2  1996/04/26  07:34:23  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6.30.1  1996/04/03  14:01:38  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.42.1  1996/03/18  19:32:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.12.2  1996/03/07  08:44:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.12.1  1996/02/20  13:46:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1995/12/31  18:17:00  fbordona
 * CSCdi46267:  DLSw sdlc secondary multidrop PUs do not connect.
 *
 * Revision 3.5  1995/12/12  17:37:36  ravip
 * CSCdi44808:  crash in csm_clsi_handler
 *
 * Revision 3.4  1995/12/09  16:49:08  fbordona
 * CSCdi45326:  DLSw+ ignores DISC.Ind in some states
 *              Make DLSw+ call CloseStn.Req with u_cep_id
 *              if ReqOpnStn.Req times out.
 *              Fix crash caused by "dlsw disable".
 *
 * Revision 3.3  1995/11/20  22:44:55  snyder
 * CSCdi44202:  spellink errors
 *              frmae -> frame
 *              inactivty -> inactivity
 *
 * Revision 3.2  1995/11/17  09:03:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:24:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:55:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/24  18:48:52  fbordona
 * Placeholder for new file
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "address.h"
#include "mgd_timers.h"
#include "ieee.h"
#include "../if/rif_util.h"
#include "../if/rif.h"
#include "../cls/clsi_msg.h"
#include "../cls/clsi_queue.h"
#include "../ui/debug.h"
#include "../ibm/libibm_externs.h"

#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_debug.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_local_private.h"
#include "../dlsw/dlsw_dlu.h"

/*
 * Global Data
 */
queuetype dl_hashQ[DL_HASH_SIZE];
queuetype dl_deadQ;

/*
 * File Scope Variables
 */
static mgd_timer dl_master_timer; 
static boolean local_debug_all = FALSE;

/* 
 * File Scope Function Prototypes
 */
static dl_ckt_t *dl_create_ckt (dlsw_ring_t *, reach_entry_t *, TCLSIMsg  *,
                                uchar *, uchar *, uchar, uchar);
static dl_ckt_t *dl_find_ckt (uchar *, uchar *, uchar, uchar);

/*
 * dlsw_local_init()
 */

void dlsw_local_init (void)
{
    ulong ii;

    mgd_timer_init_parent(&dl_master_timer, NULL);

    queue_init(&dl_deadQ, 0);
    for (ii = 0; ii < DL_HASH_SIZE; ii++) {
        queue_init(&dl_hashQ[ii], 0);
    }
}


/*
 * dlsw_local_background()
 *
 * Called every 1 second by dlsw_background() in dlsw_core.c
 */

void dlsw_local_background (void)
{
    dl_ckt_t *dl;
    dl_cep_t *dc;
    mgd_timer *expired_timer;

    while (mgd_timer_expired(&dl_master_timer)) {
        expired_timer = mgd_timer_first_expired(&dl_master_timer);
        dc = mgd_timer_context(expired_timer);
        if ((dc) && (dc->id == DL_MAGIC_ID)) {
            LDEBUG("\nDLSw: local circuit (%x) timer expired", dc);
            dl_fsm(dc, dle_timer, dle_timer_expired);
        }
    }

    if (dl_deadQ.qhead) {
        while ((dl = p_dequeue(&dl_deadQ))) {
            if (dl->cep_a.pending_clsi_msg)
                CLSIMsgDestroy(dl->cep_a.pending_clsi_msg);
            if (dl->cep_b.pending_clsi_msg)
                CLSIMsgDestroy(dl->cep_a.pending_clsi_msg);
            free(dl);
        }
    }
}


static uchar dl_get_hash_bucket (uchar *mac_a, uchar *mac_b,
                                 uchar sap_a, uchar sap_b)
{
   uchar ii, n = 0;

   for (ii = 0; ii < IEEEBYTES; ii++) {
       n += mac_a[ii];
       n += mac_b[ii];
   }
   n += sap_a;
   n += sap_b;
   n &= DL_HASH_MASK;
   return (n);
}


boolean dlsw_is_local_ckt (TCLSIMsg* clsi_msg)
{
    dl_cep_t *dc;
    ulong clsi_cnf_ret_code;
    ushort cls_op;
   
    dc = (dl_cep_t *) clsi_msg->fIdentifier;
    if ((dc == NULL) || (dc->id != DL_MAGIC_ID))
        return(FALSE);

    cls_op = CLSIMsgGetPrimitiveCode(clsi_msg);
    /*
     * For Data frames, Bypass local fsm and lan modules
     * if not doing biu segmentation.
     */
    if ((cls_op == (CLS_DATA | CLS_INDICATION)) && 
        (dlsw_sna_spoof_and_seg() == FALSE)) {
        LDEBUG("\nDLSw: L-CKT: (%x) fast i-frame proc", dc);
        CLSIMsgSetPrimitiveType(clsi_msg, CLS_REQUEST);
        clsi_msg->fIdentifierType = CLS_P_CEP_ID;
        clsi_msg->fIdentifier = dc->partner->p_cep_id;
    
        CLSIQueuePut(gDLSW2CLSWriteQ, clsi_msg);
        return(TRUE);
    }

    if ((cls_op & CLS_TYPE_MASK) == CLS_CONFIRM) {
        clsi_cnf_ret_code = CLSIMsgGetReturnCode(clsi_msg);
        if (clsi_cnf_ret_code != CLS_OK) {
            /*
             * Ignore XID response timeouts.
             */
            if (clsi_cnf_ret_code != CLS_NO_XID_RSP) {
                dl_fsm(dc, dle_admin, dle_admin_dlc_err); 
            }
            CLSIMsgDestroy(clsi_msg);
            return(TRUE);
        }
    }

    dc->clsi_msg = clsi_msg;
    dl_fsm(dc, dle_dlc, cls_op);
    if (dc->clsi_msg) {
        CLSIMsgDestroy(dc->clsi_msg);
        dc->clsi_msg = NULL;
    }
    return(TRUE);
}


static dl_ckt_t *dl_find_ckt (uchar *mac_a, uchar *mac_b,
                              uchar sap_a, uchar sap_b)
{
    dl_ckt_t *dl;
    uchar bucket;

    bucket = dl_get_hash_bucket(mac_a, mac_b, sap_a, sap_b);
    dl = (dl_ckt_t *) dl_hashQ[bucket].qhead;
    for (; dl; dl = dl->next) {
        if (!bcmp(dl->cep_a.mac, mac_a, IEEEBYTES)) {
            if ((!bcmp(dl->cep_b.mac, mac_b, IEEEBYTES)) &&
                (dl->cep_a.sap == sap_a) && (dl->cep_b.sap == sap_b)) {
                return(dl);
            }
        } else if (!bcmp(dl->cep_a.mac, mac_b, IEEEBYTES)) {
            if ((!bcmp(dl->cep_b.mac, mac_a, IEEEBYTES)) &&
                (dl->cep_a.sap == sap_b) && (dl->cep_b.sap == sap_a)) {
                return(dl);
            }
        }
    }
    return(NULL);
}


static dl_ckt_t *dl_create_ckt (dlsw_ring_t *src_port, reach_entry_t *dst_re,
                                TCLSIMsg  *src_clsi_msg, uchar *saddr,
                                uchar *daddr, uchar ssap, uchar dsap)
{
    dl_ckt_t *dl;
    uchar src_rif[SRB_MAXRIF];
    int   rif_rc;

    dl = malloc_named(sizeof(dl_ckt_t), "DLSW-LOCAL-CIRCUIT");
    if (!dl)
        return(NULL);

    /*
     * Init fields.
     * From the destination reach entry, get dest station's
     * rif and lan port id. From the clsi msg, get macs/saps 
     * and the src station's rif. 
     */
    rif_rc = get_rif_from_clsi(src_clsi_msg, src_rif);
    if (rif_rc < 0) {
        return (NULL);
    }

    dl->next = NULL;
    dl->bucket = dl_get_hash_bucket(saddr, daddr, ssap, dsap);
    dl->dl_ckt_id = (uint)dl;
    bcopy(saddr, dl->cep_a.mac, IEEEBYTES);
    bcopy(daddr, dl->cep_b.mac, IEEEBYTES);
    dl->cep_a.sap = ssap;
    dl->cep_b.sap = dsap;

    /*
     * Cep A Init.
     */ 
    dl->cep_a.p_cep_id = 0;
    dl->cep_a.id = DL_MAGIC_ID;
    dl->cep_a.parent = (void *) dl;
    dl->cep_a.partner = (dl_cep_t *)
                        (((uchar *) dl) + offsetof(dl_ckt_t, cep_b));
    dl->cep_a.state = dlst_disconnected;
    dl->cep_a.xid_state = dl_xid_reset;
    dl->cep_a.port_id = src_port;
    dl->cep_a.clsi_msg = NULL;
    dl->cep_a.pending_clsi_msg = src_clsi_msg;
    mgd_timer_init_leaf(&dl->cep_a.timer,
                        &dl_master_timer, 0, &dl->cep_a, FALSE);
    mgd_timer_stop(&dl->cep_a.timer);
    dl->cep_a.rif_len = src_rif[0] & RC_LTHMASK;
    bcopy(src_rif, dl->cep_a.rif, dl->cep_a.rif_len);
    dl->cep_a.rif[1] ^= RC_D; /* flip rif direction learned from input frame */
    dl->cep_a.max_i_field_len = 0;
    dl->cep_a.window_size = 0;
    dl->cep_a.abm_state = DLSW_ABM_UNKNOWN;
    dl->cep_a.debug = local_debug_all;

    /*
     * Cep B Init.
     */ 
    dl->cep_b.p_cep_id = 0;
    dl->cep_b.id = DL_MAGIC_ID;
    dl->cep_b.parent = (void *) dl;
    dl->cep_b.partner = (dl_cep_t *)
                        (((uchar *) dl) + offsetof(dl_ckt_t, cep_a));
    dl->cep_b.state = dlst_disconnected;
    dl->cep_b.xid_state = dl_xid_reset;
    dl->cep_b.port_id = dst_re->prp_id.port_id;
    dl->cep_b.clsi_msg = NULL;
    dl->cep_b.pending_clsi_msg = NULL;
    mgd_timer_init_leaf(&dl->cep_b.timer,
                        &dl_master_timer, 0, &dl->cep_b, FALSE);
    mgd_timer_stop(&dl->cep_b.timer);
    dl->cep_b.rif_len = dst_re->rif[0] & RC_LTHMASK;
    bcopy(dst_re->rif, dl->cep_b.rif, dl->cep_b.rif_len);
    dl->cep_b.max_i_field_len = 0;
    dl->cep_b.window_size = 0;
    dl->cep_b.abm_state = DLSW_ABM_UNKNOWN;
    dl->cep_b.debug = local_debug_all;

    p_enqueue(&dl_hashQ[dl->bucket], dl);
    return(dl);
}


/*
 * csm_to_local()
 *
 * Consumes src_clsi_msg.
 */

void csm_to_local (dlsw_ring_t *src_port, reach_entry_t *dst_re,
                   TCLSIMsg  *src_clsi_msg)
{
    dl_ckt_t *dl;
    uchar *saddr, *daddr;
    uchar ssap, dsap;

    if ((dlsw_disable) || (!dlsw_peer_active(0,0,0))) {
        CLSIMsgDestroy(src_clsi_msg);
        return;
    }

    daddr = get_daddr_from_clsi(src_clsi_msg);
    saddr = get_saddr_from_clsi(src_clsi_msg);
    dsap = get_dsap_from_clsi(src_clsi_msg);
    ssap = get_ssap_from_clsi(src_clsi_msg);

    if (local_debug_all)
        buginf("\nDLSw: csm_to_local(): %s-->%s  %e:%x->%e:%x",
               src_port->swidb->hwptr->hw_namestring,
               dst_re->prp_id.port_id->swidb->hwptr->hw_namestring,
               saddr, ssap, daddr, dsap);

    dl = dl_find_ckt(saddr, daddr, ssap, dsap);
    if (dl) {
        /*
         * The previous circuit for this mac/sap pair has
         * not cleaned up yet. Encourage the current circuit
         * to go away. Drop the clsi msg.
         */
        if (local_debug_all)
            buginf("\nDLSw: csm_to_local(): ckt already exists %e:%x->%e:%x",
                   saddr, ssap, daddr, dsap);
        dl_fsm(&dl->cep_b, dle_admin, dle_admin_stop);
        dl_fsm(&dl->cep_a, dle_admin, dle_admin_stop);
        CLSIMsgDestroy(src_clsi_msg);
    } else {
        /*
         * dl_create_ckt() will save src_clsi_msg in cep_a.
         * When ReqOpnStn.Cnf is received, it will be processed.
         */
        dl = dl_create_ckt(src_port, dst_re, src_clsi_msg,
                           saddr, daddr, ssap, dsap);
        if (dl) {
            dl_fsm(&dl->cep_b, dle_admin, dle_admin_start);
            dl_fsm(&dl->cep_a, dle_admin, dle_admin_start);
        } else {
            CLSIMsgDestroy(src_clsi_msg);
        }
    }
}


void dlsw_local_disable ()
{
    dl_ckt_t *dl;
    ulong ii;

    for (ii = 0; ii < DL_HASH_SIZE; ii++) {
        dl = (dl_ckt_t *) dl_hashQ[ii].qhead;
        for (; dl; dl = dl->next) {
            dl_fsm(&dl->cep_b, dle_admin, dle_admin_stop);
            dl_fsm(&dl->cep_a, dle_admin, dle_admin_stop);
        }
    }
}


static dl_ckt_t *validate_id(int given_ckt)
{
   dl_ckt_t *dl;
   ulong ii;

   for (ii = 0; ii < DL_HASH_SIZE; ii++) {
        dl = (dl_ckt_t *) dl_hashQ[ii].qhead;
        for (; dl; dl = dl->next) {
           if (dl->dl_ckt_id == given_ckt)
              return(dl);
        }
    }
    return NULL;
}


/*
 * local_debug()
 */

void local_debug (boolean printit, boolean sense, int major)
{
    dl_ckt_t *dl;
    ulong ii;

    if (major == -1) {
        local_debug_all = sense;
        if (printit)
            printf("\nDLSw local circuit debugging is %s", sense ? "on" : "off");

        for (ii = 0; ii < DL_HASH_SIZE; ii++) {
            dl = (dl_ckt_t *) dl_hashQ[ii].qhead;
            for (; dl; dl = dl->next) {
                dl->cep_a.debug = sense;
                dl->cep_b.debug = sense;
            }
        }
    } else {
        dl = validate_id(major);
        if (dl) {
            dl->cep_a.debug = sense;
            dl->cep_b.debug = sense;
            if (printit)
                printf("\nDLSw Local Circuit debugging is %s for %u",
                       sense ? "on" : "off", major);
        }
    } /* Specific ckt */
}


static void print_ckt (dl_ckt_t *dl, boolean banner)
{
    char            *rif_str;

    if (banner)
        printf("\n key         mac-addr    sap    state        port \trif");

    rif_str = string_getnext();
    if (rif_str == NULL)
        return;

    if (RIF_LENGTH(dl->cep_a.rif[0]))
        rif_str = get_rif_str(dl->cep_a.rif, rif_str);
    else
        rif_str = "--no rif--";

    printf("\n%u   %e %02x  %s  %s \t%s",
           dl->dl_ckt_id,
           dl->cep_a.mac, dl->cep_a.sap, dl_print_state(dl->cep_a.state), 
           dl->cep_a.port_id->swidb->hwptr->hw_short_namestring, rif_str);

    printf("\n                 PCEP: %x  UCEP: %x",
           dl->cep_a.p_cep_id, (uint)dl);

    rif_str = string_getnext();
    if (rif_str == NULL)
        return;

    if (RIF_LENGTH(dl->cep_b.rif[0]))
        rif_str = get_rif_str(dl->cep_b.rif, rif_str);
    else
        rif_str = "--no rif--";

    printf("\n          %e %02x  %s  %s \t%s",
           dl->cep_b.mac, dl->cep_b.sap, dl_print_state(dl->cep_b.state), 
           dl->cep_b.port_id->swidb->hwptr->hw_short_namestring, rif_str);

    printf("\n                 PCEP: %x  UCEP: %x\n",
           dl->cep_b.p_cep_id, (uint)dl);
}


/*
 * dlsw_show_local_ckt()
 *
 * If major is  not -1, then user wants to display a specific circuit.
 * Major is the Ckt ID.
 */

void dlsw_show_local_ckt (parseinfo *csb)
{
    dl_ckt_t *dl;
    ulong ii;
    boolean first = TRUE;

    if (GETOBJ(int,5) != DLSW_SHOW_CIRCUIT_KEY) {
        for (ii = 0; ii < DL_HASH_SIZE; ii++) {
            dl = (dl_ckt_t *) dl_hashQ[ii].qhead;
            for (; dl; dl = dl->next) {

                /* Print Circuit info if:
                 *  - all circuits have been requested
                 *  - either mac address matches requested mac address, or
                 *  - either sap address matches requested sap address */
                if (GETOBJ(int,5) == DLSW_SHOW_CIRCUIT_ALL ||
                    (GETOBJ(int,5) == DLSW_SHOW_CIRCUIT_MAC && 
                     ((!bcmp(GETOBJ(hwaddr,3)->addr, dl->cep_a.mac, 
                           IEEEBYTES)) ||
                      (!bcmp(GETOBJ(hwaddr,3)->addr, dl->cep_b.mac, 
                           IEEEBYTES)))) ||
                    (GETOBJ(int,5) == DLSW_SHOW_CIRCUIT_SAP && 
                     ((GETOBJ(int,3) == dl->cep_a.sap) ||
                      (GETOBJ(int,3) == dl->cep_b.sap) ))) {
                    print_ckt(dl, first);
                    first = FALSE;
		}
            }
        }
    } else {
        dl = validate_id(GETOBJ(int,3));
        if (dl)
                print_ckt(dl, TRUE);
    } /* Specific ckt */
}


/*
 * dlsw_clear_local_ckt()
 *
 * If major is not -1, then user wants to clear a specific circuit.
 * Major is the Ckt ID.
 */

void dlsw_clear_local_ckt (int major)
{
    dl_ckt_t *dl;

    if (major == -1) {
        dlsw_local_disable();
    } else {
        dl = validate_id(major); 
        if (dl) {
            dl_fsm(&dl->cep_b, dle_admin, dle_admin_stop);
            dl_fsm(&dl->cep_a, dle_admin, dle_admin_stop);
        }
    } /* Specific Ckt */
}


void show_local_debug ()
{
    dl_ckt_t *dl;
    ulong ii;

    if (local_debug_all) {
        printf("\n  DLSw Local Circuit debugging is on");
    } else {
        for (ii = 0; ii < DL_HASH_SIZE; ii++) {
            dl = (dl_ckt_t *) dl_hashQ[ii].qhead;
            for (; dl; dl = dl->next) {
                if (dl->cep_a.debug)
                    printf("\n  DLSw Local Circuit debugging is on for %u",
                           dl->dl_ckt_id);
            }
        }
    }
}

