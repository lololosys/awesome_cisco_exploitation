/* $Id: dlsw_csm_util.c,v 3.8.18.13 1996/09/04 13:57:20 rbatz Exp $
 *------------------------------------------------------------------
 * DLSw CSM - common routines
 *
 * August 1994, Ravi Periasamy 
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Contains routines used by the various CSM modules
 *------------------------------------------------------------------
 * $Log: dlsw_csm_util.c,v $
 * Revision 3.8.18.13  1996/09/04  13:57:20  rbatz
 * CSCdi67990:  DLSw+/QLLC local switching broken.
 *              QLLC column in dlsw_switch_table incorrect.
 * Branch: California_branch
 *
 * Revision 3.8.18.12  1996/08/30  18:46:12  pmorton
 * CSCdi67720:  LNM over DLSw broken in 11.2
 * Branch: California_branch
 * VDLC and QLLC rows were transposed in dlsw_switch_table
 *
 * Revision 3.8.18.11  1996/08/22  18:35:38  ravip
 * CSCdi66201:  DLSw load sharing broken
 * Branch: California_branch
 *
 * Revision 3.8.18.10  1996/08/09  23:43:08  ravip
 * CSCdi50688:  dlsw netbios slow session bring up with peer on demand
 * Branch: California_branch
 *
 * Revision 3.8.18.9  1996/08/09  21:55:49  ravip
 * CSCdi59181:  Best path not chosen for the first circuit
 * Branch: California_branch
 *
 * Revision 3.8.18.8  1996/08/09  19:48:11  ravip
 * CSCdi52387:  DLSw:CPUHOG process=DLSw Background, PC=6055C4E8
 *              Change CSM timer to use managed timers
 * Branch: California_branch
 *
 * Revision 3.8.18.7  1996/08/07  08:58:12  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.8.18.6  1996/07/23  13:21:39  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.8.18.5  1996/06/04  05:50:38  schiang
 * CSCdi59336:  incorporate code reviewrs comment for CSCdi57445
 * dlsw_switch_table is used to replace local_switch to store the
 * switch-ability infomation.
 * Branch: California_branch
 *
 * Revision 3.8.18.4  1996/06/02  05:09:39  schiang
 * CSCdi57445:  DLSw+ local switch doesnt block traffic from RSRB to
 * non-NCIA IFs
 * Branch: California_branch
 *
 * Revision 3.8.18.3  1996/05/31  14:26:34  ravip
 * CSCdi58834:  4500/4000/7000 routers crash on show dlsw reachability
 * Branch: California_branch
 *
 * Revision 3.8.18.2  1996/05/17  10:45:32  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.4.5  1996/05/15  00:39:09  ravip
 * CSCdi50707:  dlsw the reachability search is linear, should be binary
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.4.4  1996/04/26  07:34:13  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.8.4.3  1996/04/25  19:24:16  ravip
 * CSCdi36046:  cache with wildcard/mask does not work in dlsw
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.4.2  1996/04/25  16:20:09  ravip
 * CSCdi45773:  dlsw icanreach mac-exclusive command not working
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.4.1  1996/04/03  14:01:26  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.8.18.1  1996/03/18  19:31:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.2  1996/03/07  08:44:27  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  00:45:57  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/07  21:45:46  ravip
 * CSCdi46935:  DLSw does not propogate largest frame bits correctly
 *
 * Revision 3.7  1996/02/01  06:01:22  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/18  22:24:38  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/12  17:33:04  ravip
 * CSCdi46357:  clean up dlsw csm debug messages
 *
 * Revision 3.4  1995/12/19  00:39:47  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.3  1995/11/17  09:03:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:46  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:23:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:55:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/20  14:09:31  ravip
 * CSCdi40722: Exclusive reachability option does not filter frames
 *             correctly. Fixed problems with deleting reachability cache.
 *
 * Revision 2.1  1995/06/07  20:26:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../util/fsm.h"
 
#include "../llc2/llc2.h"
#include "../cls/clsi_msg.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ibm/netbios.h"
#include "../ibm/libibm_externs.h"

#include "../h/mgd_timers.h"


#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_core_externs.h"
#include "../dlsw/dlsw_csm_db.h"
#include "../dlsw/dlsw_csm.h"
#include "../dlsw/dlsw_debug.h"


void reverse_mac_mask (char *mac_ptr);

/*
 * String Conversion Routines for Debugs
 */

char *csm_dlx (int op)
{
    switch (op) {
      case DLX_CAN_U_REACH         : return("CAN_U_REACH");
      case DLX_I_CAN_REACH         : return("I_CAN_REACH");
      case DLX_I_CANNOT_REACH      : return("I_CANNOT_REACH");
      case DLX_START_DL            : return("START_DL");
      case DLX_DL_STARTED          : return("DL_STARTED");
      case DLX_START_DL_FAILED     : return("START_DL_FAILED");
      case DLX_RESTART_DL          : return("RESTART_DL");
      case DLX_DL_RESTARTED        : return("DL_RESTARTED");
      case DLX_RESTART_DL_FAILED   : return("RESTART_DL_FAILED");
      case DLX_XIDFRAME            : return("XIDFRAME");
      case DLX_CONTACT_STN         : return("CONTACT_STN");
      case DLX_STN_CONTACTED       : return("STN_CONTACTED");
      case DLX_CL_DATA_FRAME       : return("CL_DATA_FRAME");
      case DLX_INFO_FRAME          : return("INFO_FRAME");
      case DLX_HALT_DL             : return("HALT_DL");
      case DLX_DL_HALTED           : return("DL_HALTED");
      case DLX_SNA_DGRM            : return("SNA_DGRM");
      case DLX_NB_DGRM             : return("NB_DGRM");
      case DLX_QUERY_DL            : return("QUERY_DL");
      case DLX_QUERY_DL_RESPONSE   : return("QUERY_DL_RESPONSE");
      case DLX_IPM_FRAME           : return("IPM_FRAME");
      case DLX_CAP_XCHANGE         : return("CAP_XCHANGE");
      case DLX_CLOSE_PEER          : return("CLOSE_PEER");
      case DLX_CLOSE_PEER_ACK      : return("CLOSE_PEER_ACK");
      case DLX_ENTER_PEER_BUSY     : return("ENTER_PEER_BUSY");
      case DLX_EXIT_PEER_BUSY      : return("EXIT_PEER_BUSY");
      case DLX_XCAST_FRAME         : return("XCAST_FRAME");
      case DLX_FRAME_REJ           : return("FRAME_REJ");
      case DLX_PEER_TEST_REQ       : return("PEER_TEST_REQ");
      case DLX_PEER_TEST_RSP       : return("PEER_TEST_RSP");
      case DLX_FAST_PASS           : return("FAST_PASS");
      default                      : return("Unknown");
    }
} 

char *csm_stat (int status)
{
    switch (status) {
      case FOUND:
        return ("FOUND");
      case NOT_FOUND:
        return ("NOT_FOUND");
      case UNCONFIRMED:
        return ("UNCONFIRMED");
      case VERIFY:
        return ("VERIFY");
      case SEARCHING:
        return ("SEARCHING");
      case NO_CACHE_INFO:
        return ("NO_CACHE_INFO");
      default:
        return("Unrecognized Cache Status");
    }
}

char *csm_error (int error) 
{
    switch (error) {
      case CSM_INVALID_PARAMS:
        return ("CSM_INVALID_PARAMS");
      case CSM_NO_REACH_ENTRY:
        return ("CSM_NO_REACH_ENTRY");
      case CSM_NO_MTU_SIZE:
        return ("CSM_NO_MTU_SIZE");
      case CSM_NO_RING_MATCH:
        return ("CSM_NO_RING_MATCH");
      case CSM_NO_EXCL_MATCH:
        return ("CSM_NO_EXCL_MATCH");
      case CSM_NONE_AVAILABLE:
        return ("CSM_NONE_AVAILABLE");
      case CSM_FRAME_FILTERED:
        return ("CSM_FRAME_FILTERED");
      default:
        return("Unknown error code");
    }
}

char *csm_nbcom (int nbcom) 
{
    switch (nbcom) {
      case NETBIOS_ADD_GROUP_QUERY:      return("ADD_GROUP_QUERY");
      case NETBIOS_ADD_NAME_QUERY:       return("ADD_NAME_QUERY");
      case NETBIOS_NAME_IN_CONFLICT:     return("NAME_IN_CONFLICT");
      case NETBIOS_STATUS_QUERY:         return("STATUS_QUERY");
      case NETBIOS_TERMINATE_TRACE_REM:  return("TERMINATE_TRACE_REM");
      case NETBIOS_DATAGRAM:             return("DATAGRAM");
      case NETBIOS_DATAGRAM_BROADCAST:   return("DATAGRAM_BROADCAST");
      case NETBIOS_NAME_QUERY:           return("NAME_QUERY");
      case NETBIOS_ADD_NAME_RESPONSE:    return("ADD_NAME_RESPONSE");
      case NETBIOS_NAME_RECOGNIZED:      return("NAME_RECOGNIZED");
      case NETBIOS_STATUS_RESPONSE:      return("STATUS_RESPONSE");
      case NETBIOS_TERMINATE_TRACE_REM_LOC: return("TERMINATE_TRACE_REM_LOC");
      case NETBIOS_DATA_ACK:             return("DATA_ACK");
      case NETBIOS_DATA_FIRST_MIDDLE:    return("DATA_FIRST_MIDDLE");
      case NETBIOS_ONLY_LAST:            return("ONLY_LAST");
      case NETBIOS_SESSION_CONFIRM:      return("SESSION_CONFIRM");
      case NETBIOS_SESSION_END:          return("SESSION_END");
      case NETBIOS_SESSION_INITIALIZE:   return("SESSION_INITIALIZE");
      case NETBIOS_NO_RECEIVE:           return("NO_RECEIVE");
      case NETBIOS_RECEIVE_OUTSTANDING:  return("RECEIVE_OUTSTANDING");
      case NETBIOS_RECEIVE_CONTINUE:     return("RECEIVE_CONTINUE");
      case NETBIOS_SESSION_ALIVE:        return("SESSION_ALIVE");
      default:
        return("Unrecognized command");
    }
}

char *csm_prim (int prim)
{
    switch (prim) {
      case UI:			return("UI");
      case LLC_SIM:             return("RIM/SIM");
      case DM:                  return("DM");
      case LLC_UP:              return("UP");
      case DISC:                return("DISC/RD");
      case UA:                  return("UA");
      case SABME:               return("SABME");
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
      case (UI | P_BIT):			return("UI P/F");
      case (LLC_SIM | P_BIT):             return("RIM/SIM P/F");
      case (DM | P_BIT):                  return("DM P/F");
      case (LLC_UP | P_BIT):              return("UP P/F");
      case (DISC | P_BIT):                return("DISC/RD P/F");
      case (UA | P_BIT):                  return("UA P/F");
      case (SABME | P_BIT):               return("SABME P/F");
      case (SNRM | P_BIT):		return("SNRM P/F");
      case (FRMR | P_BIT):		return("FRMR P/F");
      case (XID | P_BIT):			return("XID P/F");
      case (CFGR | P_BIT):		return("CFGR P/F");
      case (SNRME | P_BIT):		return("SNRME P/F");
      case (LLC_TEST | P_BIT):		return("TEST P/F");
      case (BCN | P_BIT):			return("BCN P/F");
      case (RR | P_BIT):			return("RR P/F");
      case (RNR | P_BIT):			return("RNR P/F");
      case (REJ | P_BIT):			return("REJ P/F");
      default:
        return ("Unrecognized Primitive");
    }
}

/*
 * check for wildcards in string
 */
boolean str_has_wc (char *str)
{
    for ( ; *str; str++)
	if (*str == '*' || *str == '?')
	    return (TRUE);
    return (FALSE);
}

/* 
 * wild card string compare 
 * ignore padding blanks
 */
int wc_str_cmp (char *str1, char *str2)
{
    for ( ; (*str1 == *str2 || *str1 == '?' || *str2 == '?') ; 
                    str1++, str2++) {
        if ((*str1 == '\0') || (*str1 == '*')) {
            return 0;
        }
    }

    if (*str1 == '*' ||
	*str2 == '*' ||
	(*str1 == ' ' && *str2 == 0) ||
	(*str2 == ' ' && *str1 == 0)) {
        return 0;
    }

    return (*str1 - *str2);

}


/*
 * wild card string compare, to compare n bytes
 * ignore padding blanks
 */
int wc_strn_cmp (char *str1, char *str2, int str_size)
{
    int    ii;

    for (ii = 0; ii < str_size; ii++) {
        if (str1[ii] == '*' || str2[ii] == '*') {
            return 0;
        }
        if (str1[ii] == '?' || str2[ii] == '?') {
            continue;
        }
        if (str1[ii] != str2[ii]) {
	    if ((str1[ii] == ' ' && str2[ii] == 0) ||
		(str2[ii] == ' ' && str2[ii] == 0))
		 return (0);
	    else
        	return (str1[ii] - str2[ii]);
        }
    }

    return 0;

}



/* 
 * compare string applying mask
 */
int masked_mac_cmp (char *mac1, char *mac2, char *mask)
{
    int    rc;
    long   aa, bb, mm;

    aa = GETSHORT(mac1 + 0);
    bb = GETSHORT(mac2 + 0);
    mm = GETSHORT(mask + 0);
    rc = (aa & mm) - (bb & mm);
    if (rc == 0) {
        aa = GETLONG(mac1 + 2);
        bb = GETLONG(mac2 + 2);
        mm = GETLONG(mask + 2);
        rc = (aa & mm) - (bb & mm);
    }

    return rc;
}



/*
 * get_dlx_packet()
 *
 * Allocate a packet for a DLX message and initialize the DLX header.
 * Allocates extra space in front of packet for border peer
 * encapsulation header.
 */
paktype *get_dlx_packet (uchar msg_type, ulong pkt_size,
                         ulong session_id, uchar ipm_flags)
{
    paktype *pak;
    dlx_header_t *dlx_hdr;

    pak = getbuffer(pkt_size);
    if (!pak) {
        return(NULL);
    }
    pak->datagramsize = pkt_size;
    dlx_hdr = (dlx_header_t *) pak->datagramstart;
    dlx_hdr->packet_length = pkt_size;
    dlx_hdr->proto_ver_id = DLX_PROTO_VERSION;
    dlx_hdr->message_type = msg_type;
    dlx_hdr->session_id = session_id;
    dlx_hdr->ipm_flags = ipm_flags;
    dlx_hdr->return_code = 0;
    dlx_hdr->csco_reserved[0] = 0;
    dlx_hdr->csco_reserved[1] = 0;
    return(pak);
}




/*
 * get paktype *pak, for ssp message
 */
paktype *get_ssp_packet (uchar message_type, ulong udata_size, uint rem_corr, 
        		ulong portid, uchar fc_flags)
{
    int    hdr_len;
    char    ctrl_msg;
    paktype    *pak;
    ssp_control_frame_t    *ssp_hdr;

    if ((is_ssp_ctl_hdr(message_type)) || (is_ssp_ctl_mac_hdr(message_type))) {
        ctrl_msg = TRUE;
        hdr_len = sizeof(ssp_control_frame_t);
    } else {
        ctrl_msg = FALSE;
        hdr_len = sizeof(ssp_i_frame_t);
    }
    
    pak = getbuffer(hdr_len + udata_size);
    if (!pak) {
        return(NULL);
    }
    pak->datagramsize = hdr_len + udata_size;
    ssp_hdr = (ssp_control_frame_t *) pak->datagramstart;

    ssp_hdr->info_hdr.version = SSP_VERS;
    ssp_hdr->info_hdr.header_len = hdr_len;
    ssp_hdr->info_hdr.message_len = udata_size;
    ssp_hdr->info_hdr.rem_ckt_id.dlc_corr = rem_corr;
    ssp_hdr->info_hdr.rem_ckt_id.dlc_port_id = portid;
    ssp_hdr->info_hdr.msg_type1 = message_type;
    ssp_hdr->info_hdr.flow_control = fc_flags;

    if (ctrl_msg) {
        ssp_hdr->protocol_id = SSP_PROTOCOL_ID;
        ssp_hdr->header_number = SSP_HDR_NUM;
        ssp_hdr->msg_type2 = message_type;
	ssp_hdr->dlc_header_len = 0;
        ssp_hdr->ckt_priority = SSP_PRIORITY_UNSUPPORTED;
    }

    return(pak);

}


/*
 * form ssp message out of token ring packet
 */
boolean mk_ssp_cntrl_header_from_clsipkt (ssp_control_frame_t *fptr, 
            	                          TCLSIMsg *clsi_msg,
            	                          char direction)
{

    char         *daddr;
    char         *saddr;
    char         dsap;
    char         ssap;

    if ((fptr == NULL) || (clsi_msg == NULL)) {
        return FALSE;
    }

    daddr = get_daddr_from_clsi(clsi_msg);
    saddr = get_saddr_from_clsi(clsi_msg);
    dsap = get_dsap_from_clsi(clsi_msg);
    ssap = get_ssap_from_clsi(clsi_msg);

    if (direction == SSP_DIRN_FORWARD) {
        ieee_copy(daddr, fptr->dlc_id.target_mac_address);
        ieee_copy(saddr, fptr->dlc_id.origin_mac_address);
        fptr->dlc_id.origin_link_sap = ssap;
        fptr->dlc_id.target_link_sap = dsap;
    } else {
        ieee_copy(saddr, fptr->dlc_id.target_mac_address);
        ieee_copy(daddr, fptr->dlc_id.origin_mac_address);
        fptr->dlc_id.origin_link_sap = dsap;
        fptr->dlc_id.target_link_sap = ssap;
    }
    fptr->frame_direction = direction;
    
    return TRUE;
}




/*
 * Free CLSI message
 */
void free_clsi_msg (TCLSIMsg *clsi_msg)
{
    CLSIMsgDestroy(clsi_msg);
    return;
}

/*
 * Add location cache for a given mac address in the cache
 */
void dlsw_core2csm_update_cache(csm_op_t op, 
				       resource_type_t loc_rem,
				       uchar dirn,
				       char *src_adress, 
				       char *dest_adress, 
				       dlsw_ring_t *port_id,
				       peerhandle_t peer_handle,
				       uchar *local_rif,
				       uchar *data_ptr)
{
  return;
}

/*
 * Add location cache for a given mac address in the
 * LCACHE_MAC file or RCACHE_MAC file depending upon the
 * reach direction
 */
void mac_found_update_cache (char *addr, char reach_direction, 
                             u_portid_type_t *prp_id, char *rif, long mtu_size)
{

    if (reach_direction == LOCAL) {
        update_csm_cache(LCACHE_MAC, addr, prp_id, rif, mtu_size);
    } else {    /* REMOTE */
        update_csm_cache(RCACHE_MAC, addr, prp_id, rif, mtu_size);
    }
    return;
}



/*
 * Add location cache for a given netbios name in the
 * LCACHE_NAME file or RCACHE_NAME file depending upon the
 * reach direction
 */
void name_found_update_cache (char *addr, char reach_direction, 
                             u_portid_type_t *prp_id, char *rif, long mtu_size)
{

    if (reach_direction == LOCAL) {
        update_csm_cache(LCACHE_NAME, addr, prp_id, rif, mtu_size);
    } else {    /* REMOTE */
        update_csm_cache(RCACHE_NAME, addr, prp_id, rif, mtu_size);
    }
    return;
}




/*
 * Add location cache learned dynamically
 */
void update_csm_cache (db_master_t *file_no, char *name, 
                       u_portid_type_t *prp_id, char *rif, long mtu_size)
{

    int           ii, ws_ii;
    boolean       match_found;
    char          ws_status;
    record_handle rhandle;
    lcache_t      *cache_ptr, ws_cache;
    ushort        reach_cost;
    char          *str;

    match_found = FALSE;
    ws_ii = -1;
    reach_cost = 0;

    if (file_no == LCACHE_MAC) {
        CSMV_DEBUG("\nCSM: update local cache for mac %e, %s",
                   name, prp_id->port_id->swidb->hwptr->hw_namestring);
    } else if (file_no == RCACHE_MAC) {
        str = string_getnext();
        CSMV_DEBUG("\nCSM: update remote cache for mac %e, %s",
                   name, peer_handle_to_str(prp_id->peer_id, str));
    } else if (file_no == LCACHE_NAME) {
        NETB_CSMV_DEBUG("\nCSM: update local cache for name %15s, %s",
                   name, prp_id->port_id->swidb->hwptr->hw_namestring);
    } else {     /* RCACHE_NAME */
        str = string_getnext();
        NETB_CSMV_DEBUG("\nCSM: update remote cache for name %15s, %s",
                   name, peer_handle_to_str(prp_id->peer_id, str));
    }

    rhandle = seek_record(file_no, name, (void **) &cache_ptr);

    if (rhandle == NULL) {
        ws_status = NO_CACHE_INFO;
    } else {
        ws_status = cache_ptr->status;
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if ((file_no == LCACHE_MAC) || (file_no == LCACHE_NAME)) {
                if (cache_ptr->r_entry[ii].prp_id.port_id == prp_id->port_id) {
                    match_found = TRUE;
                    ws_ii = ii;
                    break;
                } else if (cache_ptr->r_entry[ii].flags == 0) {
                    if (ws_ii == -1) {
                        ws_ii = ii;
                    }
                }
            } else {
                if (cache_ptr->r_entry[ii].prp_id.peer_id == prp_id->peer_id) {
                    match_found = TRUE;
                    ws_ii = ii;
                    break;
                } else if (cache_ptr->r_entry[ii].flags == 0) {
                    if (ws_ii == -1) {
                        ws_ii = ii;
                    }
                }
            }
        }
	if ((match_found == FALSE) && (ws_ii < 0) && 
	    (cache_ptr->no_in_list < MAX_REACH)) {
	    ws_ii = cache_ptr->no_in_list;
	}

    }

    if ((file_no == RCACHE_MAC) || (file_no == RCACHE_NAME)) {
	reach_cost = peer_get_cost(prp_id->peer_id);
    } else {
	reach_cost = 0;
    }

    switch (ws_status) {
      case NO_CACHE_INFO:
        /* add a new record */
        ws_cache.cache_type = DYNAMIC;
        ws_cache.status = FOUND;
        if ((file_no == LCACHE_MAC) || (file_no == LCACHE_NAME)) {
            /* Remote direction later, when wc is complete */
            ws_cache.reach_direction = LOCAL;
            ws_cache.r_entry[0].prp_id.port_id = prp_id->port_id;
        } else {
            ws_cache.reach_direction = REMOTE;
            ws_cache.r_entry[0].prp_id.peer_id = prp_id->peer_id;
        }
	ws_cache.no_in_list = 1;
	ws_cache.r_entry[0].reach_cost = reach_cost;
        memset(ws_cache.r_entry[0].rif, 0, SRB_MAXRIF);
        if (rif != NULL) {
            bcopy(rif, ws_cache.r_entry[0].rif, SRB_MAXRIF);
        }
        ws_cache.r_entry[0].mtu_size = mtu_size;
        ws_cache.r_entry[0].flags = AVAILABLE;
        for (ii = 1; ii < MAX_REACH; ii++) {
            ws_cache.r_entry[ii].prp_id.peer_id = 0;
            ws_cache.r_entry[ii].flags = 0;
        }
        memset(&ws_cache.del_tstamp, 0, sizeof(ws_cache.del_tstamp));
        memset(&ws_cache.explore_tstamp, 0, sizeof(ws_cache.explore_tstamp));
        rhandle = add_record(file_no, name, &ws_cache);
        if (rhandle == NULL) {
            CSME_DEBUG("\nCSM: Error adding cache info - update_csm_cache");
        } else {
            lcache_t     *tmp_ptr;
            tmp_ptr = (lcache_t *) rhandle_to_data(rhandle);
            /* initialise timers */
            if (file_no == LCACHE_MAC) {
                mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                    &csm_cache_expl_timer,
                                    CSM_LMAC_CACHE, rhandle, FALSE);
                mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                    &csm_cache_del_timer,
                                    CSM_LMAC_CACHE, rhandle, FALSE);
                mgd_timer_start(&tmp_ptr->del_tstamp, csm_sna_cache_to);
            } else if (file_no == RCACHE_MAC) {
                mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                    &csm_cache_expl_timer,
                                    CSM_RMAC_CACHE, rhandle, FALSE);
                mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                    &csm_cache_del_timer,
                                    CSM_RMAC_CACHE, rhandle, FALSE);
                mgd_timer_start(&tmp_ptr->del_tstamp, csm_sna_cache_to);
            } else if (file_no == LCACHE_NAME) {       
                mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                    &csm_cache_expl_timer,
                                    CSM_LNAME_CACHE, rhandle, FALSE);
                mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                    &csm_cache_del_timer,
                                    CSM_LNAME_CACHE, rhandle, FALSE);
                mgd_timer_start(&tmp_ptr->del_tstamp, csm_nb_cache_to);
            } else {      /* RCACHE_NAME */
                mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                    &csm_cache_expl_timer,
                                    CSM_RNAME_CACHE, rhandle, FALSE);
                mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                    &csm_cache_del_timer,
                                    CSM_RNAME_CACHE, rhandle, FALSE);
                mgd_timer_start(&tmp_ptr->del_tstamp, csm_nb_cache_to);
            }
            GET_TIMESTAMP(tmp_ptr->verify_tstamp);
            mgd_timer_stop(&tmp_ptr->explore_tstamp);
        }
        break;

      case SEARCHING:
      case NOT_FOUND:
	/* there cannot be any items in the reach entry list in this case */
	cache_ptr->no_in_list = 1;
        if ((file_no == LCACHE_MAC) || (file_no == RCACHE_MAC)) {
            mgd_timer_start(&cache_ptr->del_tstamp, csm_sna_cache_to);
        } else {     /* LCACHE_NAME or RCACHE_NAME */
            mgd_timer_start(&cache_ptr->del_tstamp, csm_nb_cache_to);
        }
        GET_TIMESTAMP(cache_ptr->verify_tstamp);
	mgd_timer_stop(&cache_ptr->explore_tstamp);
        if ((file_no == LCACHE_MAC) || (file_no == LCACHE_NAME)) {
            cache_ptr->r_entry[0].prp_id.port_id = prp_id->port_id;
        } else {
            cache_ptr->r_entry[0].prp_id.peer_id = prp_id->peer_id;
        }
	cache_ptr->r_entry[0].reach_cost = reach_cost;
        memset(cache_ptr->r_entry[0].rif, 0, SRB_MAXRIF);
        if (rif != NULL) {
            bcopy(rif, cache_ptr->r_entry[0].rif, SRB_MAXRIF);
        }
        cache_ptr->r_entry[0].mtu_size = mtu_size;
        cache_ptr->r_entry[0].flags = AVAILABLE;
        for (ii = 1; ii < MAX_REACH; ii++) {
            cache_ptr->r_entry[ii].prp_id.peer_id = 0;
            cache_ptr->r_entry[ii].flags = 0;
        }
        cache_ptr->status = FOUND;
        break;

      case FOUND:
      case VERIFY:
        if (ws_status == VERIFY) {
            cache_ptr->status = FOUND;
        }
        if (match_found) {
            cache_ptr->r_entry[ws_ii].flags |= AVAILABLE;
            if ((rif != NULL) && (cache_ptr->r_entry[ws_ii].rif[0] == 0)) {
                bcopy(rif, cache_ptr->r_entry[ws_ii].rif, SRB_MAXRIF);
            }
            if (cache_ptr->r_entry[ws_ii].mtu_size < mtu_size) {
                cache_ptr->r_entry[ws_ii].mtu_size = mtu_size;
            }
	    cache_ptr->r_entry[ws_ii].reach_cost = reach_cost;
        } else {
            if ((ws_ii >= 0) && (cache_ptr->cache_type == DYNAMIC)) {
                if ((file_no == LCACHE_MAC) || (file_no == LCACHE_NAME)) {
                    cache_ptr->r_entry[ws_ii].prp_id.port_id = prp_id->port_id;
                } else {
                    cache_ptr->r_entry[ws_ii].prp_id.peer_id = prp_id->peer_id;
                }
                if (rif != NULL) {
                    bcopy(rif, cache_ptr->r_entry[ws_ii].rif, SRB_MAXRIF);
                }
                cache_ptr->r_entry[ws_ii].flags |= AVAILABLE;
                cache_ptr->r_entry[ws_ii].mtu_size = mtu_size;
	        cache_ptr->r_entry[ws_ii].reach_cost = reach_cost;
		++cache_ptr->no_in_list;
            }
        }
        mgd_timer_stop(&cache_ptr->explore_tstamp);
        break;

      case UNCONFIRMED:
        if (match_found) {
	    cache_ptr->status = FOUND;
            cache_ptr->r_entry[ws_ii].flags |= AVAILABLE;
            if ((rif != NULL) && (cache_ptr->r_entry[ws_ii].rif[0] == 0)) {
                bcopy(rif, cache_ptr->r_entry[ws_ii].rif, SRB_MAXRIF);
            }
            if (cache_ptr->r_entry[ws_ii].mtu_size < mtu_size) {
                cache_ptr->r_entry[ws_ii].mtu_size = mtu_size;
            }
	    cache_ptr->r_entry[ws_ii].reach_cost = reach_cost;
        }
        mgd_timer_stop(&cache_ptr->explore_tstamp);
	break;

      default:
        break;
    }

    return;
}

/*
 * delete_csm_cache
 *   file_no            : LCACHE_MAC, RCACHE_MAC, LCACHE_NAME or RCACHE_NAME
 *   name		: pointer to the Mac Address or the NB Name
 *				NULL to delete ALL entries
 *   prp_id		: pointe port or peer id union - NULL to delete all
 *				If reach_direction is NOT set, do not set
 *				this to any value other than NULL
 *   force_flag		: TRUE to force delete USER_CONF and CAPEX entries
 *   cache_type_flag	: &ed for all the types (CAPEX .. etc) of cache
 *				to be deleted
 *
 * return code	: VOID
 */
void delete_csm_cache (db_master_t *file_no, char *name, 
		       u_portid_type_t *prp_id, char force_flag,
                       char cache_type_flag)
{

    int           ii, tii;
    record_handle rhandle;
    lcache_t      *cache_ptr;
    lportid_t     port_id;
    peerhandle_t  peer_id;
    char          *str;

    if (prp_id != NULL) {
        if ((file_no == LCACHE_MAC) || (file_no == LCACHE_NAME)) {
            port_id = prp_id->port_id;
            peer_id = 0;
        } else {
            peer_id = prp_id->peer_id;
            port_id = NULL;
        }
    } else {
        peer_id = 0;
        port_id = NULL;
    }

    if (name != NULL) {
	if (cache_type_flag & SAP_PEND) {
	    CSMV_DEBUG("\nCSM: delete inactive static entry");
        } else if (file_no == LCACHE_MAC) {
            CSMV_DEBUG("\nCSM: delete local cache for %x:%x:%x, %s",
                       GETSHORT(name+0), GETSHORT(name+2), GETSHORT(name+4),
                       (port_id ? port_id->swidb->hwptr->hw_namestring :
				  "all local ports"));
        } else if (file_no == RCACHE_MAC) {
            str = string_getnext();
            CSMV_DEBUG("\nCSM: delete remote cache for %x:%x:%x, %s",
                      GETSHORT(name+0), GETSHORT(name+2), GETSHORT(name+4),
                      (peer_id ? peer_handle_to_str(peer_id, str) :
				 "all remote peers"));
        } else if (file_no == LCACHE_NAME) {
            NETB_CSMV_DEBUG("\nCSM: delete local cache for %15s, %s", name,
                       (port_id ? port_id->swidb->hwptr->hw_namestring :
				  "all local ports"));
        } else {   /* RCACHE_NAME */
            str = string_getnext();
            NETB_CSMV_DEBUG("\nCSM: delete remote cache for %15s, %s", name,
                      (peer_id ? peer_handle_to_str(peer_id, str) :
				 "all remote peers"));
        }
    } else {
	if (cache_type_flag & SAP_PEND) {
	    CSMV_DEBUG("\nCSM: delete inactive static entry");
        } else if (file_no == LCACHE_MAC) {
            CSMV_DEBUG("\nCSM: delete local mac cache for %s", 
                       (port_id ? port_id->swidb->hwptr->hw_namestring :
				  "all local ports"));
        } else if (file_no == RCACHE_MAC) {
            str = string_getnext();
            CSMV_DEBUG("\nCSM: delete remote mac cache for %s", 
                      (peer_id ? peer_handle_to_str(peer_id, str) :
				 "all remote peers"));
        } else if (file_no == LCACHE_NAME) {
            NETB_CSMV_DEBUG("\nCSM: delete local name cache for %s", 
                       (port_id ? port_id->swidb->hwptr->hw_namestring :
				  "all local ports"));
        } else {
            str = string_getnext();
            NETB_CSMV_DEBUG("\nCSM: delete remote name cache for %s", 
                      (peer_id ? peer_handle_to_str(peer_id, str) :
				 "all remote peers"));
        }
    }

    if (name != NULL) {
        rhandle = seek_record(file_no, name, (void **) &cache_ptr);
    } else {
        rhandle = read_next_record(file_no, NULL, NULL, (void **) &cache_ptr);
    }

    while (rhandle != NULL) {
        if ((cache_type_flag != 0) &&
                    (!(cache_ptr->cache_type & cache_type_flag))) {
            rhandle = read_next_record(file_no, rhandle, name,
                                       (void **) &cache_ptr);
            continue;
        }

        if ((port_id == NULL) && (peer_id == 0)) {
            if ((cache_ptr->cache_type == DYNAMIC) || (force_flag)) {
                mgd_timer_stop(&cache_ptr->explore_tstamp);
                mgd_timer_stop(&cache_ptr->del_tstamp);
                rhandle = delete_record_get_next(file_no, rhandle);
                cache_ptr = rhandle_to_data(rhandle);
                continue;
            } else {
                for (tii = 0; tii < cache_ptr->no_in_list; tii++) {
                    cache_ptr->r_entry[tii].flags &= ~AVAILABLE;
                }
                cache_ptr->status = UNCONFIRMED;
            }
        } else {
	    for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
                if ((((cache_ptr->reach_direction == LOCAL) && 
                      (cache_ptr->r_entry[ii].prp_id.port_id == port_id))) ||
                     ((cache_ptr->reach_direction == REMOTE) &&
                      (cache_ptr->r_entry[ii].prp_id.peer_id == peer_id))) {
                    if ((cache_ptr->cache_type == DYNAMIC) || (force_flag)) {
		        for (tii = ii; tii < cache_ptr->no_in_list; tii++) {
		            if (tii == (cache_ptr->no_in_list - 1)) {
			        memset(&cache_ptr->r_entry[tii], 0, 
                                      sizeof(reach_entry_t));
                            } else {
			        bcopy(&cache_ptr->r_entry[tii + 1],
			              &cache_ptr->r_entry[tii], 
                                      sizeof(reach_entry_t));
                            }
                        }
		        --cache_ptr->no_in_list;
                        break;
                    } else {    /* force_flag == FALSE */
                        cache_ptr->r_entry[ii].flags &= ~AVAILABLE;
                        cache_ptr->r_entry[ii].mtu_size = 0;
                        for (tii = 0; tii < cache_ptr->no_in_list; tii++) {
                            if (cache_ptr->r_entry[tii].flags & AVAILABLE) {
                                break;
                            }
                        }
                        if (tii >= cache_ptr->no_in_list) {
                            /* not even one cache_entry AVAILABLE */
                            if ((peer_id) &&
                                (dlsw_is_peer_being_removed(peer_id))) {
                                 /* schedule entry to be removed below */
                                 cache_ptr->no_in_list = 0;
                            } else {
                                 cache_ptr->status = UNCONFIRMED;
                            }
                        }
                    }
                    break;
                }
            }
            if (cache_ptr->no_in_list == 0) {
                mgd_timer_stop(&cache_ptr->explore_tstamp);
                mgd_timer_stop(&cache_ptr->del_tstamp);
                rhandle = delete_record_get_next(file_no, rhandle);
                cache_ptr = rhandle_to_data(rhandle);
                continue;
            }
        }
	rhandle = read_next_record(file_no, rhandle, name,
				       (void **) &cache_ptr);
    }

    return;
}



/*
 * Get a reachability entry for a given mac address.
 * Gives the reachability information, checking for
 * load balancing etc.
 * If ring list match is required, send in the peer_id or port_id, the
 * frame was received from.
 */
int get_reach_entry_mac(lcache_t *cache_ptr, long mtu_size, 
                        u_portid_type_t *prp_id, boolean update_use_flag)
{
    int              ii, ws_ii;
    ushort           ws_cost;
    boolean          available_flag, use_bit_flag, ring_match_found;
    boolean          mtu_size_found;
    peerhandle_t     peer_handle;
    lportid_t        port_id;

    if (cache_ptr == NULL) {
        SNA_CSME_DEBUG("\nCSM: get_reach_entry_mac recd. NULL cache_ptr");
        return CSM_INVALID_PARAMS;
    }

    if (cache_ptr->no_in_list <= 0) {
        return CSM_NO_REACH_ENTRY;
    }

    ws_cost = 0;
    available_flag = mtu_size_found = use_bit_flag = FALSE;
    ws_ii = CSM_NO_REACH_ENTRY;

    /*
     * if mtu_size is set to 0, set it to 1 
     * this will prevent us from selecting an entry with mtu_size
     * not set or set to 0.
     */
    if (mtu_size == 0) {
        mtu_size = 1;
    }
    if (prp_id != NULL) {
        /* see if there is atleast one entry that passes the
         * ring list association test!
         */
        ring_match_found = FALSE;
        if (cache_ptr->reach_direction == LOCAL) {
            port_id = NULL;
            peer_handle = prp_id->peer_id;
            for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
                port_id = cache_ptr->r_entry[ii].prp_id.port_id;
                ring_match_found = peer_pass_port_filter(peer_handle, port_id);
                if (ring_match_found) {
                    break;
                }
            }
        } else {
            port_id = prp_id->port_id;
            peer_handle = 0;
            for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
                peer_handle = cache_ptr->r_entry[ii].prp_id.peer_id,
                ring_match_found = peer_pass_port_filter(peer_handle, port_id);
                if (ring_match_found) {
                    break;
                }
            }
        }
        if (ring_match_found == FALSE) { 
            return CSM_NO_RING_MATCH;
        }
    } else {
        port_id = NULL;
        peer_handle = 0;
    }

    if (csm_load_balance == FALSE) {
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if (cache_ptr->r_entry[ii].mtu_size) {
                mtu_size_found = TRUE;
            }
            if (cache_ptr->r_entry[ii].flags & AVAILABLE) {
                available_flag = TRUE;
                if (cache_ptr->r_entry[ii].mtu_size >= mtu_size) {
	            if (cache_ptr->reach_direction == LOCAL) {
		        /* no cost info for LAN */
                        port_id = cache_ptr->r_entry[ii].prp_id.port_id;
                        if (peer_pass_port_filter(peer_handle, port_id)) {
                            ws_ii = ii;
                            break;
                        }
                    } else {
                        peer_handle = cache_ptr->r_entry[ii].prp_id.peer_id;
                        if (peer_pass_port_filter(peer_handle, port_id)) {
	                    if ((cache_ptr->r_entry[ii].reach_cost < ws_cost) ||
		                                    (ws_cost == 0)) {
		                ws_cost = cache_ptr->r_entry[ii].reach_cost;
                                ws_ii = ii;
                            }
                        }
                    }
                }
            }
        }
        if (ws_ii >= 0) {
            return ws_ii;
        } else if (!mtu_size_found) {
            return CSM_NO_MTU_SIZE;
        } else if (!available_flag) {
            return CSM_NONE_AVAILABLE;
        } else {
            return CSM_NO_REACH_ENTRY;
        }
    } else {
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if (cache_ptr->r_entry[ii].mtu_size) {
                mtu_size_found = TRUE;
            }
            if (cache_ptr->r_entry[ii].flags & AVAILABLE) {
                available_flag = TRUE;
                if (cache_ptr->r_entry[ii].mtu_size >= mtu_size) {
                    if (cache_ptr->reach_direction == LOCAL) {
                        port_id = cache_ptr->r_entry[ii].prp_id.port_id;
                    } else {
                        peer_handle = cache_ptr->r_entry[ii].prp_id.peer_id;
                    }
                    if (peer_pass_port_filter(peer_handle, port_id)) {
                        if (cache_ptr->r_entry[ii].flags & USE_BIT) {
                            /* turn off use bit flag */
                            if (update_use_flag) {
                                cache_ptr->r_entry[ii].flags &= ~USE_BIT;
                            }
                            use_bit_flag = TRUE;
                        } else if (use_bit_flag) {
                            if (update_use_flag) {
                                cache_ptr->r_entry[ii].flags |= USE_BIT;
                            }
                            return ii;
                        }
                    }
                }
            }
        }
        /*
         * if it gets to this point, if the use bit is set,
         * the last used entry was probably the only one or the
         * MAX_REACH'th entry in the table. Go through the table
         * and return the first available entry
         */
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if (cache_ptr->r_entry[ii].flags & AVAILABLE) {
                if (cache_ptr->r_entry[ii].mtu_size >= mtu_size) {
                    if (cache_ptr->reach_direction == LOCAL) {
                        port_id = cache_ptr->r_entry[ii].prp_id.port_id;
                    } else {
                        peer_handle = cache_ptr->r_entry[ii].prp_id.peer_id;
                    }
                    if (peer_pass_port_filter(peer_handle, port_id)) {
                        if (update_use_flag) {
                            cache_ptr->r_entry[ii].flags |= USE_BIT;
                        }
                        return ii;
                    }
                }
            }
        }
    }

    /*
     * If it gets to this point, there is no r_entry which is
     * available, in the cache_ptr passed to us. So return -1
     */
    if (!mtu_size_found) {
        return CSM_NO_MTU_SIZE;
    } else if (!available_flag) {
        return CSM_NONE_AVAILABLE;
    } else {
        return CSM_NO_REACH_ENTRY;
    }

}

int ssp_lf_bits_to_bytes (uchar ssp_lf_bits)
{
    switch (ssp_lf_bits & SSP_LF_BASE_MASK) {
        case SSP_LF516:     return(RC_LF516_VAL);
        case SSP_LF1500:    return(RC_LF1500_VAL);
        case SSP_LF2052:    return(RC_LF2052_VAL);
        case SSP_LF4472:    return(RC_LF4472_VAL);
        case SSP_LF8144:    return(RC_LF8144_VAL);
        case SSP_LF11407:   return(RC_LF11407_VAL);
        case SSP_LF17800:   return(RC_LF17800_VAL);
        case SSP_LFMAX:     return(RC_LF17800_VAL);
        default :           return(-1);
    }
}

uchar bytes_to_ssp_lf_bits (int bytes)
{
    switch (bytes) {
        case RC_LF516_VAL:     return(SSP_LF516);
        case RC_LF1500_VAL:    return(SSP_LF1500);
        case RC_LF2052_VAL:    return(SSP_LF2052);
        case RC_LF4472_VAL:    return(SSP_LF4472);
        case RC_LF8144_VAL:    return(SSP_LF8144);
        case RC_LF11407_VAL:   return(SSP_LF11407);
        case RC_LF17800_VAL:   return(SSP_LF17800);
        default:               return(0);
    }
}

uchar cisco_to_1795_lf_bits (uchar ssp_lf_bits)
{
    switch (ssp_lf_bits & RC_LFMASK) {
        case RC_LF516:     return(SSP_LF516);
        case RC_LF1500:    return(SSP_LF1500);
        case RC_LF2052:    return(SSP_LF2052);
        case RC_LF4472:    return(SSP_LF4472);
        case RC_LF8144:    return(SSP_LF8144);
        case RC_LF11407:   return(SSP_LF11407);
        case RC_LF17800:   return(SSP_LF17800);
        case RC_LFINITIAL: return(SSP_LF17800);
        default:           return(0);
    }
}


/*
 * Get the local and remote reachability cache entries from the
 * lcache_mac database. If the station has reachable entries in
 * both local and remote cases, there will be two records in
 * the db next to each other. But there should be only one
 * record for each case.
 */
void get_mac_cache_entries(char *mac_addr, 
			   lcache_t **local_cache, record_handle *l_rhandle,
                           lcache_t **remote_cache, record_handle *r_rhandle)
{
    if ((local_cache != NULL) || (l_rhandle != NULL)) {
        get_csm_cache_entries(LCACHE_MAC, mac_addr, local_cache, l_rhandle);
    }
    if ((remote_cache != NULL) || (r_rhandle != NULL)) {
        get_csm_cache_entries(RCACHE_MAC, mac_addr, remote_cache, r_rhandle);
    }
    return;
}

void get_name_cache_entries(char *name, 
			    lcache_t **local_cache, record_handle *l_rhandle,
                            lcache_t **remote_cache, record_handle *r_rhandle)
{
    if ((local_cache != NULL) || (l_rhandle != NULL)) {
        get_csm_cache_entries(LCACHE_NAME, name, local_cache, l_rhandle);
    }
    if ((remote_cache != NULL) || (r_rhandle != NULL)) {
        get_csm_cache_entries(RCACHE_NAME, name, remote_cache, r_rhandle);
    }
    return;
}


void get_csm_cache_entries(db_master_t *file_num, char *mac_addr,
                           lcache_t **cache_ptr, record_handle *rhandle_ptr)
{
    record_handle  tmp_rhandle;
    lcache_t       *tmp_cache_ptr;
 
 
    if ((mac_addr == NULL) || ((cache_ptr == NULL) && (rhandle_ptr == NULL))) {
        return;
    }
 
    tmp_rhandle = seek_record(file_num, mac_addr, (void **) &tmp_cache_ptr);
    if (tmp_rhandle) {
        if (cache_ptr != NULL) {
            *cache_ptr = tmp_cache_ptr;
        }
        if (rhandle_ptr != NULL) {
            *rhandle_ptr = tmp_rhandle;
        }
    } else {
        if (cache_ptr != NULL) {
            *cache_ptr = NULL;
        }
        if (rhandle_ptr != NULL) {
            *rhandle_ptr = NULL;
        }
    }
    return;
}
 
 
#ifdef CSM_WILD_CARD_SUPPORT
/*
 * csm_add_mac_wc_entry()
 * add info to the ambiguous mac cache db
 * The entries are sorted based on ambiguity. The least ambiguous first
 * and so on. 
 */
void csm_add_wc_mac_entry (char *mac_addr, char *mac_mask, 
                           cache_source_t source, reach_sense sense,
                           object_type_t obj_type, config_obj_u obj)
{

    int              rif_len;
    record_handle    rhandle;
    mac_cache_key_t  mac_key;
    wc_mac_cache_t   wc_cache, *wc_cache_ptr;
    boolean          add_info;


    CSMV_DEBUG("\nAdding WC Reach entry for mac-%e, mask-%e, source-%x",
                          mac_addr, mac_mask, source);

    add_info = TRUE;

    /* set up the key */
    ieee_copy(mac_addr, mac_key.fx_mac_addr);
    if (mac_mask != NULL) {
        ieee_copy(mac_mask, mac_key.fx_mac_mask);
    } else {
        ieee_copy(def_mac_mask, mac_key.fx_mac_mask);
    }
    reverse_mac_mask(mac_key.fx_mac_mask);

    rhandle = seek_record(WCACHE_MAC, (char *) &mac_key, 
                                      (void **) &wc_cache_ptr);
    add_info = TRUE;
    /*
     * we already have some configuration information for the
     * same resource. If it the exact same info. print out a
     * mesage and do not add to the db. otherwise just add it.
     * But read all the records matching the key to make sure
     * that there are no duplicates.
     */
    while (rhandle != NULL) {
        if ((wc_cache_ptr->cache_type == source) &&
                   (wc_cache_ptr->obj_type == obj_type)) {
            switch (obj_type) {
              case RING_ID:
                if (wc_cache_ptr->obj.ring_id == obj.ring_id) {
                    add_info = FALSE;
                }
                break;
              case PEER_ID:
                if (wc_cache_ptr->obj.peer_id == obj.peer_id) {
                    add_info = FALSE;
                }
                break;
              case PEER_GROUP_ID:
                if (wc_cache_ptr->obj.ring_group_id == obj.peer_group_id) {
                    add_info = FALSE;
                }
                break;
              case RING_GROUP_ID:
                if (wc_cache_ptr->obj.ring_group_id == obj.ring_group_id) {
                    add_info = FALSE;
                }
                break;
              case RIF_STRING:
                rif_len = RIF_LENGTH(obj.rif[0]);
                if (!(bcmp(wc_cache_ptr->obj.rif, obj.rif, rif_len))) {
                    add_info = FALSE;
                }
                break;
              case PEER_IP_ADDR:
                if (wc_cache_ptr->obj.peer_ip_addr == obj.peer_ip_addr) {
                    add_info = FALSE;
                }
                break;
              case PEER_HWIDB:
                if (wc_cache_ptr->obj.peer_hwidb == obj.peer_hwidb) {
                    add_info = FALSE;
                }
                break;
              default:
                break;
            }
            if (add_info == FALSE) {
                CSMV_DEBUG("\nCSM: Duplicate Reachability info....");
                break;
            }
        }
        rhandle = read_next_record(WCACHE_MAC, rhandle, (char *) &mac_key,
                                   (void **) &wc_cache_ptr);
    }

    if (add_info) {
        ieee_copy(mac_addr, wc_cache.mac_addr);
        if (mac_mask != NULL) {
            ieee_copy(mac_mask, wc_cache.mac_mask);
        } else {
            ieee_copy(def_mac_mask, wc_cache.mac_mask);
        }
        wc_cache.obj_type = obj_type;
        wc_cache.obj = obj;
        wc_cache.cache_type = source;
        wc_cache.sense = sense;

        rhandle = add_record(WCACHE_MAC, (char *) &mac_key, (void *) &wc_cache);

        if (rhandle) {
            CSMV_DEBUG("\nCSM: Wild Card Reachability info added....");
        } else {
            CSME_DEBUG("\nCSM: Error adding Wild Card Reachability info ....");
        }
    }

    return;
}
#endif

#ifdef CSM_WILD_CARD_SUPPORT
/*
 * csm_delete_wc_entry()
 *
 * delete ambiguous cache entry
 */
void csm_delete_wc_mac_entry (char *mac_addr, char *mac_mask, 
                              cache_source_t source, 
                              object_type_t obj_type, config_obj_u obj)
{

    int              rif_len;
    record_handle    rhandle;
    mac_cache_key_t  mac_key;
    wc_mac_cache_t   *wc_cache_ptr;
    boolean          del_info;


    del_info = FALSE;

    CSMV_DEBUG("\nCSM: Deleting WC Reach entry for mac-%e, mask-%e, source-%x",
                          mac_addr, mac_mask, source);

    rhandle = NULL;
    if ((mac_addr != NULL) && (mac_mask != NULL)) {
        /* set up the key */
        ieee_copy(mac_addr, mac_key.fx_mac_addr);
        ieee_copy(mac_mask, mac_key.fx_mac_mask);
        reverse_mac_mask(mac_key.fx_mac_mask);
        rhandle = seek_record(WCACHE_MAC, (char *) &mac_key, 
                                      (void **) &wc_cache_ptr);
    } else {
        rhandle = read_next_record(WCACHE_MAC, rhandle, NULL,
                                   (void **) &wc_cache_ptr);
    }

    if (rhandle == NULL) {
        /* no matching record found */
        CSM_DEBUG("\nCSM: NO Matching info found.... Nothing deleted");
        return;
    }

    while (rhandle) {
        /*
         * Make sure that this is the correct record. There may be
         * multiple entries with the same key. If so continue reading
         * until a match is found
         */
        del_info = FALSE;
        if (((source == 0) || (wc_cache_ptr->cache_type == source)) &&
                ((obj_type == 0) || (wc_cache_ptr->obj_type == obj_type))) {
            if (obj_type == NO_OBJECT_TYPE) {
                del_info = TRUE;
            } else {
                switch (obj_type) {
                  case RING_ID:
                    if ((obj.ring_id == 0) || 
                             (wc_cache_ptr->obj.ring_id == obj.ring_id)) {
                        del_info = TRUE;
                    }
                    break;
                  case PEER_ID:
                    if ((obj.peer_id == 0) || 
                             (wc_cache_ptr->obj.peer_id == obj.peer_id)) {
                        del_info = TRUE;
                    }
                    break;
                  case PEER_GROUP_ID:
                    if ((obj.ring_group_id == 0) ||
                     (wc_cache_ptr->obj.ring_group_id == obj.ring_group_id)) {
                        del_info = TRUE;
                    }
                    break;
                  case RING_GROUP_ID:
                    if ((obj.ring_group_id == 0) ||
                     (wc_cache_ptr->obj.ring_group_id == obj.ring_group_id)) {
                        del_info = TRUE;
                    }
                    break;
                  case RIF_STRING:
                    if (obj.rif[0] == 0) {
                        del_info = TRUE;
                    } else {
                        rif_len = RIF_LENGTH(obj.rif[0]);
                        if (!(bcmp(wc_cache_ptr->obj.rif, obj.rif, rif_len))) {
                            del_info = TRUE;
                        }
                    }
                    break;
                  case PEER_IP_ADDR:
                    if ((obj.peer_ip_addr == 0) ||
                     (wc_cache_ptr->obj.peer_ip_addr == obj.peer_ip_addr)) {
                        del_info = TRUE;
                    }
                    break;
                  case PEER_HWIDB:
                    if ((obj.peer_hwidb == NULL) ||
                           (wc_cache_ptr->obj.peer_hwidb == obj.peer_hwidb)) {
                        del_info = TRUE;
                    }
                    break;
                  default:
                    break;
                }
            }
            if (del_info) {
                delete_record_get_next(WCACHE_MAC, rhandle);
                CSMV_DEBUG("\nCSM: WC Reachability info Deleted....");
                break;
            }
        } else {
            if ((mac_addr != NULL) && (mac_mask != NULL)) {
                rhandle = read_next_record(WCACHE_MAC, rhandle, 
                                           (char *) &mac_key,
                                           (void **) &wc_cache_ptr);
            } else {
                rhandle = read_next_record(WCACHE_MAC, rhandle, NULL,
                                           (void **) &wc_cache_ptr);
            }
        }
    }

    if (rhandle == NULL) {
        /* means that we did not find a match in read_next */
        CSM_DEBUG("\nCSM: NO Matching info found.... Nothing deleted");
    }

    return;

}
#endif




void reverse_mac_mask (char *mac_ptr)
{

    int    ii;

    for (ii = 0; ii < IEEEBYTES; ii++) {
        mac_ptr[ii] ^= 0xff;
    }

    return;

}


#ifdef CSM_WILD_CARD_SUPPORT
/*
 * Get the mac wild card cache entries. This currently gets the first
 * hit and does not search further. This means that if there is a better
 * entry after this one, that will not be used.
 */
void get_wc_mac_cache_entries(char *mac_addr, 
                              wc_mac_cache_t **wc_cache_ptr,
                              record_handle *wc_rhandle,
                              char direction)
{

    int              rc;
    wc_mac_cache_t   *tmp_cache_ptr;
    record_handle    rhandle;
    boolean          match_found;

    tmp_cache_ptr = NULL;
    *wc_cache_ptr = NULL;
    match_found = FALSE;

    rhandle = read_next_record(WCACHE_MAC, NULL, NULL, 
                               (void **) &tmp_cache_ptr);
    while (rhandle) {
        rc = masked_mac_cmp(mac_addr, tmp_cache_ptr->mac_addr, 
                            tmp_cache_ptr->mac_mask);
        if (rc == 0) {
            switch (tmp_cache_ptr->obj_type) {
              case RIF_STRING:
              case RING_ID:
              case RING_GROUP_ID:
                if (direction == LOCAL) {
                    *wc_cache_ptr = tmp_cache_ptr;
                    *wc_rhandle = rhandle;
                    match_found = TRUE;
                }
                break;
              case PEER_ID:
              case PEER_GROUP_ID:
              case PEER_IP_ADDR:
              case PEER_HWIDB:
                if (direction == REMOTE) {
                    *wc_cache_ptr = tmp_cache_ptr;
                    *wc_rhandle = rhandle;
                    match_found = TRUE;
                }
                break;
              default:
                break;
            }
            if (match_found) {
                CSMV_DEBUG("\nCSM: Found %e in wildcard cache", mac_addr);
                break;
            }
        } else {
            rhandle = read_next_record(WCACHE_MAC, rhandle, NULL, 
                                       (void **) &tmp_cache_ptr);
        }
    }

    return;

}
#endif

#ifdef CSM_WILD_CARD_SUPPORT
/*
 * Get the netbios name wild card cache entries. This currently gets 
 * the first hit and does not search further. This means that if there 
 * is a better entry after this one, that will not be used.
 */
void get_wc_name_cache_entries(char *nb_name, 
                               wc_name_cache_t **wc_cache_ptr,
                               record_handle *wc_rhandle,
                               char direction)
{

    int              rc;
    wc_name_cache_t  *tmp_cache_ptr;
    record_handle    rhandle;
    boolean          match_found;

    tmp_cache_ptr = NULL;
    *wc_cache_ptr = NULL;
    *wc_rhandle = NULL;
    match_found = FALSE;

    rhandle = read_next_record(WCACHE_NAME, NULL, NULL, 
                               (void **) &tmp_cache_ptr);
    while (rhandle != NULL) {
        rc = wc_strn_cmp(nb_name, tmp_cache_ptr->nb_name, 
                         NETBIOS_NAME_LENGTH_DEFAULT);
        if (rc == 0) {
            switch (tmp_cache_ptr->obj_type) {
              case RIF_STRING:
              case RING_ID:
              case RING_GROUP_ID:
                if (direction == LOCAL) {
                    *wc_cache_ptr = tmp_cache_ptr;
                    *wc_rhandle = rhandle;
                    match_found = TRUE;
                }
                break;
              case PEER_ID:
              case PEER_GROUP_ID:
              case PEER_IP_ADDR:
              case PEER_HWIDB:
                if (direction == REMOTE) {
                    *wc_cache_ptr = tmp_cache_ptr;
                    *wc_rhandle = rhandle;
                    match_found = TRUE;
                }
                break;
              default:
                break;
            }
            if (match_found) {
                NETB_CSMV_DEBUG("\nCSM: Found %15s in wildcard cache", nb_name);
                break;
            }
        } else {
            rhandle = read_next_record(WCACHE_NAME, rhandle, NULL, 
                                       (void **) &tmp_cache_ptr);
        }
    }

    return;

}
#endif



#ifdef CSM_WILD_CARD_SUPPORT
/*
 * csm_add_name_wc_entry()
 * add info to the ambiguous netbios name cache db
 */
void csm_add_wc_name_entry (char *name, 
                            cache_source_t source, 
                            reach_sense sense, 
                            object_type_t obj_type, 
                            config_obj_u obj)
{

    int              rif_len;
    record_handle    rhandle;
    wc_name_cache_t  wc_cache, *wc_cache_ptr;
    boolean          add_info;


    NETB_CSMV_DEBUG("\nCSM:Adding WC Reach entry for Netbios Name-%s, source-%x",
                          name, source);

    add_info = TRUE;

    rhandle = seek_record(WCACHE_NAME, name, (void **) &wc_cache_ptr);
    add_info = TRUE;
    /*
     * we already have some configuration information for the
     * same resource. If it the exact same info. print out a
     * mesage and do not add to the db. otherwise just add it.
     * But read all the records matching the key to make sure
     * that there are no duplicates.
     */
    while (rhandle != NULL) {
        if ((wc_cache_ptr->cache_type == source) &&
                   (wc_cache_ptr->obj_type == obj_type)) {
            switch (obj_type) {
              case RING_ID:
                if (wc_cache_ptr->obj.ring_id == obj.ring_id) {
                    add_info = FALSE;
                }
                break;
              case PEER_ID:
                if (wc_cache_ptr->obj.peer_id == obj.peer_id) {
                    add_info = FALSE;
                }
                break;
              case PEER_GROUP_ID:
                if (wc_cache_ptr->obj.ring_group_id == obj.peer_group_id) {
                    add_info = FALSE;
                }
                break;
              case RING_GROUP_ID:
                if (wc_cache_ptr->obj.ring_group_id == obj.ring_group_id) {
                    add_info = FALSE;
                }
                break;
              case RIF_STRING:
                rif_len = RIF_LENGTH(obj.rif[0]);
                if (!(bcmp(wc_cache_ptr->obj.rif, obj.rif, rif_len))) {
                    add_info = FALSE;
                }
                break;
              case PEER_IP_ADDR:
                if (wc_cache_ptr->obj.peer_ip_addr == obj.peer_ip_addr) {
                    add_info = FALSE;
                }
                break;
              case PEER_HWIDB:
                if (wc_cache_ptr->obj.peer_hwidb == obj.peer_hwidb) {
                    add_info = FALSE;
                }
                break;
              default:
                break;
            }
            if (add_info == FALSE) {
                NETB_CSMV_DEBUG("\nCSM: Duplicate Reachability info....");
                break;
            }
        }
        rhandle = read_next_record(WCACHE_NAME, rhandle, name,
                                   (void **) &wc_cache_ptr);
    }

    if (add_info) {
        bcopy(name, wc_cache.nb_name, NETBIOS_NAME_LENGTH_DEFAULT);
        /* set 16th bit to 0 for print routines... */
        wc_cache.nb_name[NETBIOS_MAX_NAME_LEN] = 0;
        wc_cache.obj_type = obj_type;
        wc_cache.obj = obj;
        wc_cache.cache_type = source;
        wc_cache.sense = sense;

        rhandle = add_record(WCACHE_NAME, name, (void *) &wc_cache);

        if (rhandle) {
            NETB_CSMV_DEBUG("\nCSM: Wild Card Reachability info added....");
        } else {
            NETB_CSME_DEBUG("\nCSM: Error adding Wild Card Reachability info ....");
        }
    }

    return;
}
#endif


#ifdef CSM_WILD_CARD_SUPPORT
/*
 * csm_delete_wc_name_entry()
 *
 * delete ambiguous cache entry
 */
void csm_delete_wc_name_entry (char *nb_name, cache_source_t source, 
                               object_type_t obj_type, config_obj_u obj)
{

    int              rif_len;
    record_handle    rhandle;
    wc_name_cache_t  *wc_cache_ptr;
    boolean          del_info;


    del_info = FALSE;

    NETB_CSMV_DEBUG("\nCSM: Deleting WC Reach entry for Netbios Name-%s, source-%x",
                          nb_name, source);

    rhandle = NULL;
    if (nb_name != NULL) {
        rhandle = seek_record(WCACHE_NAME, nb_name, (void **) &wc_cache_ptr);
    } else {
        rhandle = read_next_record(WCACHE_NAME, rhandle, NULL,
                                   (void **) &wc_cache_ptr);
    }

    if (rhandle == NULL) {
        /* no matching record found */
        NETB_CSM_DEBUG("\nCSM: NO Matching info found.... Nothing deleted");
        return;
    }

    while (rhandle) {
        /*
         * Make sure that this is the correct record. There may be
         * multiple entries with the same key. If so continue reading
         * until a match is found
         */
        del_info = FALSE;
        if (((source == 0) || (wc_cache_ptr->cache_type == source)) &&
                ((obj_type == 0) || (wc_cache_ptr->obj_type == obj_type))) {
            if (obj_type == NO_OBJECT_TYPE) {
                del_info = TRUE;
            } else {
                switch (obj_type) {
                  case RING_ID:
                    if ((obj.ring_id == 0) || 
                             (wc_cache_ptr->obj.ring_id == obj.ring_id)) {
                        del_info = TRUE;
                    }
                    break;
                  case PEER_ID:
                    if ((obj.peer_id == 0) || 
                             (wc_cache_ptr->obj.peer_id == obj.peer_id)) {
                        del_info = TRUE;
                    }
                    break;
                  case PEER_GROUP_ID:
                    if ((obj.ring_group_id == 0) ||
                     (wc_cache_ptr->obj.ring_group_id == obj.ring_group_id)) {
                        del_info = TRUE;
                    }
                    break;
                  case RING_GROUP_ID:
                    if ((obj.ring_group_id == 0) ||
                     (wc_cache_ptr->obj.ring_group_id == obj.ring_group_id)) {
                        del_info = TRUE;
                    }
                    break;
                  case RIF_STRING:
                    if (obj.rif[0] == 0) {
                        del_info = TRUE;
                    } else {
                        rif_len = RIF_LENGTH(obj.rif[0]);
                        if (!(bcmp(wc_cache_ptr->obj.rif, obj.rif, rif_len))) {
                            del_info = TRUE;
                        }
                    }
                    break;
                  case PEER_IP_ADDR:
                    if ((obj.peer_ip_addr == 0) ||
                     (wc_cache_ptr->obj.peer_ip_addr == obj.peer_ip_addr)) {
                        del_info = TRUE;
                    }
                    break;
                  case PEER_HWIDB:
                    if ((obj.peer_hwidb == NULL) ||
                           (wc_cache_ptr->obj.peer_hwidb == obj.peer_hwidb)) {
                        del_info = TRUE;
                    }
                    break;
                  default:
                    break;
                }
            }
            if (del_info) {
                delete_record_get_next(WCACHE_NAME, rhandle);
                NETB_CSMV_DEBUG("\nCSM: WC Reachability info Deleted....");
                break;
            }
        } else {
            rhandle = read_next_record(WCACHE_NAME, rhandle, nb_name,
                                       (void **) &wc_cache_ptr);
        }
    }

    if (rhandle == NULL) {
        /* means that we did not find a match in read_next */
        NETB_CSM_DEBUG("\nCSM: NO Matching info found.... Nothing deleted");
    }

    return;

}
#endif


