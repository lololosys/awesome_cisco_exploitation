/* $Id: dlsw_core_fsm.h,v 3.10.6.7 1996/08/28 12:40:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_core_fsm.h,v $
 *------------------------------------------------------------------
 * 
 * DLSw subsystem 
 *
 * Fri July 29, 1994 G. Pandian
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * 
 *------------------------------------------------------------------
 * $Log: dlsw_core_fsm.h,v $
 * Revision 3.10.6.7  1996/08/28  12:40:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.6.6  1996/07/23  13:21:29  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.10.6.5  1996/07/19  03:55:18  athippes
 * CSCdi60758:  dlsw : duplicate circuit identifier
 * Branch: California_branch
 *
 * Revision 3.10.6.4  1996/06/18  23:09:55  ravip
 * CSCdi60356:  DLSw MIB: Incorrent value returned for circuit timestamps
 * Branch: California_branch
 *
 * Revision 3.10.6.3  1996/06/11  15:01:10  ravip
 * CSCdi58441:  xid3 flooding in csna/dlsw environment cause access
 *              router to reboot
 *                  - Carry xid command response bit in the message
 *                  - Use Capex to exchange information about this
 *                    support
 * Branch: California_branch
 *
 * Revision 3.10.6.2  1996/05/17  10:44:56  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.4.3  1996/04/03  14:01:03  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.4.2  1996/03/19  18:36:33  fbordona
 * CSCdi51832:  DLSw: p_unqueue didnt find 179780 in queue 5091C
 *              Create dlsw_delay_eventQ.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.4.1  1996/03/17  17:37:47  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.10.6.1  1996/03/18  19:31:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.12.3  1996/03/16  06:38:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.12.2  1996/03/07  08:44:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.12.1  1996/02/20  00:45:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/03/05  23:56:26  kpatel
 * CSCdi50720:  SNA session hangs under heavy traffic
 *
 * Revision 3.9  1996/02/29  04:08:01  kpatel
 * CSCdi50323:  DLSw+/LLC2 shows BUSY even though permitted units are
 *              not zero
 *
 * Revision 3.8  1996/02/28  09:32:49  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.7  1996/02/07  21:43:09  ravip
 * CSCdi46935:  DLSw does not propogate largest frame bits correctly
 *
 * Revision 3.6  1995/12/23  00:53:47  kpatel
 * CSCdi46079:  DLSw+ does not support non-activation XID
 *
 * Revision 3.5  1995/12/12  17:36:04  ravip
 * CSCdi44808:  crash in csm_clsi_handler
 *
 * Revision 3.4  1995/12/09  16:49:03  fbordona
 * CSCdi45326:  DLSw+ ignores DISC.Ind in some states
 *              Make DLSw+ call CloseStn.Req with u_cep_id
 *              if ReqOpnStn.Req times out.
 *              Fix crash caused by "dlsw disable".
 *
 * Revision 3.3  1995/11/29  19:44:33  ravip
 * CSCdi40989:  crash: router tried to reference free memory in dlsw code
 *              use mem_lock() to lock memory, to avoid other routines
 *              from deleting the memory block in use.
 *              Use a flag to tag actions that would change the links.
 *
 * Revision 3.2  1995/11/17  09:02:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:22:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:55:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/07  04:37:52  kpatel
 * CSCdi39871:  DLSw+ does not interoperate with remote window size
 *              of one
 *
 * Revision 2.4  1995/06/28  18:50:08  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *              Clean up DLSw header files
 *
 * Revision 2.3  1995/06/26  18:48:25  kpatel
 * CSCdi36367:  DLSw+ circuit entry lookup slow with large number of
 * circuits
 *
 * Revision 2.2  1995/06/18  21:21:17  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:26:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DLSW_CORE_FSM_H__
#define __DLSW_CORE_FSM_H__

#include "../util/fsm.h"
#include <string.h>

/*
 * CLSi op tables
 */
static const ushort cstm_clsi_op_tbl[] =
{
        CLS_TEST_STN            | CLS_INDICATION,
        CLS_ID_STN              | CLS_INDICATION,
        CLS_UDATA_STN           | CLS_INDICATION,
        CLS_CONNECT_STN         | CLS_INDICATION,
};
#define NDLSW_CSTM_CLSI_OP sizeof(cstm_clsi_op_tbl)/sizeof(ushort)

static const ushort init_clsi_op_tbl[] =
{
        CLS_ACTIVATE_RING       | CLS_CONFIRM,
        CLS_DEACTIVATE_RING     | CLS_CONFIRM,
        CLS_ENABLE              | CLS_CONFIRM,
        CLS_DISABLE             | CLS_CONFIRM
};
#define NDLSW_INIT_CLSI_OP sizeof(init_clsi_op_tbl)/sizeof(ushort)


/*
 * Macros
 */
#define MSG_D   if ((dlsw_core_debug_msgs) ||           \
                    (dlsw_ptr->dlsw_core_debug_msgs))   buginf
#define STATE_D if ((dlsw_core_debug_state) ||          \
                    (dlsw_ptr->dlsw_core_debug_state))  buginf
#define FLOW_D  if ((dlsw_core_debug_flow) ||           \
                    (dlsw_ptr->dlsw_core_debug_flow))   buginf
#define XID_D   if ((dlsw_core_debug_xid) ||            \
                    (dlsw_ptr->dlsw_core_debug_xid))    buginf


/* 
 * ################################################################
 * 			STATES 
 * ################################################################
 */

typedef enum {
    dlswst_undefined    = 0,
    dlswst_disconnect,
    dlswst_loc_resolve,		/* awaiting req opn stn confirm */
    dlswst_rem_resolve,		/* successful cep creation */
    dlswst_ckt_start,	     	/* awaiting ICANREACH_cs */
    dlswst_ckt_pending,	     	/* awaiting REACH_ACK */
    dlswst_established,
    dlswst_contact_pend,	/* awaiting DLC_CONTACTED */
    dlswst_connect_pend,   	/* awaiting CONTACTED */
    dlswst_connected,
    dlswst_disc_pending, 	/* awaiting ssp dl_halted */
    dlswst_halt_pending,	/* awaiting disc.cnf */
    dlswst_halt_pend_noack,	/* awaiting disc.cnf or close_stn.cnf */
    dlswst_close_pend,		/* awaiting close_stn.cnf */
    dlswst_restart_pend,    	/* The remote DLS is awaiting the 
				   DLC_DL_HALTED indication following the 
				   DLC_HALT_DL request */
    dlswst_ckt_restart,	     	/* The DLS that originated the reset is
				   awaiting the restart of the data link
				   and the DL_RESTARTED response to a
				   RESTART_DL message. */
    dlswst_fsm_max = dlswst_ckt_restart
} dlswst_t;


/* 
 * ################################################################
 *  			EVENTS - MAJOR CATEGORIES 
 * ################################################################
 */

typedef enum {
    dlswma_admin = 1,	
    dlswma_wan,	
    dlswma_dlc,	
    dlswma_timer,
    dlswma_max = dlswma_timer
} dlswma_t;


/*
 * ######################################################################
 *                      EVENTS - MINOR CATEGORIES - ADMIN ( dlswma_admin)
 * ######################################################################
 */

/*
 * Need to be able to distinguish admin_stop from admin_error
 * admin_stop is when a user enters "clear circuit".
 * admin_error is when a dlc error occurs. 
 * When the fsm is in local or remote resolve pending, admin_stop
 * must be ignored, so as to not get dlsw and cls out of context
 * sync, but admin_error must be honored.
 */ 
typedef enum {
    dlswmi_admin_stop = 1,
    dlswmi_admin_error,
    dlswmi_admin_wan_fail,
    dlswmi_admin_max = dlswmi_admin_wan_fail
} dlswmi_admin_t;


/* 
 * ######################################################################
 *  			EVENTS - MINOR CATEGORIES - FROM WAN (dlswma_wan)
 * ######################################################################
 */


typedef enum {
    dlswmi_wan_unknown,   	/* UNK  - Unknown		*/
    dlswmi_wan_canureach = 1,   /* CUR  - CAN U Reach Station	*/
    dlswmi_wan_icanreach,  	/* ICR  - I Can Reach Station	*/
    dlswmi_wan_reach_ack,    	/* ACK  - Reach Acknowledgment	*/
    dlswmi_wan_dgmframe,    	/* DGM  - Datagram Frame	*/ 
    dlswmi_wan_xidframe,    	/* XID  - XID Frame		*/
    dlswmi_wan_contact,      	/* CONQ - contact Remote Station*/
    dlswmi_wan_contacted,    	/* CONR - Remote Station Contacted */
    dlswmi_wan_restart_dl,   	/* RSTQ - Restart Data Link	*/
    dlswmi_wan_dl_restarted, 	/* RSTR - Data Link Restarted	*/
    dlswmi_wan_infoframe,    	/* INFO - Information (I) Frame	*/
    dlswmi_wan_halt_dl,	       	/* HLTR - Halt Data Link	*/ 
    dlswmi_wan_dl_halted,      	/* HLTS - Data Link Halted	*/
    dlswmi_wan_netbios_nq,     	/* NBNR - NetBIOS Name Query	*/
    dlswmi_wan_netbios_nr,     	/* NBNS - NetBIOS Name Recognized */
    dlswmi_wan_dataframe,      	/* DATA - Data Frame		*/
    dlswmi_wan_halt_dl_noack,  	/* HLTN - Halt Data Link No Ack */
    dlswmi_wan_netbios_anq,    	/* NBAN - NetBIOS Add Name Query*/
    dlswmi_wan_netbios_anr,    	/* NBAS - NetBIOS Add Name Response*/
    dlswmi_wan_ifcm,		/* IFCM - Independent flow control */
    dlswmi_tcp_pipe_failure,	/* TCP pipe failure		*/
    dlswmi_wan_max	= dlswmi_tcp_pipe_failure
} dlswmi_wan_t;

/* 
 * ######################################################################
 *  			EVENTS - MINOR CATEGORIES - FROM LAN (dlswma_dlc)
 * ######################################################################
 */

typedef enum {
    dlswmi_dlc_ros_cnf = 1,
    dlswmi_dlc_conn_cnf,
    dlswmi_dlc_conn_ind,
    dlswmi_dlc_id_ind,
    dlswmi_dlc_id_cnf,
    dlswmi_dlc_data_ind,
    dlswmi_dlc_udata_ind,  /* NETBIOS */
    dlswmi_dlc_flow_ind,
    dlswmi_dlc_disc_cnf,
    dlswmi_dlc_disc_ind,
    dlswmi_dlc_close_stn_cnf,
    dlswmi_dlc_max = dlswmi_dlc_close_stn_cnf
} dlswmi_dlc_t;
    
/* 
 * ######################################################################
 *  			EVENTS - MINOR CATEGORIES - TIMER ( dlswma_timer)
 * ######################################################################
 */

typedef enum {
    dlswmi_timer_expired = 1,
    dlswmi_timer_max = dlswmi_timer_expired
} dlswmi_timer_t;


/*
 * Various constants
 */
#define DLSW_MAGIC_ID        0xABCDEF99
#define DLSW_GENERAL_TIMEOUT   (15 * ONESEC) /* max secs in transition states */
#define DLSW_GENERAL_TIMEOUT_2 (30 * ONESEC)
#define DLSW_XID_TIMEOUT       (60 * ONESEC) /* max secs in between xids */
                                         /* allows time for the dlc to retry */
#define DLSW_MAX_XID_LEN     255
#define NDLSW_NO_ZWO_MAX       3   /* no of zwos before the window shrinks */
#define NDLSW_MAX_NAME       100

#define dlsw_our_slot           our_port_id & 0xf000
#define dlsw_our_port           our_port_id & 0x0f00
#define dlsw_our_sub_num        our_port_id & 0x00f0
#define dlsw_our_type           our_port_id & 0x000f

#define dlsw_their_slot         their_port_id & 0xf000
#define dlsw_their_port         their_port_id & 0x0f00
#define dlsw_their_sub_num      their_port_id & 0x00f0
#define dlsw_their_type         their_port_id & 0x000f



/* 
 * Number of frames that could possibly
 * cross over from clsi to dlsw 
 * before our stop flow request reaches CLSI 
 */

/*
 * DLSW subsystem header
 */

#define DLSW_MAJVERSION 1
#define DLSW_MINVERSION 0
#define DLSW_EDITVERSION 0

#define DLSWC_ERR_FSM 1
#define DLSWC_ERR_CSM 2
#define DLSWC_ERR_LAN 3
#define DLSWC_ERR_WAN 4

/*
 * XID support
 */
typedef enum {
    dlsw_xid_reset = 1,
    dlsw_xid_ind_rcvd,
    dlsw_xid_req_sent
} dlsw_xid_st_t;


/*
 * LLC CMD RSP flags
 */
#define DLSW_LLC_CMD            1
#define DLSW_LLC_RSP            2


typedef struct _dlsw_hash {

    struct _dlsw_hash	*next;
    long		magic;
    ushort		bucket;
    uint		u_cep_id;
    uint		p_cep_id;
    mgd_timer		timer;
    dlswst_t		state;
    peerhandle_t	peer_handle;

    ssp_dlc_id_t       dlc_id;
    /* This storage always assumes that target is theirs 
       and origin is ours irrespective of the circuit origination;
       In other words these cells are directionless */

    dlsw_ring_t         *port_id_dgra; /* dispatcher global ring access ptr */

    uint 		our_port_id;
    uint		our_dlc_corr; /* dlsw_ptr */
    uint		our_transport_id; /* peer_handle */
    uint		their_port_id;
    uint		their_dlc_corr;
    uint		their_transport_id;

    boolean		my_ckt;
    			/* TRUE - if it is owned 
			   (not neccessarily originated) 
			   by this switch  */

    /* flow control */
    ulong               fc_prev_llc_flowlevel;
    ulong 		fc_llc_flowlevel;
    ulong 		fc_peer_flowlevel;
    ushort		fc_send_granted_units;
    ushort		fc_send_curr_window;
    boolean		fc_send_owe_fcack;

    ushort              fc_largest_recv_granted;
    ushort              fc_largest_send_granted;
    ushort              fc_half_window_sents;
    ushort              fc_half_window_rcvds;
    ushort              fc_reset_window_sents;
    ushort              fc_reset_window_rcvds;


    ushort		fc_recv_curr_window;
    ushort		fc_recv_granted_units;
    boolean		fc_recv_owe_fcack;
    boolean		fc_recv_init_needed;
    boolean		fc_recv_grant_oper;
    boolean             fc_recv_grant_delayed;
    boolean             fc_stop_flow_sent;
    boolean             fc_data_ind_queued;

    paktype		*wan_pak;
    TCLSIMsg		*clsi_msg;
    TCLSIMsg		*pending_clsi_msg;

    dlsw_xid_st_t	xid_state;

    /* dlc dependent parameters for XID negotiations */
    ushort              max_i_field_len;  /* max receivable i-field len */
    uchar               window_size;      /* max i-frames without ack */
    dlsw_abm_type       abm_state;

    uchar               local_rif_len;    
    uchar		local_rif[SRB_MAXRIF];

    /* Net management */
    ulong 		forwards;
    ulong		explorers;
    ulong		datagrams;
    ulong		iframes;
    ssp_llc_flags_t	llc_flags;
    boolean             del_flag;
    long                reach_mtu_size;      /* what csm says */

    /* debug */
    boolean dlsw_core_debug_msgs;
    boolean dlsw_core_debug_state;
    boolean dlsw_core_debug_flow;
    boolean dlsw_core_debug_xid;
    boolean dlsw_csm_debug_error;
    boolean dlsw_csm_debug_event;
    boolean dlsw_csm_debug_verbose;
    boolean dlsw_csm_debug_sna;
    boolean dlsw_csm_debug_netb;
    boolean dlsw_csm_debug_all;

    sys_timestamp      circuit_entry_time;
    sys_timestamp      circuit_state_time;

} dlsw_t;



/* 
 * ################################################################
 * 		CORE FSM PRIVATE EXTERNAL DATA
 * ################################################################
 */
extern queuetype dlsw_hashQ[NDLSW_HASH_SIZE];
extern queuetype dlsw_deadQ;
extern queuetype dlsw_delay_eventQ;

extern boolean  dlsw_core_debug_msgs;
extern boolean  dlsw_core_debug_state;
extern boolean  dlsw_core_debug_flow;
extern boolean  dlsw_core_debug_xid;


/* 
 * ################################################################
 * 		CORE FSM PRIVATE FUNCTION PROTOTYPES
 * ################################################################
 */
extern void dlsw_queue_cls_message(TCLSIMsg* clsi_msg);
extern boolean dlsw_msgBLOCK (void);
extern forktype dlsw_msg_proc (void);
extern forktype dlsw_background (void);
extern void dlsw_send_ssp (dlsw_t *dlsw_ptr, ssp_op_t ssp_op,
                           uchar *payload_ptr, ulong payload_len);

extern char *dlsw_sspop2str(uint op);

extern int get_rif_from_clsi(TCLSIMsg *clsi_msg, uchar *rif);
extern boolean dlsw_is_ssp_ctl_mac_hdr(dlsw_t *dlsw_ptr, ssp_op_t ssp_op);


/*
 * flow control suport
 */
extern void dlsw2clsi_flow_action (dlsw_t *dlsw_ptr, ushort action);

/*
 * ################################################################
 *              INLINE FUNCTIONS
 * ################################################################
 */

static inline dlswmi_wan_t dlsw_ssp_classify (ssp_op_t opcode)
{
    switch (opcode) {
        case  SSP_OP_canureach:             return(dlswmi_wan_canureach);
        case  SSP_OP_icanreach:             return(dlswmi_wan_icanreach);
        case  SSP_OP_reach_ack:             return(dlswmi_wan_reach_ack);
        case  SSP_OP_dgmframe:              return(dlswmi_wan_dgmframe);
        case  SSP_OP_xidframe:              return(dlswmi_wan_xidframe);
        case  SSP_OP_contact:               return(dlswmi_wan_contact);
        case  SSP_OP_contacted:             return(dlswmi_wan_contacted);
        case  SSP_OP_restart_dl:            return(dlswmi_wan_restart_dl);
        case  SSP_OP_dl_restarted:          return(dlswmi_wan_dl_restarted);
        case  SSP_OP_infoframe:             return(dlswmi_wan_infoframe);
        case  SSP_OP_halt_dl:               return(dlswmi_wan_halt_dl);
        case  SSP_OP_dl_halted:             return(dlswmi_wan_dl_halted);
        case  SSP_OP_netbios_nq:            return(dlswmi_wan_netbios_nq);
        case  SSP_OP_netbios_nr:            return(dlswmi_wan_netbios_nr);
        case  SSP_OP_dataframe:             return(dlswmi_wan_dataframe);
        case  SSP_OP_halt_dl_noack:         return(dlswmi_wan_halt_dl_noack);
        case  SSP_OP_ifcm:                  return(dlswmi_wan_ifcm); 
        default:                            return(dlswmi_wan_unknown);
    }
}


static inline char *dlsw_print_event (dlswma_t major, uint minor, uchar *buff)
{
    char *prmajor = "Invalid Event";
    char *prminor = "Invalid Event";

    switch (major) {

      case dlswma_admin:
        prmajor = "ADMIN";
        switch (minor) {
          case dlswmi_admin_stop:
            prminor = "STOP"; break;
          case dlswmi_admin_error:
             prminor = "DLC_ERROR"; break;
          case dlswmi_admin_wan_fail:
            prminor = "WAN_FAIL"; break;
        }
        break;

      case dlswma_dlc:
        prmajor = "DLC";
        switch (minor) {
          case CLS_REQ_OPNSTN | CLS_CONFIRM:
            prminor = "ReqOpnStn.Cnf"; break;

          case CLS_CONNECT | CLS_CONFIRM:
            prminor = "Connect.Cnf"; break;

          case CLS_CONNECTED | CLS_INDICATION:
            prminor = "Connected.Ind"; break;

          case CLS_CONNECT | CLS_INDICATION:
          case CLS_CONNECT_STN | CLS_INDICATION:
            prminor = "Connect.Ind"; break;

          case CLS_ID_STN | CLS_INDICATION:
          case CLS_ID | CLS_CONFIRM:
          case CLS_ID | CLS_INDICATION:
            prminor = "Id"; break;

          case CLS_DATA | CLS_INDICATION:
            prminor = "Data.Ind"; break;

          case CLS_FLOW | CLS_INDICATION:
            prminor = "Flow.Ind"; break;

          case CLS_DISCONNECT | CLS_CONFIRM:
            prminor = "Disc.Cnf"; break;

          case CLS_DISCONNECT | CLS_INDICATION:
            prminor = "Disc.Ind"; break;

          case CLS_CLOSE_STN | CLS_CONFIRM:
            prminor = "CloseStn.Cnf"; break;
        }
        break;

      case dlswma_wan:
        prmajor = "WAN";
        switch (minor) {
          case dlswmi_wan_canureach:
            prminor = "CUR"; break;
          case dlswmi_wan_icanreach:
            prminor = "ICR"; break;
          case dlswmi_wan_reach_ack:
            prminor = "ACK"; break;
          case dlswmi_wan_dgmframe:
            prminor = "DGM"; break;
          case dlswmi_wan_xidframe:
            prminor = "XID"; break;
          case dlswmi_wan_contact:
            prminor = "CONQ"; break;
          case dlswmi_wan_contacted:
            prminor = "CONR"; break;
          case dlswmi_wan_restart_dl:
            prminor = "RSTQ"; break;
          case dlswmi_wan_dl_restarted:
            prminor = "RSTR"; break;
          case dlswmi_wan_infoframe:
            prminor = "INFO"; break;
          case dlswmi_wan_halt_dl:
            prminor = "HLTQ"; break;
          case dlswmi_wan_dl_halted:
            prminor = "HLTR"; break;
          case dlswmi_wan_netbios_nq:
            prminor = "NBNQ"; break;
          case dlswmi_wan_netbios_nr:
            prminor = "NBNR"; break;
          case dlswmi_wan_dataframe:
            prminor = "DATA"; break;
          case dlswmi_wan_halt_dl_noack:
            prminor = "HLTN"; break;
          case dlswmi_tcp_pipe_failure:
            prminor = "WANF"; break;
          case dlswmi_wan_ifcm:
            prminor = "IFCM"; break;
          default:
            prminor = "UNKNOWN"; break;
        }
        break;

      case dlswma_timer:
        prmajor = "TIMER";
        switch (minor) {
          case dlswmi_timer_expired:
            prminor = "Expired"; break;
        }
        break;
    }

    strcpy(buff, prmajor);
    strcat(buff, "-");
    strcat(buff , prminor);
    return (buff);
}


static inline char *dlsw_print_state (dlswst_t state)
{
    switch (state) {
      case dlswst_disconnect:
        return("DISCONNECTED");
      case dlswst_loc_resolve:
        return("LOCAL_RESOLVE");
      case dlswst_rem_resolve:
        return("REMOTE_RESOLVE");
      case dlswst_established:
        return("CKT_ESTABLISHED");
      case dlswst_ckt_start:
        return("CKT_START");
      case dlswst_ckt_pending:
        return("CKT_PENDING");
      case dlswst_connect_pend:
        return("CONNECT_PENDING");
      case dlswst_contact_pend:
        return("CONTACT_PENDING");
      case dlswst_connected:
        return("CONNECTED");
      case dlswst_ckt_restart:
        return("CKT_RESTART");
      case dlswst_disc_pending:
        return("DISC_PENDING");
      case dlswst_restart_pend:
        return("RESTART_PENDING");
      case dlswst_halt_pending:
        return("HALT_PENDING");
      case dlswst_halt_pend_noack:
        return("HALT_NOACK_PEND");
      case dlswst_close_pend:
        return("CLOSE_PEND");
      default:
        return("UNKNOWN");
    }
}


extern int dlsw_fsm_decode (dlsw_t *dlsw_ptr, int major, int minor);
extern ushort dlsw_fsm (dlsw_t *dlsw_ptr, dlswma_t major, int minor);
extern void dlsw_restart_fsm(void *d, int old_state, int major, int minor);

extern ushort dlsw_action_a (dlsw_t *);
extern ushort dlsw_action_b (dlsw_t *);
extern ushort dlsw_action_c (dlsw_t *);
extern ushort dlsw_action_d (dlsw_t *);
extern ushort dlsw_action_e (dlsw_t *);
extern ushort dlsw_action_f (dlsw_t *);
extern ushort dlsw_action_g (dlsw_t *);
extern ushort dlsw_action_h (dlsw_t *);
extern ushort dlsw_action_i (dlsw_t *);
extern ushort dlsw_action_j (dlsw_t *);
extern ushort dlsw_action_k (dlsw_t *);
extern ushort dlsw_action_l (dlsw_t *);
extern ushort dlsw_action_m (dlsw_t *);
extern ushort dlsw_action_n (dlsw_t *);
extern ushort dlsw_action_o (dlsw_t *);
extern ushort dlsw_action_p (dlsw_t *);
extern ushort dlsw_action_q (dlsw_t *);
extern ushort dlsw_action_r (dlsw_t *);
extern ushort dlsw_action_s (dlsw_t *);
extern ushort dlsw_action_t (dlsw_t *);
extern ushort dlsw_action_u (dlsw_t *);
extern ushort dlsw_action_v (dlsw_t *);
extern ushort dlsw_action_w (dlsw_t *);
extern ushort dlsw_action_x (dlsw_t *);
extern ushort dlsw_action_y (dlsw_t *);
extern ushort dlsw_action_z (dlsw_t *);
extern ushort dlsw_action_1 (dlsw_t *);
extern ushort dlsw_action_2 (dlsw_t *);
extern ushort dlsw_action_3 (dlsw_t *);
extern ushort dlsw_action_4 (dlsw_t *);
extern ushort dlsw_action_5 (dlsw_t *);
extern ushort dlsw_action_6 (dlsw_t *);
extern ushort dlsw_action_7 (dlsw_t *);

#define DLSW_ACTION_a FSM_ACTION_ROUTINE(dlsw_action_a)
#define DLSW_ACTION_b FSM_ACTION_ROUTINE(dlsw_action_b)
#define DLSW_ACTION_c FSM_ACTION_ROUTINE(dlsw_action_c)
#define DLSW_ACTION_d FSM_ACTION_ROUTINE(dlsw_action_d)
#define DLSW_ACTION_e FSM_ACTION_ROUTINE(dlsw_action_e)
#define DLSW_ACTION_f FSM_ACTION_ROUTINE(dlsw_action_f)
#define DLSW_ACTION_g FSM_ACTION_ROUTINE(dlsw_action_g)
#define DLSW_ACTION_h FSM_ACTION_ROUTINE(dlsw_action_h)
#define DLSW_ACTION_i FSM_ACTION_ROUTINE(dlsw_action_i)
#define DLSW_ACTION_j FSM_ACTION_ROUTINE(dlsw_action_j)
#define DLSW_ACTION_k FSM_ACTION_ROUTINE(dlsw_action_k)
#define DLSW_ACTION_l FSM_ACTION_ROUTINE(dlsw_action_l)
#define DLSW_ACTION_m FSM_ACTION_ROUTINE(dlsw_action_m)
#define DLSW_ACTION_n FSM_ACTION_ROUTINE(dlsw_action_n)
#define DLSW_ACTION_o FSM_ACTION_ROUTINE(dlsw_action_o)
#define DLSW_ACTION_p FSM_ACTION_ROUTINE(dlsw_action_p)
#define DLSW_ACTION_q FSM_ACTION_ROUTINE(dlsw_action_q)
#define DLSW_ACTION_r FSM_ACTION_ROUTINE(dlsw_action_r)
#define DLSW_ACTION_s FSM_ACTION_ROUTINE(dlsw_action_s)
#define DLSW_ACTION_t FSM_ACTION_ROUTINE(dlsw_action_t)
#define DLSW_ACTION_u FSM_ACTION_ROUTINE(dlsw_action_u)
#define DLSW_ACTION_v FSM_ACTION_ROUTINE(dlsw_action_v)
#define DLSW_ACTION_w FSM_ACTION_ROUTINE(dlsw_action_w)
#define DLSW_ACTION_x FSM_ACTION_ROUTINE(dlsw_action_x)
#define DLSW_ACTION_y FSM_ACTION_ROUTINE(dlsw_action_y)
#define DLSW_ACTION_z FSM_ACTION_ROUTINE(dlsw_action_z)
#define DLSW_ACTION_1 FSM_ACTION_ROUTINE(dlsw_action_1)
#define DLSW_ACTION_2 FSM_ACTION_ROUTINE(dlsw_action_2)
#define DLSW_ACTION_3 FSM_ACTION_ROUTINE(dlsw_action_3)
#define DLSW_ACTION_4 FSM_ACTION_ROUTINE(dlsw_action_4)
#define DLSW_ACTION_5 FSM_ACTION_ROUTINE(dlsw_action_5)
#define DLSW_ACTION_6 FSM_ACTION_ROUTINE(dlsw_action_6)
#define DLSW_ACTION_7 FSM_ACTION_ROUTINE(dlsw_action_7)

#endif /* __DLSWCOREFSM_H__ */



