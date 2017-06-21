/* $Id: dlsw_core_debug.c,v 3.4.20.7 1996/09/11 17:38:44 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_core_debug.c,v $
 *------------------------------------------------------------------
 * All the core specific debug flags are set/cleared in here
 * DLSw subsystem 
 *
 * Fri July 29, 1994 G. Pandian
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Acts as a demuxer for various core specific debug flags
 *------------------------------------------------------------------
 * $Log: dlsw_core_debug.c,v $
 * Revision 3.4.20.7  1996/09/11  17:38:44  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.4.20.6  1996/07/25  09:43:11  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.4.20.5  1996/07/19  03:55:14  athippes
 * CSCdi60758:  dlsw : duplicate circuit identifier
 * Branch: California_branch
 *
 * Revision 3.4.20.4  1996/07/17  23:25:06  ravip
 * CSCdi62619:  Router crash after issue a show dlsw circuits following
 * micro reload
 * Branch: California_branch
 *
 * Revision 3.4.20.3  1996/05/30  14:43:23  sbales
 * CSCdi59064:  DLSw+: show commands need to be more usable
 *              modify options for "sh dls reach", "sh dls cir", "sh dls
 *              loc", modify formatting for "show dls peer"
 * Branch: California_branch
 *
 * Revision 3.4.20.2  1996/05/17  10:44:50  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.6.2  1996/04/26  07:33:50  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4.6.1  1996/04/03  14:00:57  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.20.1  1996/03/18  19:31:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.2  1996/03/07  08:44:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.20.1  1996/02/20  00:45:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/01  06:01:04  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/11/29  19:44:20  ravip
 * CSCdi40989:  crash: router tried to reference free memory in dlsw code
 *              use mem_lock() to lock memory, to avoid other routines
 *              from deleting the memory block in use.
 *              Use a flag to tag actions that would change the links.
 *
 * Revision 3.2  1995/11/17  09:02:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:22:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:54:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:26:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "ieee.h"
#include "address.h" /* Defines hwaddrtype for clsi_msg.h    */
#include "subsys.h"
#include "mgd_timers.h"
#include "sys_registry.h"
#include "../ui/debug.h"

#include "../if/network.h"
#include "../if/tring.h"
#include "../if/if_vidb.h"
#include "../ui/debug.h"
#include "../cls/clsi_msg.h"
#include "../ibm/netbios.h"
#include "../ibm/libibm_externs.h"
#include "../if/rif_util.h"
#include "../ibm/sdlc.h"
#include "../util/fsm.h"
#include "../srt/srt_registry.h"
#include "../cls/dlc_registry.h"
 
#include "../dlsw/parser_defs_dlsw.h"
#include "../dlsw/dlsw_core_debug.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_core_externs.h"
#include "../dlsw/dlsw_core_fsm.h"
#include "../dlsw/dlsw_dlu.h"
#include "../dlsw/dlsw_debug.h"


/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "dlsw_core_debug_flags.h"


/*
 * dlsw_core_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the DLSW debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

void dlsw_core_debug_all (boolean flag)
{
    int ii;
    dlsw_t *dlsw_ptr;

    generic_debug_all(dlsw_core_debug_arr, flag);

    for (ii = 0; ii < NDLSW_HASH_SIZE; ii++) {
        dlsw_ptr = (dlsw_t *) dlsw_hashQ[ii].qhead;
        for (; dlsw_ptr; dlsw_ptr = dlsw_ptr->next) {
            dlsw_ptr->dlsw_core_debug_msgs = flag;
            dlsw_ptr->dlsw_core_debug_state = flag;
            dlsw_ptr->dlsw_core_debug_flow = flag;
            dlsw_ptr->dlsw_core_debug_xid = flag;
        }
    }
}

/*
 * show_core_debug is called to display the values of all the DLSW
 * debugging variables.
 */

void show_core_debug ()
{
    dlsw_t *dlsw_ptr;
    ulong ii;

    /* 
     * Debug Messages
     */
    if (dlsw_core_debug_msgs) {
        printf("\nDLSw core message debugging is on");
    } else {
        for (ii = 0; ii < NDLSW_HASH_SIZE; ii++) {
            dlsw_ptr = (dlsw_t *) dlsw_hashQ[ii].qhead;
            for (; dlsw_ptr; dlsw_ptr = dlsw_ptr->next) {
                if (dlsw_ptr->dlsw_core_debug_msgs)
                    printf("\nDLSw core message debugging is on for %u",
                           dlsw_ptr->our_dlc_corr);
            }
        }
    }

    /* 
     * Debug States
     */
    if (dlsw_core_debug_state) {
        printf("\nDLSw core state debugging is on");
    } else {
        for (ii = 0; ii < NDLSW_HASH_SIZE; ii++) {
            dlsw_ptr = (dlsw_t *) dlsw_hashQ[ii].qhead;
            for (; dlsw_ptr; dlsw_ptr = dlsw_ptr->next) {
                if (dlsw_ptr->dlsw_core_debug_state)  
                    printf("\nDLSw core state debugging is on for %u",
                           dlsw_ptr->our_dlc_corr);
            }
        }
    }

    /* 
     * Debug Flow Control
     */
    if (dlsw_core_debug_flow) {
        printf("\nDLSw core flow control debugging is on");
    } else {
        for (ii = 0; ii < NDLSW_HASH_SIZE; ii++) {
            dlsw_ptr = (dlsw_t *) dlsw_hashQ[ii].qhead;
            for (; dlsw_ptr; dlsw_ptr = dlsw_ptr->next) {
                if (dlsw_ptr->dlsw_core_debug_flow)  
                    printf("\nDLSw core flow_control debugging is on for %u",
                           dlsw_ptr->our_dlc_corr);
            }
        }
    }

    /* 
     * Debug XID
     */
    if (dlsw_core_debug_xid) {
        printf("\nDLSw core xid debugging is on");
    } else {
        for (ii = 0; ii < NDLSW_HASH_SIZE; ii++) {
            dlsw_ptr = (dlsw_t *) dlsw_hashQ[ii].qhead;
            for (; dlsw_ptr; dlsw_ptr = dlsw_ptr->next) {
                if (dlsw_ptr->dlsw_core_debug_xid)  
                    printf("\nDLSw core xid debugging is on for %u",
                           dlsw_ptr->our_dlc_corr);
            }
        }
    }
}


/*
 * dlsw_core_debug_init is called at DLSW subsystem startup. It registers
 * the routines to maintain and display the DLSW debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when DLSW is started.
 */

void dlsw_core_debug_init (void)
{

    /* Register for "debug all". A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(dlsw_core_debug_all, "dlsw_core_debug_all");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when DLSW is initialized, we want to turn on
     * all the DLSW debugging right now.
     */
    dlsw_core_debug_all(debug_all_p());
}

static dlsw_t *validate_corr(uint given_dlc)
{

    dlsw_t *dlsw_ptr;
    ulong ii;

    for (ii = 0; ii < NDLSW_HASH_SIZE; ii++) {
         dlsw_ptr = (dlsw_t *) dlsw_hashQ[ii].qhead;
         for (; dlsw_ptr; dlsw_ptr = dlsw_ptr->next) {
            if (dlsw_ptr->our_dlc_corr == given_dlc)
               return(dlsw_ptr);
         }
    }
    return NULL;
}


/*
 * parse_debug_core()
 */

void parse_debug_core (parseinfo *csb)
{
    dlsw_t *dlsw_ptr;
    ulong ii;
    uint given_dlc;

    if (GETOBJ(int,3) == -1) {
        switch (GETOBJ(int,2)) {
          case DEBUG_DLSW_CORE_MSGS:
            dlsw_core_debug_msgs = csb->sense; break;
          case DEBUG_DLSW_CORE_STATE:
            dlsw_core_debug_state = csb->sense; break;
          case DEBUG_DLSW_CORE_FLOW:
            dlsw_core_debug_flow = csb->sense; break;
          case DEBUG_DLSW_CORE_XID:
            dlsw_core_debug_xid = csb->sense; break;
          case DEBUG_DLSW_CORE_SUM:
            dlsw_core_debug_all(csb->sense); break;
        }

        for (ii = 0; ii < NDLSW_HASH_SIZE; ii++) {
            dlsw_ptr = (dlsw_t *) dlsw_hashQ[ii].qhead;
            for (; dlsw_ptr; dlsw_ptr = dlsw_ptr->next) {
                switch (GETOBJ(int,2)) {
                  case DEBUG_DLSW_CORE_MSGS:
                    dlsw_ptr->dlsw_core_debug_msgs = csb->sense; break;
                  case DEBUG_DLSW_CORE_STATE:
                    dlsw_ptr->dlsw_core_debug_state = csb->sense; break;
                  case DEBUG_DLSW_CORE_FLOW:
                    dlsw_ptr->dlsw_core_debug_flow = csb->sense; break;
                  case DEBUG_DLSW_CORE_XID:
                    dlsw_ptr->dlsw_core_debug_xid = csb->sense; break;
                  case DEBUG_DLSW_CORE_SUM:
                    dlsw_ptr->dlsw_core_debug_msgs = csb->sense;
                    dlsw_ptr->dlsw_core_debug_state = csb->sense;
                    dlsw_ptr->dlsw_core_debug_flow = csb->sense; 
                    dlsw_ptr->dlsw_core_debug_xid = csb->sense;
                    break;
                }
            }
        }
    } else {
        given_dlc = GETOBJ(int, 3);
        dlsw_ptr = validate_corr(given_dlc);
        if (dlsw_ptr) {
            switch (GETOBJ(int,2)) {
              case DEBUG_DLSW_CORE_MSGS:
                dlsw_ptr->dlsw_core_debug_msgs = csb->sense; break;
              case DEBUG_DLSW_CORE_STATE:
                dlsw_ptr->dlsw_core_debug_state = csb->sense; break;
              case DEBUG_DLSW_CORE_FLOW:
                dlsw_ptr->dlsw_core_debug_flow = csb->sense; break;
              case DEBUG_DLSW_CORE_XID:
                dlsw_ptr->dlsw_core_debug_xid = csb->sense; break;
              case DEBUG_DLSW_CORE_SUM:
                dlsw_ptr->dlsw_core_debug_msgs = csb->sense;
                dlsw_ptr->dlsw_core_debug_state = csb->sense;
                dlsw_ptr->dlsw_core_debug_flow = csb->sense;
                dlsw_ptr->dlsw_core_debug_xid = csb->sense;
                break;
           }  /* switch (GETOBJ(int,2)) */
        }
    }
    show_core_debug();
}


void  dlsw_core_disable (parseinfo *csb)
{
    return;
}
 
 
static const char show_dlsw_hdr[] =
"Index           local addr(lsap)    remote addr(dsap)  state ";

static void print_circuit (dlsw_t *dlsw_ptr, boolean banner, int detail,
                           char *peer_str)
{
    char        *rif_str;
 
    if (banner)
        printf(show_dlsw_hdr);

    printf("\n%u \t%e(%02x)  %e(%02x) %s",
           dlsw_ptr->our_dlc_corr,
           dlsw_ptr->my_ckt ? dlsw_ptr->dlc_id.origin_mac_address :
           dlsw_ptr->dlc_id.target_mac_address,
 
           dlsw_ptr->my_ckt ? dlsw_ptr->dlc_id.origin_link_sap :
           dlsw_ptr->dlc_id.target_link_sap,
 
           dlsw_ptr->my_ckt ? dlsw_ptr->dlc_id.target_mac_address :
           dlsw_ptr->dlc_id.origin_mac_address,
 
           dlsw_ptr->my_ckt ? dlsw_ptr->dlc_id.target_link_sap :
           dlsw_ptr->dlc_id.origin_link_sap,
           dlsw_print_state(dlsw_ptr->state));

    if (detail) {
        printf("\n\tPCEP: %10x UCEP: %x", dlsw_ptr->p_cep_id,
               dlsw_ptr->u_cep_id);

        if (dlsw_ptr->port_id_dgra->port_type != CLS_TBRIDGE) {
            printf("\n\tPort:%s          %s",
                   dlsw_ptr->port_id_dgra->swidb->hwptr->hw_short_namestring,
                   peer_str);
	} else {
            printf("\n\tPort:TB%x          %s",
                   dlsw_ptr->port_id_dgra->trans_bridge_group, 
                   peer_str);
	}

        printf("\n\tFlow-Control-Tx CW:%d, Permitted:%d; "
               "Rx CW:%d, Granted:%d",
               dlsw_ptr->fc_send_curr_window,
               dlsw_ptr->fc_send_granted_units,
               dlsw_ptr->fc_recv_curr_window,
               dlsw_ptr->fc_recv_granted_units);
 
        rif_str = string_getnext();
        if (rif_str == NULL)
            return;

        if (RIF_LENGTH(dlsw_ptr->local_rif[0]))
            rif_str = get_rif_str(dlsw_ptr->local_rif, rif_str);
        else
            rif_str = "--no rif--";

        printf("\n\tRIF = %s", rif_str);
    }
}


void parse_show_core (parseinfo *csb)
{
    ulong ii;
    boolean first = TRUE;
    dlsw_t      *dlsw_ptr;
    uint given_dlc;
    leveltype       status;
    char *peer_str;

    peer_str = string_getnext();
    if (peer_str == NULL)
        return;
 
    automore_enable(NULL);

    if (GETOBJ(int,5) != DLSW_SHOW_CIRCUIT_KEY) {
        for (ii = 0; ii < NDLSW_HASH_SIZE; ii++) {
            dlsw_ptr = (dlsw_t *) dlsw_hashQ[ii].qhead;
            for (; dlsw_ptr; dlsw_ptr = dlsw_ptr->next) {
                status = raise_interrupt_level(NETS_DISABLE);
                mem_lock(dlsw_ptr);
                mem_lock(dlsw_ptr->port_id_dgra);
                reset_interrupt_level(status);

                /* Print Circuit info if:
                 *  - all circuits have been requested
                 *  - either mac address matches requested mac address, or
                 *  - either sap address matches requested sap address */
                if (GETOBJ(int,5) == DLSW_SHOW_CIRCUIT_ALL ||
                    (GETOBJ(int,5) == DLSW_SHOW_CIRCUIT_MAC && 
                     ((!bcmp(GETOBJ(hwaddr,3)->addr, 
                             dlsw_ptr->dlc_id.target_mac_address,
                             IEEEBYTES)) ||
                      (!bcmp(GETOBJ(hwaddr,3)->addr,
                             dlsw_ptr->dlc_id.origin_mac_address,
                             IEEEBYTES)))) ||
                    (GETOBJ(int,5) == DLSW_SHOW_CIRCUIT_SAP && 
                     ((GETOBJ(int,3) == dlsw_ptr->dlc_id.target_link_sap) ||
                      (GETOBJ(int,3) == dlsw_ptr->dlc_id.origin_link_sap) ))) {
                    automore_conditional(NDLSW_LINES);
                    if (dlsw_ptr->peer_handle) {
                        peer_handle_to_str(dlsw_ptr->peer_handle, peer_str);
                    } else {
                        peer_str[0] = '\0';
                    }
                    print_circuit(dlsw_ptr, first, GETOBJ(int,6), peer_str);
                    first = FALSE;
		}
                free(dlsw_ptr->port_id_dgra);
                if (dlsw_ptr->del_flag) {
                    printf("\nCircuit may have changed.... Retry command");
                    free(dlsw_ptr);
                    automore_disable();
                    return;
                } else {
                    free(dlsw_ptr);
                }

            }
        }
    } else {
        given_dlc = GETOBJ(int, 3);
        dlsw_ptr = validate_corr(given_dlc);
        if (dlsw_ptr) {
            status = raise_interrupt_level(NETS_DISABLE);
            mem_lock(dlsw_ptr);
            mem_lock(dlsw_ptr->port_id_dgra);
            reset_interrupt_level(status);
            if (dlsw_ptr->peer_handle) {
                peer_handle_to_str(dlsw_ptr->peer_handle, peer_str);
            } else {
                peer_str[0] = '\0';
            }
            print_circuit(dlsw_ptr, TRUE, GETOBJ(int,6), peer_str);
            free(dlsw_ptr->port_id_dgra);
            free(dlsw_ptr);
        }
    }
    automore_disable();
}
 
void parse_clear_circuits (parseinfo *csb)
{
    int ii;
    dlsw_t *dlsw_ptr;
    uint given_dlc;

    if (GETOBJ(int, 3)  == -1) {
        for (ii = 0; ii < NDLSW_HASH_SIZE; ii++) {
            dlsw_ptr = (dlsw_t *) dlsw_hashQ[ii].qhead;
            for (; dlsw_ptr; dlsw_ptr = dlsw_ptr->next)
                dlsw_fsm(dlsw_ptr, dlswma_admin, dlswmi_admin_stop);
        }
    } else {
        given_dlc = GETOBJ(int, 3);
        dlsw_ptr = validate_corr(given_dlc);
        if (dlsw_ptr)
            dlsw_fsm(dlsw_ptr, dlswma_admin, dlswmi_admin_stop);
    }
}

