/* $Id: dlsw_local_private.h,v 3.6.42.3 1996/07/19 03:55:32 athippes Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_local_private.h,v $
 *------------------------------------------------------------------
 * DLSw Local Switching Internal Header File
 *
 * August 1995, Frank Bordonaro
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dlsw_local_private.h,v $
 * Revision 3.6.42.3  1996/07/19  03:55:32  athippes
 * CSCdi60758:  dlsw : duplicate circuit identifier
 * Branch: California_branch
 *
 * Revision 3.6.42.2  1996/05/17  10:45:56  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.30.2  1996/04/11  14:37:18  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.30.1  1996/04/03  14:01:41  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.42.1  1996/03/18  19:32:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.12.2  1996/03/07  08:44:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.12.1  1996/02/20  13:46:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1995/12/31  18:17:03  fbordona
 * CSCdi46267:  DLSw sdlc secondary multidrop PUs do not connect.
 *
 * Revision 3.5  1995/12/23  00:53:51  kpatel
 * CSCdi46079:  DLSw+ does not support non-activation XID
 *
 * Revision 3.4  1995/12/15  17:18:29  fbordona
 * CSCdi45661:  DLSw+ peers show incorrect circuit count.
 *              Added checking for enable and disable of ports.
 *              Consolidate fsm to disp *stn structures.
 *              Consolidate fsm_to_disp() and cstm_to_disp().
 *              Improve local switching debug.
 *
 * Revision 3.3  1995/12/09  16:49:10  fbordona
 * CSCdi45326:  DLSw+ ignores DISC.Ind in some states
 *              Make DLSw+ call CloseStn.Req with u_cep_id
 *              if ReqOpnStn.Req times out.
 *              Fix crash caused by "dlsw disable".
 *
 * Revision 3.2  1995/11/17  09:03:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:24:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:55:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/24  18:49:05  fbordona
 * Placeholder for new file
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _DLSW_LOCAL_PRIVATE_H_
#define _DLSW_LOCAL_PRIVATE_H_

/*
 * Constants
 */
#define DL_GENERAL_TIMEOUT (30 * ONESEC) /* max secs in transition states */
#define DL_XID_TIMEOUT     (30 * ONESEC) /* max secs in between xids */
                                         /* allows time for the dlc to retry */
#define DL_ROS_TIMEOUT     (120 * ONESEC)/* max secs wait for ReqOpnStn.Cfm */
#define DL_MAGIC_ID        0xC0EDBABE

/*
 * Macros
 */
#define LDEBUG   if (dc->debug) buginf

#include <string.h>

/*
 * External Data References
 */
extern queuetype dl_hashQ[DL_HASH_SIZE];
extern queuetype dl_deadQ;

/* 
 * States
 *
 * dl_disconnected :    No CLS context.
 * 
 */
typedef enum {
    dlst_disconnected = 1,
    dlst_ros_sent,
    dlst_ckt_established,
    dlst_conn_ind_rcvd,
    dlst_conn_req_sent,
    dlst_connected,
    dlst_disc_ind_rcvd,
    dlst_disc_req_sent,
    dlst_close_pending,
    dlst_fsm_max	= dlst_close_pending
} dl_state_t;


/* 
 *  Major Categories
 */
typedef enum _dl_major
{
    dle_admin	= 1,
    dle_dlc,
    dle_timer
} dle_major_t;

typedef enum {
    dle_admin_start	 = 1,
    dle_admin_stop,
    dle_admin_conn,
    dle_admin_disc,
    dle_admin_dlc_err
} dle_admin_t;

typedef enum {
    dle_timer_expired = 1,
} dle_timer_t;

typedef enum {
    dle_req_opn_stn_cnf = 1,
    dle_conn_cnf,
    dle_conn_ind,
    dle_id_ind,
    dle_id_cnf,
    dle_data_ind,
    dle_flow_ind,
    dle_disc_cnf,
    dle_disc_ind,
    dle_close_stn_cnf
} dle_dlc_t;

typedef enum {
    dl_xid_reset = 1,
    dl_xid_ind_rcvd,
    dl_xid_req_sent
} dl_xid_st_t;

typedef struct dl_cep_t_ {
    ulong id; /* used to validate entry on lookup */
    void *parent;
    struct dl_cep_t_ *partner;
    uint p_cep_id;

    dlsw_ring_t *port_id;
    TCLSIMsg *clsi_msg;
    TCLSIMsg *pending_clsi_msg;
    mgd_timer timer;
    dl_state_t state;
    dl_xid_st_t xid_state;
    boolean debug;

    uchar mac[IEEEBYTES];
    uchar sap;
    uchar rif_len;
    uchar rif[SRB_MAXRIF];

    ushort max_i_field_len;  /* max receivable i-field len */
    dlsw_abm_type abm_state;
    uchar window_size;       /* max i-frames without ack */
} dl_cep_t;

typedef struct _dl_ckt {
    struct _dl_ckt *next;
    uchar bucket;
    uint  dl_ckt_id;   /* dl_ckt ptr */

    dl_cep_t cep_a;
    dl_cep_t cep_b;
} dl_ckt_t;


/*
 * Function Prototypes
 */
extern ushort dl_fsm (dl_cep_t *, dle_major_t, int);
extern int dl_fsm_decode (dl_cep_t *, dle_major_t major, int minor);
extern ushort dl_action_a (dl_cep_t *);
extern ushort dl_action_b (dl_cep_t *);
extern ushort dl_action_c (dl_cep_t *);
extern ushort dl_action_d (dl_cep_t *);
extern ushort dl_action_e (dl_cep_t *);
extern ushort dl_action_f (dl_cep_t *);
extern ushort dl_action_g (dl_cep_t *);
extern ushort dl_action_h (dl_cep_t *);
extern ushort dl_action_i (dl_cep_t *);
extern ushort dl_action_j (dl_cep_t *);
extern ushort dl_action_k (dl_cep_t *);
extern ushort dl_action_l (dl_cep_t *);
extern ushort dl_action_m (dl_cep_t *);
extern ushort dl_action_n (dl_cep_t *);
extern ushort dl_action_o (dl_cep_t *);
extern ushort dl_action_p (dl_cep_t *);
extern ushort dl_action_q (dl_cep_t *);
extern ushort dl_action_r (dl_cep_t *);
extern ushort dl_action_s (dl_cep_t *);
extern ushort dl_action_t (dl_cep_t *);
extern ushort dl_action_u (dl_cep_t *);
extern ushort dl_action_v (dl_cep_t *);
extern ushort dl_action_w (dl_cep_t *);
extern ushort dl_action_x (dl_cep_t *);
extern ushort dl_action_y (dl_cep_t *);
extern ushort dl_action_z (dl_cep_t *);
extern ushort dl_action_1 (dl_cep_t *);
extern ushort dl_action_2 (dl_cep_t *);

/*
 * Inlines
 */
static inline char *dl_print_event (dle_major_t major, uint minor, uchar *buff)
{
    char *prmajor = "Invalid Event";
    char *prminor = "Invalid Event";

    switch (major) {

      case dle_admin:
        prmajor = "ADMIN";
        switch (minor) {
          case dle_admin_start:
            prminor = "START"; break;
          case dle_admin_stop:
            prminor = "STOP"; break;
          case dle_admin_conn:
            prminor = "CONN"; break;
          case dle_admin_disc:
            prminor = "DISC"; break;
          case dle_admin_dlc_err:
            prminor = "DLCERR"; break;
        }
        break;

      case dle_dlc:
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

      case dle_timer:
        prmajor = "TIMER";
        switch (minor) {
          case dle_timer_expired:
            prminor = "Expired"; break;
        }
        break;
    }

    strcpy(buff, prmajor);
    strcat(buff, "-");
    strcat(buff , prminor);
    return (buff);
}


static inline char *dl_print_state (dl_state_t state)
{
    switch (state) {
      case dlst_disconnected:
        return("DISCONNECTED "); 
      case dlst_ros_sent:
        return("OPN_STN_PEND ");
      case dlst_ckt_established:
        return("ESTABLISHED  ");
      case dlst_conn_ind_rcvd:
        return("CONN_IN_PEND ");
      case dlst_conn_req_sent:
        return("CONN_OUT_PEND");
      case dlst_connected:
        return("CONNECTED    ");
      case dlst_disc_ind_rcvd:
        return("DISC_IN_PEND ");
      case dlst_disc_req_sent:
        return("DISC_OUT_PEND"); 
      case dlst_close_pending:
        return("CLOSE_PEND   "); 
      default:
        return("UNKNOWN      ");
    }      
}

#define DL_ACTION_a FSM_ACTION_ROUTINE(dl_action_a)
#define DL_ACTION_b FSM_ACTION_ROUTINE(dl_action_b)
#define DL_ACTION_c FSM_ACTION_ROUTINE(dl_action_c)
#define DL_ACTION_d FSM_ACTION_ROUTINE(dl_action_d)
#define DL_ACTION_e FSM_ACTION_ROUTINE(dl_action_e)
#define DL_ACTION_f FSM_ACTION_ROUTINE(dl_action_f)
#define DL_ACTION_g FSM_ACTION_ROUTINE(dl_action_g)
#define DL_ACTION_h FSM_ACTION_ROUTINE(dl_action_h)
#define DL_ACTION_i FSM_ACTION_ROUTINE(dl_action_i)
#define DL_ACTION_j FSM_ACTION_ROUTINE(dl_action_j)
#define DL_ACTION_k FSM_ACTION_ROUTINE(dl_action_k)
#define DL_ACTION_l FSM_ACTION_ROUTINE(dl_action_l)
#define DL_ACTION_m FSM_ACTION_ROUTINE(dl_action_m)
#define DL_ACTION_n FSM_ACTION_ROUTINE(dl_action_n)
#define DL_ACTION_o FSM_ACTION_ROUTINE(dl_action_o)
#define DL_ACTION_p FSM_ACTION_ROUTINE(dl_action_p)
#define DL_ACTION_q FSM_ACTION_ROUTINE(dl_action_q)
#define DL_ACTION_r FSM_ACTION_ROUTINE(dl_action_r)
#define DL_ACTION_s FSM_ACTION_ROUTINE(dl_action_s)
#define DL_ACTION_t FSM_ACTION_ROUTINE(dl_action_t)
#define DL_ACTION_u FSM_ACTION_ROUTINE(dl_action_u)
#define DL_ACTION_v FSM_ACTION_ROUTINE(dl_action_v)
#define DL_ACTION_w FSM_ACTION_ROUTINE(dl_action_w)
#define DL_ACTION_x FSM_ACTION_ROUTINE(dl_action_x)
#define DL_ACTION_y FSM_ACTION_ROUTINE(dl_action_y)
#define DL_ACTION_z FSM_ACTION_ROUTINE(dl_action_z)
#define DL_ACTION_1 FSM_ACTION_ROUTINE(dl_action_1)
#define DL_ACTION_2 FSM_ACTION_ROUTINE(dl_action_2)

#endif 


