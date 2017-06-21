/* $Id: if_cip_sna.c,v 3.4.20.3 1996/08/07 08:58:57 snyder Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_cip_sna.c,v $
 *------------------------------------------------------------------
 * if_cip_sna.c - SNA support for the CIP.
 *
 * May 1995, Mike Otto
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_cip_sna.c,v $
 * Revision 3.4.20.3  1996/08/07  08:58:57  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4.20.2  1996/05/17  11:17:52  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.6.2  1996/04/03  14:29:31  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.6.1  1996/03/21  06:07:57  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.4.20.1  1996/03/18  19:41:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/07  09:39:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.2  1996/02/20  00:51:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/01/29  22:01:31  motto
 * CSCdi46976:  CIP NetView alerts are not formatted correctly
 *
 * Revision 3.3.26.1  1996/01/24  21:46:20  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  09:17:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:35  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/08/09  19:29:16  rbatz
 * CSCdi38250:  CIP CSNA SNMP traps do not work
 *
 * Revision 2.7  1995/08/04  14:10:11  motto
 * Changed CIP LAN interface into SRB to only have one entry per LAN
 * instead of per adapter.
 * CSCdi37535:  CIP LAN SRB explorers multiplying by # of adapters defined
 * per LAN
 *
 * Revision 2.6  1995/07/20  13:33:57  motto
 * CSCdi37360:  CIP SNA LLC CC state display incorrect
 *
 * Revision 2.5  1995/07/18  21:46:27  motto
 * Update MIBs to current level of CIP support
 * Cleaned up CSNA show command support
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.4  1995/07/13  19:41:51  motto
 * Fixed all channel tcp/ip show commands to be consistent and work
 * properly.
 * CSCdi36252:  show ex ch <slot>/<port> tcp-connections needs parms to be
 * optional
 *
 * Revision 2.3  1995/06/28  09:24:35  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/21  20:28:09  motto
 * Fix 'show extended channel x/2 <llc2|csna|connection-map|
 * max-llc2-sessions> ....' commands for SNA support on the CIP card.
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.1  1995/06/07  20:40:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "subsys.h"
#include "cbus_registry.h"
#include "../dspu/snanm.h"
#include "../dspu/snanm_registry.regh"
#include "../dspu/cipllc_types.h"
#include "logger.h"
#include "if_cip.h"
#include "if_cip_sysmgt_rpc.h"
#include "if_cip_vlan.h"
#include "../if/channel_debug.h"
#include "if_cip_sna.h"

static char csna_dev[80];
static char local_dev[80];
static char remote_dev[80];

#define FRMR_CAUSE  4
#define FRMR_INVALID_RSP           1
#define FRMR_UNEXPECTED_IFRAME     3
#define FRMR_IFIELD_TOO_LONG       4
#define FRMR_INVALID_NR            8
#define FRMR_INVALID_CMD           17

static char *const csna_status_text[] = {
    "closed",
    "pendingOpen",
    "open",
    "pendingSetup",
    "setupComplete",
    "pendingClose",
    "unknown"
};

static char *const csna_slowdown_text[] = {
    "off",
    "sent",
    "received",
    "both",
    "unknown"
};

/*
 * Get a last LLC Ww cause string
 */
char *get_last_ww_cause (long last_ww_cause) {
    switch (last_ww_cause) {
        case SYSMGT_NEVER_INVOKED:        return "neverInvoked";
        case SYSMGT_LOST_DATA:            return "lostData";
        case SYSMGT_MAC_LAYER_CONGESTION: return "macLayerCongestion";
        default:                          break;
    }
    return "other";
}

/*
 * Get the llc cc state string
 */
char *get_llc_cc_state (llc_cc_oper_t *cc_oper) {
    switch (cc_oper->llcCcOperState) {
      case SYSMGT_ADM:           return "aDM";
      case SYSMGT_SETUP:         return "setup";
      case SYSMGT_CONN:          return "conn";
      case SYSMGT_NORMAL:        return "normal";
      case SYSMGT_BUSY:          return "busy";
      case SYSMGT_REJECT:        return "reject";
      case SYSMGT_AWAIT:         return "await";
      case SYSMGT_AWAIT_BUSY:    return "awaitBusy";
      case SYSMGT_AWAIT_REJECT:  return "awaitReject";
      case SYSMGT_D_CONN:        return "discConn";
      case SYSMGT_RESET:         return "reset";
      case SYSMGT_ERROR:         return "error";
      case SYSMGT_PEND_DISC_RSP: return "pendDiscRsp";
      default: break;
    }
    return "unknown";
}

char *get_csna_slowdown_text (csna_dev_t *csna_dev) {
    int state = csna_dev->cipCardCsnaOperSlowDownState;
    if (state >= (sizeof(csna_slowdown_text)/sizeof(char *))) {
        state = (sizeof(csna_slowdown_text)/sizeof(char *)) - 1;
    }
    return csna_slowdown_text[state];
}

char *get_csna_status_text (csna_dev_t *csna_dev) {
    int state = csna_dev->cipCardCsnaOperState;
    if (state >= (sizeof(csna_status_text)/sizeof(char *))) {
        state = (sizeof(csna_status_text)/sizeof(char *)) - 1;
    }
    return csna_status_text[state];
}

/*
 * csna_dev_rpc - Retrieve CSNA info from specified CIP card.
 * Input: hwidb = interface
 *        vc    = pointer to cip_vcptype
 *        csna_dev = pointer to data structure to 
 *        return CSNA info in.
 * Output: csna_dev_t pointer if found or NULL if not found
 */
csna_dev_t *csna_dev_rpc (hwidbtype *hwidb, cip_vctype *vc, 
                         csna_dev_t *csna_dev) {

    netmgt_header_t nm_hdr;
    csna_dev_t   *results = NULL;

    if (csna_dev && vc && hwidb) {
        csna_dev->index.vcn    = vc->vc_num;
        nm_hdr.class           = SYSMGT_CSNA;
        nm_hdr.type            = (ushort)CSNA_DEVICE;
        nm_hdr.index_length    = CSNA_DEVICE_INDEX_SIZE;
        nm_hdr.flags.operation = NETMGT_GET_MATCH;
        nm_hdr.data_length     = CSNA_DEVICE_SIZE;
        nm_hdr.slot            = hwidb->slot;

        if (channel_sysmgt_debug) {
            buginf("\n%s CIP CSNA DEVICE (%d/%d,vc=%d)", 
                   netmgt_operations[nm_hdr.flags.operation],
                   nm_hdr.slot, hwidb->unit, vc->vc_num);
        }

        results = sysmgt_rpc(hwidb, &nm_hdr, csna_dev);
    }
    return results;
}

/*
 * get_csna_dev - Retrieve CSNA info from specified CIP card.
 * Input: search = SEARCH_NEXT or SEARCH_MATCH
 *        slot  = pointer to slot index 
 *        port  = pointer to port index
 *        path  = pointer to path index
 *        dev   = pointer to device index
 *        csna_dev = pointer to data structure to 
 *        return CSNA info in.
 * Output: csna_dev_t pointer if found or NULL if not found
 */
csna_dev_t *get_csna_dev (int search, long *slot, long *port,
			  long *path, long *dev, csna_dev_t *csna_dev) {

    cip_vctype *vc;
    hwidbtype *hwidb;

    vc = cip_getSubchannelCsnaConfig(slot, port, path, dev, &hwidb, search);
    return csna_dev_rpc(hwidb, vc, csna_dev);
}

/*
 * get_admin_llc2_sessions - Retrieve configure number of llc2 sessions on cip.
 * Input: search = SEARCH_NEXT or SEARCH_MATCH
 *        slot  = pointer to slot index 
 *        sessions = pointer to number of sessions being retrieved 
 * Output: Truth Value
 */
boolean get_admin_llc2_sessions (int search, long *slot, long *sessions) {
    hwidbtype *hwidb;
    boolean rc = FALSE;
    
    FOR_ALL_HWIDBS ( hwidb) {
        if ((hwidb->type == IDBTYPE_CIP) && 
            (cip_interface_is_virtual(hwidb)) &&
            ((*slot == hwidb->slot) ||
             (((search == SEARCH_NEXT) && (*slot < hwidb->slot))))) {
            *sessions = cip_get_csna_vlan_maxllc2_sessions( hwidb);
            *slot = hwidb->slot;
            rc = TRUE;
            break;
        }
    }
    return rc;
}

/*
 * get_from_cip_llc2_session_info - Retrieve llc2 sessions info on cip.
 * Input: hwidb = Cip Interface.
 *        search = SEARCH_NEXT or SEARCH_MATCH
 *        llc2_sess = pointer to data being retrieved 
 * Output: Truth Value
 */
boolean get_from_cip_llc2_session_info (hwidbtype * hwidb, int search,
                                        llc2_sessions_t *llc2_sess)
{
    netmgt_header_t nm_hdr;

    nm_hdr.class           = SYSMGT_LLC;
    nm_hdr.type            = (ushort)SYSMGT_LLC_SESSIONS;
    nm_hdr.index_length    = 0;
    nm_hdr.flags.operation = NETMGT_GET_MATCH;
    nm_hdr.slot            = hwidb->slot;
    nm_hdr.data_length     = LLC2_SESSIONS_SIZE;

    if (channel_sysmgt_debug) {
        buginf("\n%s CIP LLC2 SESSIONS (%d/%d)", 
                 netmgt_operations[nm_hdr.flags.operation],
                 nm_hdr.slot, hwidb->unit);
    }

    if (sysmgt_rpc(hwidb, &nm_hdr, llc2_sess)) {
        if (nm_hdr.flags.ret_code == NETMGT_FOUND)
            return TRUE;
    }

    return FALSE;
}
 

/*
 * get_llc2_sessions_info - Retrieve llc2 sessions info on cip.
 * Input: search = SEARCH_NEXT or SEARCH_MATCH
 *        slot  = pointer to slot index 
 *        llc2_sess = pointer to data being retrieved 
 * Output: Truth Value
 */
boolean get_llc2_sessions_info (int search, long *slot, 
                                llc2_sessions_t *llc2_sess) {
    hwidbtype *hwidb;
    boolean rc = FALSE;
    
    FOR_ALL_HWIDBS ( hwidb) {
        if ((hwidb->type == IDBTYPE_CIP) && 
            (cip_interface_is_virtual(hwidb)) &&
            ((*slot == hwidb->slot) ||
             (((search == SEARCH_NEXT) && (*slot < hwidb->slot))))) {

            if (get_from_cip_llc2_session_info(hwidb, search, llc2_sess)) {
                *slot = hwidb->slot;
                rc = TRUE;
                break;
            }
	}
    }
    return rc;
}

boolean get_llc2_conn (hwidbtype *hwidb, unsigned long csna_handle, 
		       csna_llc2_conn_t *csna_llc2_conn) {
    netmgt_header_t nm_hdr;

    nm_hdr.class           = SYSMGT_CSNA;
    nm_hdr.type            = (ushort)CSNA_LLC2_CONN;
    nm_hdr.index_length    = CSNA_HANDLE_SIZE;
    nm_hdr.flags.operation = NETMGT_GET_MATCH;
    nm_hdr.slot            = hwidb->slot;
    nm_hdr.data_length     = CSNA_LLC2_CONN_SIZE;
    csna_llc2_conn->index.csna_handle = csna_handle;

    if (channel_sysmgt_debug) {
        buginf("\n%s CIP LLC2 CONNECTIONS (%d/%d,%u)", 
                netmgt_operations[nm_hdr.flags.operation],
                nm_hdr.slot, hwidb->unit, csna_handle);
    }

    return (sysmgt_rpc( hwidb, &nm_hdr, csna_llc2_conn) != NULL);
}

/*
 * Retrieve LLC port info from specified CIP card.
 * Input: search = SYSMGT_GET, SYSMGT_GET_NEXT
 *        hwidb = CIP interface
 *        virtual_index = virtual lan id
 *        llc_port_p = pointer to data structure to 
 *        return LLC info in
 *        size = size of llc_port_p info structure
 *        table_id = type of llc port info (admin, oper, stats)
 * Output: boolean TRUE if found or FALSE if not found
 */
boolean get_cip_llc_port (int search, hwidbtype *hwidb, long *virtual_index,
			  void *llc_port_p, ushort size, ushort table_id) {
   netmgt_header_t nm_hdr;
   llc_port_index_t *port_index_p;
   void *vadapt;
   llc_port_admin_t *port_data;
   uchar *name;

   if (llc_port_p) {
        if (table_id == SYSMGT_LLC_PORT_ADMIN) {
            vadapt = cip_get_vadapt_from_vc( hwidb, (search == SEARCH_MATCH), 
                                             *virtual_index);
            if (vadapt != NULL) {
                port_data = (llc_port_admin_t *)llc_port_p;
                *virtual_index = get_vadapt_vc( vadapt);
                name = get_vadapt_adapt_name( vadapt);
                if (name) {
                    bcopy( name, port_data->llcPortAdminName, PORT_NAME_LEN);
                }
                port_data->llcPortAdminMaxSaps = MAX_SAPS_PER_CIP;
                port_data->llcPortAdminMaxCcs = cip_get_csna_vlan_maxllc2_sessions( hwidb);
                port_data->llcPortAdminMaxPDUOctets = get_vadapt_llc2_N1( vadapt);
                port_data->llcPortAdminMaxUnackedIPDUsSend = get_vadapt_llc2_Tw( vadapt);
                port_data->llcPortAdminMaxUnackedIPDUsRcv = get_vadapt_llc2_Rw( vadapt);
                port_data->llcPortAdminMaxRetransmits = get_vadapt_llc2_N2( vadapt);
                port_data->llcPortAdminAckTimer = get_vadapt_llc2_T1_ack( vadapt);
                port_data->llcPortAdminPbitTimer = get_vadapt_llc2_T1_pf( vadapt);
                port_data->llcPortAdminRejTimer = get_vadapt_llc2_T1_rej( vadapt);
                port_data->llcPortAdminBusyTimer = get_vadapt_llc2_T1_busy( vadapt);
                port_data->llcPortAdminInactTimer = get_vadapt_llc2_Ti( vadapt);
                port_data->llcPortAdminDelayAckCount = get_vadapt_llc2_N3( vadapt);
                port_data->llcPortAdminDelayAckTimer = get_vadapt_llc2_T2( vadapt);
                port_data->llcPortAdminNw = get_vadapt_llc2_Nw( vadapt);
                return TRUE;
            }
        }
        else {
            nm_hdr.class           = SYSMGT_LLC;
            nm_hdr.type            = table_id;
            nm_hdr.index_length    = LLC_PORT_INDEX_SIZE;
            nm_hdr.data_length     = size;
            nm_hdr.flags.operation = (search == SEARCH_MATCH) ? 
                                      NETMGT_GET_MATCH : NETMGT_GET_NEXT;
            port_index_p = llc_port_p;
            port_index_p->vcn = *virtual_index;

            if (channel_sysmgt_debug) {
                buginf("\n%s CIP LLC2 PORT (%d/%d,%u)", 
                       netmgt_operations[nm_hdr.flags.operation],
                       nm_hdr.slot, hwidb->unit, *virtual_index);
            }

            if (sysmgt_rpc(hwidb, &nm_hdr, llc_port_p)) {
                if (nm_hdr.flags.ret_code == NETMGT_FOUND) {
                    *virtual_index = port_index_p->vcn;
                    return TRUE;
	        }
	    }
        }
    }
    return FALSE;
}

/*
 * Retrieve LLC SAP info from specified CIP card.
 * Input: search = SYSMGT_GET, SYSMGT_GET_NEXT
 *        hwidb = CIP interface
 *        virtual_index = virtual lan id
 *        local_sap = sap to retrieve info for
 *        sap_p = pointer to data structure to 
 *        return LLC info in
 *        size = size of llc_sap_p info structure
 *        table_id = type of llc sap info (admin, oper, stats)
 * Output: boolean TRUE if found or FALSE if not found
 */
boolean get_cip_llc_sap (int search, hwidbtype *hwidb, 
			 long *virtual_index, long local_sap,
			 void *sap_p, ushort size,
			 ushort table_id) {
   netmgt_header_t nm_hdr;
   llc_sap_index_t *sap_index_p;

   if (sap_p) {
        if (table_id == SYSMGT_LLC_SAP_ADMIN) {
            /* find virtual adapters */
        }
        else {
            nm_hdr.class            = SYSMGT_LLC;
            nm_hdr.type             = table_id;
            nm_hdr.index_length     = LLC_SAP_INDEX_SIZE;
            nm_hdr.slot             = hwidb->slot;
            nm_hdr.data_length      = size;
            nm_hdr.flags.operation  = (search == SEARCH_MATCH) ? 
                                       NETMGT_GET_MATCH : NETMGT_GET_NEXT;
            sap_index_p = sap_p;
            sap_index_p->vcn = *virtual_index;
            sap_index_p->local_sap = local_sap;

            if (channel_sysmgt_debug) {
                buginf("\n%s CIP SAP PORT (%d/%d,%u,%02X)", 
                       netmgt_operations[nm_hdr.flags.operation],
                       nm_hdr.slot, hwidb->unit, *virtual_index, local_sap);
            }

            if (sysmgt_rpc(hwidb, &nm_hdr, sap_p)) {
                if (nm_hdr.flags.ret_code == NETMGT_FOUND) {
                    *virtual_index = sap_index_p->vcn;    
                    return TRUE;
	        }
	    }
        }
    }
    return FALSE;
}

/*
 * Retrieve LLC CC info from specified CIP card.
 * Input: search = SYSMGT_GET, SYSMGT_GET_NEXT
 *        hwidb = CIP interface
 *        virtual_index = virtual lan id
 *        local_sap = sap to retrieve info for
 *        rem_mac   = pointer to remote MAC address
 *        rem_sap   = remote sap to retrieve info for
 *        llc_cc_p = pointer to data structure to 
 *        return LLC info in
 *        size = size of llc_cc_p info structure
 *        table_id = type of llc cc info (admin, oper, stats)
 * Output: boolean TRUE if found or FALSE if not found
 */
boolean get_cip_llc_cc (int search, hwidbtype *hwidb, 
			long *virtual_index, long local_sap,
			uchar *rem_mac, long rem_sap,
			void *cc_p, ushort size,
			ushort table_id) {
   netmgt_header_t nm_hdr;
   llc_cc_index_t *cc_index_p;

   if (cc_p) {
        if (table_id == SYSMGT_LLC_CC_ADMIN) {
            /* find virtual adapters */
        }
        else {
            nm_hdr.class           = SYSMGT_LLC;
            nm_hdr.type            = table_id;
            nm_hdr.index_length    = LLC_CC_INDEX_SIZE;
            nm_hdr.data_length     = size;
            nm_hdr.slot            = hwidb->slot;
            nm_hdr.flags.operation = (search == SEARCH_MATCH) ? 
                                      NETMGT_GET_MATCH : NETMGT_GET_NEXT;
            cc_index_p = cc_p;
            cc_index_p->vcn = *virtual_index;
            cc_index_p->local_sap = local_sap;
            if (rem_mac) {
               bcopy(rem_mac, &cc_index_p->rem_mac[0], MAC_ADDR_LEN);
	    }
            else {
               memset(&cc_index_p->rem_mac[0], 0, MAC_ADDR_LEN);
            }
            cc_index_p->rem_sap = rem_sap;

            if (channel_sysmgt_debug) {
                buginf("\n%s CIP CC PORT (%d/%d,%u,%02X,%14e,%02X)", 
                       netmgt_operations[nm_hdr.flags.operation],
                       nm_hdr.slot, hwidb->unit, *virtual_index, local_sap,
                       cc_index_p->rem_mac, rem_sap);
            }

            if (sysmgt_rpc(hwidb, &nm_hdr, cc_p)) {
                if (nm_hdr.flags.ret_code == NETMGT_FOUND) {
                    *virtual_index = cc_index_p->vcn;
                    return TRUE;
	        }
	    }
        }
    }
    return FALSE;
}

static boolean cip_get_llc_info ( boolean get_match, hwidbtype *hwidb, 
                                  void *llc_data) {
    sna_dlc_llc_t *llc_p;
    boolean rc = FALSE;
    int search;

    if ((hwidb->type == IDBTYPE_CIP) && 
        (cip_interface_is_virtual(hwidb)) && 
        (llc_data)) {
        llc_p = (sna_dlc_llc_t *)llc_data;
        search = get_match ? SEARCH_MATCH : SEARCH_NEXT;
        switch (llc_p->info_type) {
            case SYSMGT_LLC_PORT:
                rc = get_cip_llc_port(search, hwidb, 
                                      &llc_p->llcPortVirtualIndex,
                                      llc_p->llc_info, llc_p->size, 
                                      llc_p->table_id);
                break;
            case SYSMGT_LLC_SAP:
                rc = get_cip_llc_sap(search, hwidb, 
                                     &llc_p->llcPortVirtualIndex,
                                     llc_p->llcSapNumber, llc_p->llc_info, 
                                     llc_p->size, llc_p->table_id);
                break;
            case SYSMGT_LLC_CC:
                rc = get_cip_llc_cc(search, hwidb, 
                                    &llc_p->llcPortVirtualIndex,
                                    llc_p->llcSapNumber, 
                                    llc_p->llcCcRMac, llc_p->llcCcRSap,
                                    llc_p->llc_info, llc_p->size, 
                                    llc_p->table_id);
                break;
	    default:
                break;
        }
    }
    return rc;
}

static void get_local_dev ( void *vlan, void *vadapt, cip_llctype *cipllc) {

    sprintf(local_dev, "LAN:%s(%d) ADAPTER:%e LSAP:%02X", 
            get_vlan_lan_type_name( vlan, get_vlan_lan_type( vlan )),
            get_vlan_lan_id( vlan),
            get_vadapt_mac_address( vadapt), 
            cipllc->local_sap);
}

/*
 * setup_sp_event - prepare event for alert handler
 */
static boolean setup_sap_event (cip_llctype *cipllc, hwidbtype *hwidb, 
				llc_sap_event_t *sap_ev,
				csna_llc2_conn_t *csna) {
    void *vlan, *vadapt;
    
    vadapt = cip_get_vadapt_from_vc( hwidb, TRUE, sap_ev->index.vcn);
    if (vadapt == NULL) {
        if (channel_sysmgt_debug) {
            buginf("\nCIP%d SYSMGT LLC event error - unknown ADAPTER(vc=%d)", 
                    hwidb->slot, sap_ev->index.vcn);
        }
        return FALSE;
    }
    vlan   = cip_get_vlan_from_vadapt( hwidb, vadapt);
    if (vlan == NULL) {
        if (channel_sysmgt_debug) {
            buginf("\nCIP%d SYSMGT LLC event error - unknown LAN(vc=%d)", 
                    hwidb->slot, sap_ev->index.vcn);
        }
        return FALSE;
    }
    
    cipllc->idb = hwidb;
    cipllc->virtual_index = sap_ev->index.vcn;
    cipllc->adapno = get_vadapt_adapt_no( vadapt);
    bcopy( get_vadapt_adapt_name( vadapt), cipllc->vadapt_name, 
           sizeof(cipllc->vadapt_name));
    cipllc->vadapt_name[sizeof(cipllc->vadapt_name)] = 0; /* term string */
    cipllc->local_ring = get_vlan_srb_local_ring( vlan);
    cipllc->lan_id = get_vlan_lan_id( vlan);
    bcopy( get_vadapt_mac_address( vadapt), cipllc->local_macaddr, 
           sizeof(cipllc->local_macaddr));
    cipllc->local_sap = sap_ev->index.local_sap;
    if (!get_llc2_conn(hwidb, sap_ev->csna_fail_handle, csna)) {
        if (channel_sysmgt_debug) {
            buginf("\nCIP%d SYSMGT LLC event error - invalid CSNA handle(%d)", 
                    hwidb->slot, sap_ev->csna_fail_handle);
            return FALSE;
        }
    }
    cipllc->csna_port = csna->cipCardCsnaPort;
    cipllc->csna_path = csna->cipCardCsnaConnPath;
    cipllc->csna_device = csna->cipCardCsnaConnDevice;
    remote_dev[0] = 0;
    get_local_dev( vlan, vadapt, cipllc);
    return TRUE;
}

/*
 * setup_cc_event - prepare event for alert handler
 */
static boolean setup_cc_event (cip_llctype *cipllc, hwidbtype *hwidb, 
			       llc_cc_event_t *cc_ev, csna_llc2_conn_t *csna) {
    void *vlan, *vadapt;
    
    vadapt = cip_get_vadapt_from_vc( hwidb, TRUE, cc_ev->index.vcn);
    if (vadapt == NULL) {
        if (channel_sysmgt_debug) {
            buginf("\nCIP%d SYSMGT LLC event error - unknown ADAPTER(vc=%d)", 
                    hwidb->slot, cc_ev->index.vcn);
        }
        return FALSE;
    }
    vlan   = cip_get_vlan_from_vadapt( hwidb, vadapt);
    if (vlan == NULL) {
        if (channel_sysmgt_debug) {
            buginf("\nCIP%d SYSMGT LLC event error - unknown LAN(vc=%d)", 
                    hwidb->slot, cc_ev->index.vcn);
        }
        return FALSE;
    }
    
    cipllc->idb = hwidb;
    cipllc->virtual_index = cc_ev->index.vcn;
    cipllc->adapno = get_vadapt_adapt_no( vadapt);
    bcopy( get_vadapt_adapt_name( vadapt), cipllc->vadapt_name, 
           sizeof(cipllc->vadapt_name));
    cipllc->vadapt_name[sizeof(cipllc->vadapt_name)] = 0; /* term string */
    cipllc->local_ring = get_vlan_srb_local_ring( vlan);
    cipllc->lan_id = get_vlan_lan_id( vlan);
    bcopy( get_vadapt_mac_address( vadapt), cipllc->local_macaddr, 
           sizeof(cipllc->local_macaddr));
    cipllc->local_sap = cc_ev->index.local_sap;
    bcopy( cc_ev->index.rem_mac, cipllc->remote_macaddr, 
           sizeof(cipllc->remote_macaddr));
    cipllc->remote_sap = cc_ev->index.rem_sap;
    cipllc->v_r = cc_ev->v_r;
    cipllc->v_s = cc_ev->v_s;
    cipllc->last_n_r_rxd = cc_ev->last_n_r_rxd;
    cipllc->last_ctrl_rxd_msb = cc_ev->last_ctrl_rxd_msb;
    cipllc->last_ctrl_rxd_lsb = cc_ev->last_ctrl_rxd_lsb;
    cipllc->last_ctrl_txd_msb = cc_ev->last_ctrl_txd_msb;
    cipllc->last_ctrl_txd_lsb = cc_ev->last_ctrl_txd_lsb;
    cipllc->lstn_role = cc_ev->lstn_role;
    if (!get_llc2_conn(hwidb, cc_ev->csna_handle, csna)) {
        if (channel_sysmgt_debug) {
            buginf("\nCIP%d SYSMGT LLC event error - invalid CSNA handle(%d)", 
                    hwidb->slot, cc_ev->csna_handle);
            return FALSE;
        }
    }
    cipllc->csna_port = csna->cipCardCsnaPort;
    cipllc->csna_path = csna->cipCardCsnaConnPath;
    cipllc->csna_device = csna->cipCardCsnaConnDevice;
    bcopy(cc_ev->frmr, cipllc->frmr, sizeof(cipllc->frmr));
    cipllc->cc_state = SYSMGT_RESET;     /* Hard code for now */
    get_local_dev( vlan, vadapt, cipllc);
    sprintf(remote_dev, "RMAC:%e RSAP:%02X", 
            cipllc->remote_macaddr, cipllc->remote_sap);
    return TRUE;
}

/*
 * cip_netmgt_event_handler - registered function that is called
 *                            whenever an LLC event is recevied
 *                            from the CIP.
 */
static void cip_netmgt_event_handler (ushort slot, ushort code, 
				      ushort length, void *ev_data ) {
    hwidbtype *hwidb;
    csna_llc2_conn_t csna_llc_conn;
    cip_llctype cipllc;
    llc_cc_event_t *cc_ev;
    llc_sap_event_t *sap_ev;
    uchar frmr_code;
    char *descriptor = NULL;
    boolean doSnmpTrap = TRUE;

    FOR_ALL_HWIDBS ( hwidb) {
        if ((hwidb->type == IDBTYPE_CIP) && 
            (cip_interface_is_virtual(hwidb)) &&
            (slot == hwidb->slot)) {
            break;
        }
    }
    if (hwidb == NULL) {
        if (channel_sysmgt_debug) {
            buginf("\nCIP%d SYSMGT LLC event error - invalid interface", 
                    hwidb->slot);
        }
        return;
    }

    if (code >= MAX_SNALLC_EVENTS) {
        if (channel_sysmgt_debug) {
            buginf("\nCIP%d SYSMGT LLC event error - unknown event code", 
                    hwidb->slot, code);
        }
        return;
    }

    cc_ev = (llc_cc_event_t *)ev_data;
    sap_ev = (llc_sap_event_t *)ev_data;
    switch (code) {
        case UNKNOWN_SAP_EVENT:
            if (!setup_cc_event( &cipllc, hwidb, cc_ev, &csna_llc_conn))
                return;
            reg_invoke_Cipllc_Alert(CipllcUnknownSapAlert, &cipllc);
            descriptor = "Connection attempt failed/Unknown SAP";
            doSnmpTrap = FALSE;
            break;
        case DUP_SAP_EVENT:
            if (!setup_sap_event( &cipllc, hwidb, sap_ev, &csna_llc_conn))
                return;
            reg_invoke_Cipllc_Alert(CipllcDupSapAlert, &cipllc);
            descriptor = "Open SAP failed/Duplicate detected";
            break;
        case CCS_EXCEEDED_EVENT:
            if (!setup_cc_event( &cipllc, hwidb, cc_ev, &csna_llc_conn))
                return;
            reg_invoke_Cipllc_Alert(CipllcConnLimitExceededAlert, &cipllc);
            descriptor = "Connection attempted failed/No connection resources";
            break;
        case LINK_LOST_EVENT:
            if (!setup_cc_event( &cipllc, hwidb, cc_ev, &csna_llc_conn))
                return;
            reg_invoke_Cipllc_Alert(CipllcAlert1, &cipllc);
            descriptor = "Connection failed/Link lost";
            break;
        case DM_RXD_EVENT:
            if (!setup_cc_event( &cipllc, hwidb, cc_ev, &csna_llc_conn))
                return;
            reg_invoke_Cipllc_Alert(CipllcAlert2, &cipllc);
            descriptor = "Connection failed/Received DM in ABM";
            break;
        case SABME_RXD_IN_ABME_EVENT:
            if (!setup_cc_event( &cipllc, hwidb, cc_ev, &csna_llc_conn))
                return;
            reg_invoke_Cipllc_Alert(CipllcAlert3, &cipllc);
            descriptor = "Connection failed/Received SABME while in ABM";
            break;
        case FRMR_RXD_EVENT:
            frmr_code = cc_ev->frmr[FRMR_CAUSE];
            if (!setup_cc_event( &cipllc, hwidb, cc_ev, &csna_llc_conn))
                return;
            if ((frmr_code == FRMR_INVALID_RSP) || 
                (frmr_code == FRMR_INVALID_CMD)) {
                reg_invoke_Cipllc_Alert(CipllcAlert4, &cipllc);
                descriptor = "Connection failed/Frmr rxd - invalid cmd/rsp";
            }
            else if (frmr_code == FRMR_UNEXPECTED_IFRAME) {
                reg_invoke_Cipllc_Alert(CipllcAlert5, &cipllc);
                descriptor = "Connection failed/Frmr rxd - unexpected I-frame";
            }
            else if (frmr_code == FRMR_INVALID_NR) {
                reg_invoke_Cipllc_Alert(CipllcAlert6, &cipllc);
                descriptor = "Connection failed/Frmr rxd - invalid Nr";
            }
            else if (frmr_code == FRMR_IFIELD_TOO_LONG) {
                reg_invoke_Cipllc_Alert(CipllcAlert7, &cipllc);
                descriptor = "Connection failed/Frmr rxd - I-field too long";
            }
            break;
        case FRMR_TXD_EVENT:
            frmr_code = cc_ev->frmr[FRMR_CAUSE];
            if (!setup_cc_event( &cipllc, hwidb, cc_ev, &csna_llc_conn))
                return;
            if ((frmr_code == FRMR_INVALID_RSP) || 
                (frmr_code == FRMR_INVALID_CMD)) {
                reg_invoke_Cipllc_Alert(CipllcAlert8, &cipllc);
                descriptor = "Connection failed/Frmr txd - invalid cmd/rsp";
            }
            else if (frmr_code == FRMR_UNEXPECTED_IFRAME) {
                reg_invoke_Cipllc_Alert(CipllcAlert9, &cipllc);
                descriptor = "Connection failed/Frmr txd - unexpected I-frame";
            }
            else if (frmr_code == FRMR_INVALID_NR) {
                reg_invoke_Cipllc_Alert(CipllcAlert10, &cipllc);
                descriptor = "Connection failed/Frmr txd - invalid Nr";
            }
            else if (frmr_code == FRMR_IFIELD_TOO_LONG) {
                reg_invoke_Cipllc_Alert(CipllcAlert11, &cipllc);
                descriptor = "Connection failed/Frmr txd - I-field too long";
            }
            break;
        default:
            if (channel_sysmgt_debug) {
                buginf("\nCIP%d SYSMGT LLC event - unknown type code", 
                        hwidb->slot, code);
            }
            doSnmpTrap = FALSE;
            return;
    }

    if (doSnmpTrap)
        reg_invoke_cip_cipcsna_trap(hwidb, code, &cipllc);

    sprintf(csna_dev, "CSNA Port(%d) Path(%04X) Device(%02X)",
                      csna_llc_conn.cipCardCsnaPort,
                      csna_llc_conn.cipCardCsnaConnPath,
                      csna_llc_conn.cipCardCsnaConnDevice);

    if (channel_event_sna_debug && descriptor) {
        buginf("\nCIP LLC ALERT %s - %s\n%s\n%s\n%s\n",
                hwidb->hw_namestring, descriptor, 
                csna_dev, local_dev, remote_dev);
    }
}

static void cip_sna_init( subsystype *subsys) {
    reg_add_get_llc_info(cip_get_llc_info, "cip_get_llc_info");
    reg_add_cip_netmgt_event( SYSMGT_LLC, cip_netmgt_event_handler, 
                              "cip_netmgt_event_handler");
}

/*
 * CIP SNA subsystem header
 */

#define CIP_SNA_MAJVERSION   1
#define CIP_SNA_MINVERSION   0
#define CIP_SNA_EDITVERSION  1

SUBSYS_HEADER(cip_sna, CIP_SNA_MAJVERSION, CIP_SNA_MINVERSION,
	      CIP_SNA_EDITVERSION,
	      cip_sna_init, SUBSYS_CLASS_MANAGEMENT,
	      "req: cip", NULL);

