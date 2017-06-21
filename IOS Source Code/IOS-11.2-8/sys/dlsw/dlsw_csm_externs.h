/* $Id: dlsw_csm_externs.h,v 3.5.12.7 1996/08/16 06:55:52 ioakley Exp $
 *------------------------------------------------------------------
 * DLSw CSM (Circuit Setup Manager) External Header File
 *
 * August 1994, Ravi Periaswamy
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Contains external declarations for users of DLSw CSM module 
 * services.
 *------------------------------------------------------------------
 * $Log: dlsw_csm_externs.h,v $
 * Revision 3.5.12.7  1996/08/16  06:55:52  ioakley
 * CSCdi58903:  Add dlsw touch timer support for sna/netbios
 *              reachability cache entries.
 * Branch: California_branch
 *
 * Revision 3.5.12.6  1996/08/09  23:42:16  ravip
 * CSCdi50688:  dlsw netbios slow session bring up with peer on demand
 * Branch: California_branch
 *
 * Revision 3.5.12.5  1996/08/09  21:55:22  ravip
 * CSCdi59181:  Best path not chosen for the first circuit
 * Branch: California_branch
 *
 * Revision 3.5.12.4  1996/08/09  19:47:51  ravip
 * CSCdi52387:  DLSw:CPUHOG process=DLSw Background, PC=6055C4E8
 *              Change CSM timer to use managed timers
 * Branch: California_branch
 *
 * Revision 3.5.12.3  1996/06/02  05:09:31  schiang
 * CSCdi57445:  DLSw+ local switch doesnt block traffic from RSRB to
 * non-NCIA IFs
 * Branch: California_branch
 *
 * Revision 3.5.12.2  1996/05/30  14:43:31  sbales
 * CSCdi59064:  DLSw+: show commands need to be more usable
 *              modify options for "sh dls reach", "sh dls cir", "sh dls
 *              loc", modify formatting for "show dls peer"
 * Branch: California_branch
 *
 * Revision 3.5.12.1  1996/05/17  10:45:17  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.5  1996/05/14  23:59:39  ioakley
 * CSCdi57686:  Streamline core_to_csm processing by replacing the
 *              core_to_csm function with inline calls to clsi/ssp/dlx
 *              handler routines from the dlsw_core.c module.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.4  1996/05/14  03:20:10  kpatel
 * CSCdi55075:  Cant define dlsw direct encapsulation for more that pvc
 *              dls frame-relay does not work over point to point
 *              sub-interface (CSCdi55085)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.3  1996/05/10  15:00:47  ioakley
 * CSCdi57325:  Remove dlsw reliance on port_number/slot_number
 *              and identify all dlsw ports by associated swidb.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.2  1996/04/25  16:20:06  ravip
 * CSCdi45773:  dlsw icanreach mac-exclusive command not working
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.1  1996/04/20  02:08:01  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5  1996/02/12  22:12:17  ravip
 * CSCdi48012:  1795 interoperability
 *              - Reverse source and destination mac addresses for
 *                SSP_OP_netbios_nr and _anr frames
 *              - Set remote data link correlator in SSP_OP_netbios_nr
 *                to origin data link correlator values received in
 *                SSP_OP_netbios_nq frame
 *
 * Revision 3.4  1995/12/21  00:36:04  kpatel
 * CSCdi45362:  DLSw: Netbios call doesnt go thru for DLSw over
 *              FrameRelay
 *
 * Revision 3.3  1995/12/12  17:36:23  ravip
 * CSCdi44808:  crash in csm_clsi_handler
 *
 * Revision 3.2  1995/11/17  09:02:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:23:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:55:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/13  17:48:39  ravip
 * CSCdi35261:  DLSw should permit ARB from target
 *              Nerd knob to control ARB and SRB of SNA and Netbios frames
 *              Store test frames if search is pending for the dmac
 *
 * Revision 2.1  1995/06/07  20:26:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */





#ifndef __DLSW_CSM_EXTERNS_H__
#define __DLSW_CSM_EXTERNS_H__

#include "../cls/clsi_msg.h"
#include "../dlsw/dlsw_peer_externs.h"


/*
 * Configuration parameters
 */
extern queuetype dlsw_ctlQ;
extern boolean   csm_load_balance;
extern uchar     dlsw_sna_xcast_flag;
extern uchar     dlsw_nb_xcast_flag;

#define BOX_MTU_SIZE	(rc_code_to_bytes((peer_get_box_largest_frame())))


/*  Timers  */
extern ulong csm_sna_expl_rt;
extern ulong csm_sna_expl_to;
extern ulong csm_sna_cache_to;
extern ulong csm_sna_cache_verify_to;
extern ulong csm_nb_expl_to;
extern ulong csm_nb_expl_rt;
extern ulong csm_nb_cache_to;
extern ulong csm_nb_cache_verify_to;
extern ulong csm_negative_cache_to;
extern ulong csm_expl_wait_rt;
extern ulong csm_expl_delay;
extern ulong csm_pod_retry_to;
 
extern const char def_mac_mask[IEEEBYTES];

/* Touch Timers */
extern ulong csm_touch_sna_cache_active_to;
extern ulong csm_touch_sna_cache_reset_to;
extern ulong csm_touch_nb_cache_active_to;
extern ulong csm_touch_nb_cache_reset_to;

/*
 * Information on bridge groups attached to DLSw
 */
typedef struct _dlsw_bgroup_t {
    struct _dlsw_bgroup_t *next;
    idbtype *tbridge_idb;	/* ptr to vidb linked into bridge group */
    ushort span_index;		/* index of bridge group attached to DLSw */
    int   sap_prilist_num;
    int   sna_la_prilist_num;
    ulong llc2_akmax;               /* Max number unack'd I-frames rcv'd     */
    ulong llc2_akdelay_time;        /* Max time I-frame remains unack'd      */
    ulong llc2_idle_time;           /* IDLE time  - freq. of idle polling    */
    ulong llc2_wind;                /* Max number unack'd I-frames xmit'd    */
    ulong llc2_n2;                  /* N2 retransmission count               */
    ulong llc2_T1_time;             /* T1 time - wait for ack of I-frame     */
    ulong llc2_tbusy_time;          /* TBUSY time - wait before re-polling   */
    ulong llc2_trej_time;           /* TREJ time  - wait for reject I-frame  */
    ulong llc2_tpf_time;            /* TPF time   - wait for final response  */
    ulong llc2_xid_neg_val_time;    /* Frequency of XID                      */
    ulong llc2_xid_rtry_time;       /* XID retry time - wait for XID rsp     */
    ulong llc2_txQ_max;             /* Max depth of LLC2 transmit queue      */
} dlsw_bgroup_t;

/*
 * Does not include SAPs.??
 * SAP values comes from config parser and goes down to the
 * dlc providers.
 */
typedef struct _dlsw_ring_t {
    struct _dlsw_ring_t *next;
    hword port_type;		/* port type */
    idbtype *swidb;             /* sw idb */
    hword ring_number;          /* source ring */
    hword ring_group;           /* target-virtual ring */
    hword bridge_number;	/* bridge number */
    hword trans_bridge_group;   /* for ethernet ... transparent bridge group */
    word  mtu_size;	        /* MTU  */
    word  p_sap_id; 		/* This is the bridging SAP ID for the entire i/f */
    uchar enabled:1;           /* true if actrng is success */
    queuetype dlsw_sapQ;
    boolean ignore_port_lf;
} dlsw_ring_t;

extern queuetype dlsw_ringQ;

typedef struct _dlsw_sap_t {
    struct _dlsw_sap_t *next;
    idbtype    *swidb;             /* sw idb */
    uchar 	sap;
    word  	mtu_size;	        /* MTU  */
    word  	p_sap_id;
    uchar enabled:1;           /* true if actrng is success */
} dlsw_sap_t;


/*
 * Configuration Command Function Prototypes
 */
extern void dlsw_csm_command (parseinfo *);
extern char *csm_dlx (int);

/*
 * Info needed to parse "show dls reachability" command
 */

#define LCACHE_MAC_FLAG  0
#define RCACHE_MAC_FLAG  1
#define LCACHE_NAME_FLAG 2
#define RCACHE_NAME_FLAG 3

/* 
 * flags 
 */
#define FUNC_ADDR_BIT     0x80
#define LOCAL             1
#define REMOTE            2
#define MAX_COST          99
#define FORCE             TRUE
#define NO_FORCE          FALSE

typedef enum {
    USER_CONFIGURED = 1,
    CAP_XCHANGE
    } cache_source_t;

typedef enum {
    NOT_EXCLUSIVE,
    EXCLUSIVE
    } exclusivity_t;

typedef enum {
    SET = 1,
    CLEAR = 2
    } cache_action_t;

typedef enum {
    REACHABLE = 1,
    NOT_REACHABLE
    } reach_sense;

typedef enum {
    MAC_ADDRESS = 1,
    NB_NAME
    } resource_type_t;

typedef enum {
    NO_OBJECT_TYPE = 0,
    RIF_STRING,
    RING_ID,
    PEER_ID,
    PEER_GROUP_ID,
    RING_GROUP_ID,
    PEER_IP_ADDR,
    PEER_HWIDB,
    PORT_ID
    } object_type_t;

typedef dlsw_ring_t* lportid_t;

typedef union {
    ushort        rif[SRB_MAXRIF/2];
    ushort        ring_id;
    ushort        ring_group_id;
    peerhandle_t  peer_id;
    ushort        peer_group_id;
    ipaddrtype    peer_ip_addr;
    hwidbtype     *peer_hwidb;
    lportid_t     port_id;
    } config_obj_u;

typedef union u_portid_type_ {
    peerhandle_t peer_id;
    lportid_t    port_id;
    ulong        ring_id;
    } u_portid_type_t;

typedef struct reach_entry_ {
    u_portid_type_t prp_id;
    long            mtu_size;
    ushort          reach_cost;
    uchar           rif[SRB_MAXRIF]; /* change later to store rif id */
    char            flags;
    char            pad[3];
    } reach_entry_t;

typedef union u_msg_ptr_ {
    paktype     *pak;
    TCLSIMsg    *clsi_msg;
    } u_msg_ptr_t;
 
/*
 * CSM / Core interface
 */
typedef enum {
    NO_OP,          /* just calling for fun */
    CLSI_MSG_PROC,
    DLX_MSG_PROC,
    SSP_MSG_PROC,
    DLX_START_NEWDL,
    SSP_START_NEWDL,
    CLSI_START_NEWDL,
    UPDATE_CACHE
    } csm_op_t;
 
typedef enum {
    FRAME_REJ,
    PROC_OK,
    NEW_CONNECTION,
    POD_PEER_NOT_READY,
    NO_REACH_ENTRY,
    POD_PEER_INVALID
    } csm_rc_t;
 
typedef struct csm_info_ {
    uchar    *saddr;
    uchar    *daddr;
    uchar    ssap;
    uchar    dsap;
    uchar    pf_flag;
    uchar    ctrl;
    uchar    rif[SRB_MAXRIF]; /* lf adjusted for lan interface mtu and box lf */
    long     mtu_bytes; /* min(packet lf, lan interface mtu, and box lf */
    uchar    mtu_code;
    netbios_header *nb_hdr;
} csm_info_t;

/* CSM's Managed timer masters */
extern mgd_timer csm_cache_del_timer;       
extern mgd_timer csm_cache_expl_timer;      
extern mgd_timer csm_resp_retry_timer;     
extern mgd_timer csm_resp_del_timer;      
extern mgd_timer csm_icr_del_timer;      
extern mgd_timer csm_icr_respond_timer;      
extern mgd_timer csm_startdl_del_timer; 
extern mgd_timer csm_startdl_retry_timer; 
extern mgd_timer csm_startdl_respond_timer; 
extern mgd_timer csm_nr_retry_timer;   
extern mgd_timer csm_nr_del_timer;    
extern mgd_timer csm_dlxnr_del_timer;  
extern mgd_timer csm_anq_del_timer;     
extern mgd_timer csm_anq_retry_timer;     

extern sys_timestamp    csm_expl_delay_timer;  /* explorer delay timer */

/* timer types */
#define CSM_RMAC_CACHE      1
#define CSM_LMAC_CACHE      2
#define CSM_RNAME_CACHE     3
#define CSM_LNAME_CACHE     4

/*
 * CSM function prototypes
 */
extern void dlsw_csm_init (void);
extern void csm_house_keeping (void);
extern paktype *get_dlx_packet (uchar, ulong, ulong, uchar);
extern paktype *get_ssp_packet (uchar, ulong, uint, ulong, uchar);
extern csm_rc_t csm_to_core (csm_op_t, u_portid_type_t *, u_msg_ptr_t *,
                             reach_entry_t *);
extern void csm_to_local (dlsw_ring_t *src_dgra, reach_entry_t *dst_re,
                          TCLSIMsg  *src_clsi_msg);
extern void csm_update_mac_entry (cache_action_t, cache_source_t,
                                  char *, char *, reach_sense,
                                  object_type_t, config_obj_u);
extern void csm_update_nbname_entry (cache_action_t, cache_source_t,
                                     char *, reach_sense, object_type_t, 
                                     config_obj_u);
extern void csm_update_sap_entry (cache_action_t, cache_source_t, uchar,
                                  reach_sense, peerhandle_t);
extern void csm_clear_sap_entries (cache_source_t);
extern void csm_clear_cap_entries (peerhandle_t);
extern void csm_destroy_peer(peerhandle_t);
extern void csm_delete_cache(peerhandle_t);
extern void clear_csm_cache(parseinfo *);
extern boolean str_has_wc(char*);
extern int wc_str_cmp (char *, char *);
extern void csm_activate_static(dlsw_ring_t *);
extern int send_ui_frame_to_peers (peerhandle_t *, int, char *, int,
                            uchar, lportid_t, char, long, char, ulong,
                            ulong, csm_info_t *);
extern void nbnr_recd_update_info (char direction, char *src_name,
                                   char *dst_name, u_portid_type_t *prp_id,
                                   char *rif);
extern void dlsw_core2csm_update_cache(csm_op_t op, 
				       resource_type_t,
				       uchar dirn,
				       char *src_adress, 
				       char *dest_adress, 
				       dlsw_ring_t *port_id,
				       peerhandle_t peer_handle,
				       uchar *local_rif,
				       uchar *data_ptr);

extern void dlsw_fct_lan2wan_start (TCLSIMsg *clsi_msg,
                                    peerhandle_t peer_handle,
                                    uchar *macs, uchar dsap, uchar ssap);
extern void dlsw_fct_wan2lan_start (paktype *pak, peerhandle_t peer_handle,
                                    dlsw_ring_t *port_ptr, uchar *rif);

extern boolean csm_local_mac_excl_pass (char *mac_addr);
extern boolean csm_local_nb_excl_pass (char *nb_name);
extern boolean csm_peer_mac_excl_pass (peerhandle_t ph, char *mac_addr);
extern boolean csm_peer_nb_excl_pass (peerhandle_t ph, char *nb_name);
extern csm_rc_t csm_clsi_handler(lportid_t, TCLSIMsg *);
extern csm_rc_t csm_ssp_handler (peerhandle_t, paktype *);
extern csm_rc_t csm_dlx_handler (peerhandle_t, paktype *);
 
extern uchar *get_daddr_from_clsi(TCLSIMsg *);
extern uchar *get_saddr_from_clsi(TCLSIMsg *);
extern int  get_rif_from_clsi(TCLSIMsg *, uchar *rif);
extern uchar get_dsap_from_clsi(TCLSIMsg *);
extern uchar get_ssap_from_clsi(TCLSIMsg *);
extern uchar get_pfflag_from_clsi(TCLSIMsg *);
extern uchar get_cmdrsp_from_clsi(TCLSIMsg *);
extern uchar get_ctrl_from_clsi(TCLSIMsg *);
extern uchar *get_usrdata_from_clsi(TCLSIMsg *);
extern int get_usrdata_len_from_clsi(TCLSIMsg *);
extern long portid_to_mtu(lportid_t);
extern long peerid_to_mtu(peerhandle_t);
extern void csm_start_explr_delay_timer(void);
extern int ssp_lf_bits_to_bytes (uchar ssp_lf_bits);
extern uchar bytes_to_ssp_lf_bits (int bytes);
extern uchar cisco_to_1795_lf_bits (uchar ssp_lf_bits);
extern ushort portid2ringid (lportid_t);

extern void delete_xidfailure_mac_entry (uchar *, u_portid_type_t *);

/*
 * dlsw_ignore_port_lf_type
 *
 * Return TRUE if lan type does not negotiate an lf.
 */

static inline boolean dlsw_ignore_port_lf_type (ushort port_type)
{
    switch (port_type) {
      case CLS_SDLC:
      case CLS_QLLC:
      case CLS_VDLC:
          return (TRUE);
      default:
          return(FALSE);
    }
}


/*
 * dlsw_ignore_port_lf
 *
 * Return TRUE if lan type does not negotiate an lf.
 */

static inline boolean dlsw_ignore_port_lf (dlsw_ring_t *dlsw_ring)
{
    return(dlsw_ring->ignore_port_lf);
}


/* flags for switch table */
#define NO_LOCAL_SWITCH         0x01
#define LOCAL_SWITCH            NO_LOCAL_SWITCH
#define NO_REMOTE               0x02
#define REMOTE_SWITCH           NO_REMOTE
#define NO_NETBIOS              0x04
#define NETBIOS_SUPPORT         NO_NETBIOS


/*
 * local_switch field in the dlsw_ring_t structure was used to decide if an
 * interface supports local switch or not.  Since we need to store more
 * switch-ability info, dlsw_switch_table is used to replace the local_switch.
 * dlsw_switch_table is a two dimension array.  It uses the CLS type as index.
 * each element is an unsigned char which can store eight different info.
 * Currently, three switch-ability info are supported:
 * NO_LOCAL_SWITCH: indicates local switch is not supported between the given
 * two cls type.  The first index is the from port type and the second index is
 * the to port type.
 * NO_REMOTE: indicate the remote switch is not supported for a particular
 * port type.  The info is stored in the element where the first index is the
 * same as the second index.
 * NO_NETBIOS: indicate the NetBIOS traffic is  not supported for a particular
 * port type.  The info is stored in the element where the first index is the
 * same as the second index.
 */

static const uchar dlsw_switch_table[CLS_MAXDLC][CLS_MAXDLC] =  {

/* ILLEGAL  */ 
	{
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ILLEGAL 	*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* SDLC		*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* SOCKET	*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* LLC		*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* LOCAL_ACK	*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* RSRB		*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* TBRIDGE	*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* FR		*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ATM		*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* QLLC		*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* VDLC		*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* PPP		*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS 	/* NCIA		*/
	},

/* SDLC */ 
	{
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ILLEGAL 	*/
	NO_NETBIOS,				/* SDLC		*/
	NO_NETBIOS,				/* SOCKET	*/
	NO_NETBIOS,				/* LLC		*/
	NO_NETBIOS,				/* LOCAL_ACK	*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* RSRB		*/
	NO_NETBIOS,				/* TBRIDGE	*/
	NO_NETBIOS,				/* FR		*/
	NO_NETBIOS,				/* ATM		*/
	NO_NETBIOS,				/* QLLC		*/
	NO_NETBIOS,				/* VDLC		*/
	NO_NETBIOS,				/* PPP		*/
 	NO_NETBIOS				/* NCIA		*/
	},

/* SOCK */ 
	{
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ILLEGAL 	*/
	NO_NETBIOS,				/* SDLC		*/
	NO_LOCAL_SWITCH,			/* SOCKET	*/
	NO_LOCAL_SWITCH,			/* LLC		*/
	NO_LOCAL_SWITCH,			/* LOCAL_ACK	*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* RSRB		*/
	NO_LOCAL_SWITCH,			/* TBRIDGE	*/
	NO_LOCAL_SWITCH,			/* FR		*/
	NO_LOCAL_SWITCH,			/* ATM		*/
	NO_NETBIOS,                             /* QLLC		*/
	0,					/* VDLC		*/
	NO_LOCAL_SWITCH,			/* PPP		*/
 	0 			 		/* NCIA		*/
	},

/* LLC  */ 
	{
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ILLEGAL 	*/
	NO_NETBIOS,				/* SDLC		*/
	NO_LOCAL_SWITCH,			/* SOCKET	*/
	NO_LOCAL_SWITCH,			/* LLC		*/
	NO_LOCAL_SWITCH,			/* LOCAL_ACK	*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* RSRB		*/
	NO_LOCAL_SWITCH,			/* TBRIDGE	*/
	NO_LOCAL_SWITCH,			/* FR		*/
	NO_LOCAL_SWITCH,			/* ATM		*/
	NO_NETBIOS,                             /* QLLC		*/
	0,					/* VDLC		*/
	NO_LOCAL_SWITCH,			/* PPP		*/
 	0  					/* NCIA		*/
	},

/* LOCAL_ACK */ 
	{
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ILLEGAL 	*/
	NO_NETBIOS,				/* SDLC		*/
	NO_LOCAL_SWITCH,			/* SOCKET	*/
	NO_LOCAL_SWITCH,			/* LLC		*/
	NO_LOCAL_SWITCH,			/* LOCAL_ACK	*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* RSRB		*/
	NO_LOCAL_SWITCH,			/* TBRIDGE	*/
	NO_LOCAL_SWITCH,			/* FR		*/
	NO_LOCAL_SWITCH,			/* ATM		*/
	NO_NETBIOS,                             /* QLLC		*/
	0,					/* VDLC		*/
	NO_LOCAL_SWITCH,			/* PPP		*/
 	0 			 		/* NCIA		*/
	},

/* RSRB */ 
	{
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ILLEGAL 	*/
 	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* SDLC		*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* SOCKET	*/
 	NO_LOCAL_SWITCH|NO_REMOTE,		/* LLC		*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* LOCAL_ACK	*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* RSRB		*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* TBRIDGE	*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* FR		*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* ATM		*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* QLLC		*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* VDLC		*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* PPP		*/
   	NO_REMOTE  				/* NCIA		*/
	},

/* TBRIDGE */ 
	{
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ILLEGAL 	*/
	NO_NETBIOS,				/* SDLC		*/
	NO_LOCAL_SWITCH,			/* SOCKET	*/
	NO_LOCAL_SWITCH,			/* LLC		*/
	NO_LOCAL_SWITCH,			/* LOCAL_ACK	*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* RSRB		*/
	NO_LOCAL_SWITCH,			/* TBRIDGE	*/
	NO_LOCAL_SWITCH,			/* FR		*/
	NO_LOCAL_SWITCH,			/* ATM		*/
	NO_NETBIOS,                             /* QLLC		*/
	0,					/* VDLC		*/
	NO_LOCAL_SWITCH,			/* PPP		*/
	0			  		/* NCIA		*/
	},

/* FR   */ 
	{
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ILLEGAL 	*/
	NO_NETBIOS,				/* SDLC		*/
	NO_LOCAL_SWITCH,			/* SOCKET	*/
	NO_LOCAL_SWITCH,			/* LLC		*/
	NO_LOCAL_SWITCH,			/* LOCAL_ACK	*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* RSRB		*/
	NO_LOCAL_SWITCH,			/* TBRIDGE	*/
	NO_LOCAL_SWITCH,			/* FR		*/
	NO_LOCAL_SWITCH,			/* ATM		*/
	NO_NETBIOS,                             /* QLLC		*/
	0,					/* VDLC		*/
	NO_LOCAL_SWITCH,			/* PPP		*/
 	0  					/* NCIA		*/
	},

/* ATM  */ 
	{
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ILLEGAL 	*/
	NO_NETBIOS,				/* SDLC		*/
	NO_LOCAL_SWITCH,			/* SOCKET	*/
	NO_LOCAL_SWITCH,			/* LLC		*/
	NO_LOCAL_SWITCH,			/* LOCAL_ACK	*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* RSRB		*/
	NO_LOCAL_SWITCH,			/* TBRIDGE	*/
	NO_LOCAL_SWITCH,			/* FR		*/
	NO_LOCAL_SWITCH,			/* ATM		*/
	NO_NETBIOS,                             /* QLLC		*/
	0,					/* VDLC		*/
	NO_LOCAL_SWITCH,			/* PPP		*/
 	0 					/* NCIA		*/
	 },

/* QLLC */ 
	{
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ILLEGAL 	*/
	NO_NETBIOS,				/* SDLC		*/
	NO_NETBIOS,				/* SOCKET	*/
	NO_NETBIOS,				/* LLC		*/
	NO_NETBIOS,				/* LOCAL_ACK	*/
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* RSRB		*/
	NO_NETBIOS,				/* TBRIDGE	*/
	NO_NETBIOS,				/* FR		*/
	NO_NETBIOS,				/* ATM		*/
	NO_NETBIOS,				/* QLLC		*/
	NO_NETBIOS,				/* VDLC		*/
	NO_NETBIOS,				/* PPP		*/
 	NO_NETBIOS  				/* NCIA		*/
	},

/* VDLC */ 
	{
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ILLEGAL 	*/
	NO_NETBIOS,				/* SDLC		*/
	NO_LOCAL_SWITCH,			/* SOCKET	*/
	0,					/* LLC		*/
	NO_LOCAL_SWITCH,			/* LOCAL_ACK	*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* RSRB		*/
	0,					/* TBRIDGE	*/
	NO_LOCAL_SWITCH,			/* FR		*/
	NO_LOCAL_SWITCH,			/* ATM		*/
	NO_NETBIOS,                             /* QLLC		*/
	0,					/* VDLC		*/
	NO_LOCAL_SWITCH,			/* PPP		*/
 	0 					/* NCIA		*/
	 },

/* PPP  */ 
	{
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ILLEGAL 	*/
	NO_NETBIOS,				/* SDLC		*/
	NO_LOCAL_SWITCH,			/* SOCKET	*/
	NO_LOCAL_SWITCH,			/* LLC		*/
	NO_LOCAL_SWITCH,			/* LOCAL_ACK	*/
	NO_LOCAL_SWITCH|NO_REMOTE,		/* RSRB		*/
	NO_LOCAL_SWITCH,			/* TBRIDGE	*/
	NO_LOCAL_SWITCH,			/* FR		*/
	NO_LOCAL_SWITCH,			/* ATM		*/
	NO_NETBIOS,                             /* QLLC		*/
	0,					/* VDLC		*/
	NO_LOCAL_SWITCH,			/* PPP		*/
 	0  					/* NCIA		*/
	},

/* NCIA */ 
	{
	NO_LOCAL_SWITCH|NO_REMOTE|NO_NETBIOS,	/* ILLEGAL 	*/
	NO_NETBIOS,				/* SDLC		*/
	0,					/* SOCKET	*/
	0,					/* LLC		*/
	0,					/* LOCAL_ACK	*/
    	NO_REMOTE,				/* RSRB		*/
	0,					/* TBRIDGE	*/
	0,					/* FR		*/
	0,					/* ATM		*/
	NO_NETBIOS,				/* QLLC		*/
	0,					/* VDLC		*/
	0,					/* PPP		*/
	0  					/* NCIA		*/
	}
};




/*
 *  check if switch is allowed or not by checking port type
 */
static inline boolean dlsw_can_switch_by_type (CLSDLCType_e port_type1, 
                             CLSDLCType_e port_type2, uchar switch_type)
{
    return(!(dlsw_switch_table[port_type1][port_type2] & switch_type));
}



/*
 *  check if switch is allowed or not by checking idbtype
 */
static inline boolean dlsw_can_switch (idbtype *swidb1, idbtype *swidb2, 
                                       uchar switch_type) {
    return(dlsw_can_switch_by_type(get_cls_type(swidb1->hwptr), 
                       get_cls_type(swidb2->hwptr), switch_type) );
}




#endif /* __DLSW_CSM_EXTERNS_H__ */


