/* $Id: dlsw_csm.h,v 3.4.12.8 1996/08/22 18:34:44 ravip Exp $
 *------------------------------------------------------------------
 * DLSw CSM (Circuit Setup Manager) Internal Header File
 *
 * August 1994, Ravi Periaswamy
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Contains declarations for the DLSw CSM module. This includes
 * typedefs, constants, data structures and function prototypes.
 *------------------------------------------------------------------
 * $Log: dlsw_csm.h,v $
 * Revision 3.4.12.8  1996/08/22  18:34:44  ravip
 * CSCdi66201:  DLSw load sharing broken
 * Branch: California_branch
 *
 * Revision 3.4.12.7  1996/08/16  06:55:46  ioakley
 * CSCdi58903:  Add dlsw touch timer support for sna/netbios
 *              reachability cache entries.
 * Branch: California_branch
 *
 * Revision 3.4.12.6  1996/08/09  23:42:09  ravip
 * CSCdi50688:  dlsw netbios slow session bring up with peer on demand
 * Branch: California_branch
 *
 * Revision 3.4.12.5  1996/08/09  21:55:10  ravip
 * CSCdi59181:  Best path not chosen for the first circuit
 * Branch: California_branch
 *
 * Revision 3.4.12.4  1996/08/09  19:47:48  ravip
 * CSCdi52387:  DLSw:CPUHOG process=DLSw Background, PC=6055C4E8
 *              Change CSM timer to use managed timers
 * Branch: California_branch
 *
 * Revision 3.4.12.3  1996/08/05  18:38:55  ravip
 * CSCdi62784:  dlsw netbios windows nt cant connect nq sent directed no nr
 * Branch: California_branch
 *
 * Revision 3.4.12.2  1996/06/02  05:09:29  schiang
 * CSCdi57445:  DLSw+ local switch doesnt block traffic from RSRB to
 * non-NCIA IFs
 * Branch: California_branch
 *
 * Revision 3.4.12.1  1996/05/17  10:45:03  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.4  1996/05/15  00:38:48  ravip
 * CSCdi50707:  dlsw the reachability search is linear, should be binary
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.2.3  1996/05/14  22:48:57  ioakley
 * CSCdi53536:  Ensure that static dlsw netbios-name cache entries
 *              remain in running-config following reload and also
 *              whenever the dlsw tcp connection is brought down.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.2.2  1996/05/02  00:05:21  ravip
 * CSCdi56281:  dlsw sna local switching could cause broadcast storm
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.2.1  1996/04/25  16:20:04  ravip
 * CSCdi45773:  dlsw icanreach mac-exclusive command not working
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4  1996/02/12  22:12:05  ravip
 * CSCdi48012:  1795 interoperability
 *              - Reverse source and destination mac addresses for
 *                SSP_OP_netbios_nr and _anr frames
 *              - Set remote data link correlator in SSP_OP_netbios_nr
 *                to origin data link correlator values received in
 *                SSP_OP_netbios_nq frame
 *
 * Revision 3.3  1996/02/07  21:43:31  ravip
 * CSCdi46935:  DLSw does not propogate largest frame bits correctly
 *
 * Revision 3.2  1995/11/17  09:02:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:23:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:55:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/10/20  14:08:20  ravip
 * CSCdi40722: Exclusive reachability option does not filter frames
 *             correctly. Fixed problems with deleting reachability cache.
 *
 * Revision 2.2  1995/06/13  17:48:14  ravip
 * CSCdi35261:  DLSw should permit ARB from target
 *              Nerd knob to control ARB and SRB of SNA and Netbios frames
 *              Store test frames if search is pending for the dmac
 *
 * Revision 2.1  1995/06/07  20:26:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




#ifndef __DLSW_CSM_H__
#define __DLSW_CSM_H__

/*
 * CSM Scope Externs
 */
extern boolean csm_load_balance;
extern boolean dlsw_csm_debug_error;
extern boolean dlsw_csm_debug_event;
extern boolean dlsw_csm_debug_verbose;
extern boolean dlsw_csm_debug_sna;
extern boolean dlsw_csm_debug_netb;
extern boolean dlsw_csm_debug_all;


/*
 * Default Value Constants
 */
#define CSM_DEF_EXPLQ_DEPTH        100 /* packets */
#define CSM_DEF_SNA_XCAST_FLAG     (RC_BROADBIT | RC_SPANBIT)
#define CSM_DEF_NB_XCAST_FLAG      (RC_BROADBIT | RC_SPANBIT)

/*
 * Default timer constants
 */
#define CSM_DEF_SNA_EXPL_TO        (180 * ONESEC)
#define CSM_DEF_SNA_CACHE_TO       (16 * ONEMIN)
#define CSM_DEF_SNA_VERIFY_INT     (4 * ONEMIN)
#define CSM_DEF_SNA_RETRY_INT      (30 * ONESEC)
#define CSM_DEF_NB_EXPL_TO         (6 * ONESEC)
#define CSM_DEF_NB_CACHE_TO        (16 * ONEMIN)
#define CSM_DEF_NB_VERIFY_INT      (4 * ONEMIN)
#define CSM_DEF_NB_RETRY_INT       (1 * ONESEC)
#define CSM_DEF_NEG_CACHE_TO       0
#define CSM_DEF_EXPL_WAIT_INT      0
#define CSM_DEF_EXPL_DELAY         0
#define CSM_DEF_POD_RETRY_TO       (1 * ONESEC)
 
#define CSM_DEF_POD_RETRY_COUNT    3

/*
 * Default touch timer constants
 */
#define CSM_DEF_TOUCH_SNA_CACHE_ACTIVE_TO     0
#define CSM_DEF_TOUCH_SNA_CACHE_RESET_TO      0
#define CSM_DEF_TOUCH_NB_CACHE_ACTIVE_TO      0
#define CSM_DEF_TOUCH_NB_CACHE_RESET_TO       0
#define CSM_TOUCH_CACHE_RESTART_DELTA         (20 * ONESEC) 

/*
 * DLX limits 
 */
#define MAX_REACH           4   /* No. of reachable node info stored    */

/* reachability cache flags */
#define AVAILABLE         1
#define USE_BIT           2
#define INDELIBLE         128

/* 
 * misc. stuff 
 */
#define DLSW_LPORT_ID_COOKIE    1   /* local port num. to use for ssp  */
#define DLSW_XPORT_ID_COOKIE    1   /* local xport num. to use for ssp */
#define DLSW_SAP_COOKIE         0x04 
#define DLSW_AC_COOKIE          AC_PRI0
#define DLSW_FC_COOKIE          FC_TYPE_LLC
#define DIRECT_FRAME            0   /* Send frame as non broadcast     */
#define MAX_TESTDATA_SIZE       256 /* maximum size of testframe data  */
#define MAX_UIDATA_SIZE         256 /* maximum size of uiframe data    */

/* Name query type flags */
#define NQ_FIND_NAME            0
#define NQ_CALL_NAME            1

/* error codes */
#define CSM_INVALID_PARAMS      -1
#define CSM_NO_REACH_ENTRY      -11
#define CSM_NO_MTU_SIZE         -12
#define CSM_NO_RING_MATCH       -13
#define CSM_NO_EXCL_MATCH       -14
#define CSM_NONE_AVAILABLE      -15
#define CSM_FRAME_FILTERED      -21


typedef enum {
    NO_SEND         = 0,
    ALL_LAN_PORTS   = 1,   /* Send frame to all lan ports        */
    SPEC_LAN_PORT   = 2,   /* Send frame to specific lan port    */
    SPEC_RING_GROUP = 3,   /* Send frame to specific ring groups */
    ALL_WAN_PEERS   = 4,   /* Send frame to all wan peers        */
    SPEC_WAN_PEER   = 5,   /* Send frame to specific wan peer    */
    SPEC_PEER_GROUP = 6,   /* Send frame to specific peer groups */
    CSM_LOCAL_SWITCH = 7
    } e_xmission_type;

typedef enum { 
    NO_CACHE_TYPE = 0,
    DYNAMIC = 1,
    CAP_EXCHANGE = 2,
    USER_CONF = 4,
    USER_CONF_RIF = 8,
    SAP_PEND = 16
    } e_cache_type;

typedef enum {
    FOUND = 1,
    NOT_FOUND = 2,
    UNCONFIRMED = 3,
    VERIFY = 4,
    SEARCHING = 5,
    NO_CACHE_INFO
    } e_cache_status;

typedef enum {
    DLX_MSG = 1,
    SSP_MSG,
    CLSI_MSG
    } e_message_pointer_type;

struct lcache_ {
    mgd_timer      del_tstamp;
    mgd_timer      explore_tstamp;
    sys_timestamp  verify_tstamp;
    reach_entry_t  r_entry[MAX_REACH];
    e_cache_type   cache_type;
    e_cache_status status;
    ushort         no_in_list;
    char           reach_direction;
    char           pad;
    };
typedef struct lcache_ lcache_t;


typedef union {
    ulong    peer_group_id;
    ulong    ring_group_id;
    } u_group_id_t;

struct group_cache_ {
    e_cache_type    cache_type;
    char            direction;
    u_group_id_t    group_id[MAX_REACH];
    };
typedef struct group_cache_ group_cache_t;

struct resp_pend_ {
    char            saddr[IEEEBYTES]; /* just to use for retries of test */
    char            ssap;             /* just to use for retries of test */
    char            flags;            /* wrench bit                      */
    peerhandle_t    peer_id;          /* NULL if just verifying          */
    e_xmission_type trans_type;
    long            mtu_size;         /* requested by peer               */
    char            mtu_flag;         /* requested by peer               */
    char            no_in_list;
    char            pad1[2];
    lportid_t       port_id[MAX_REACH]; /* list of rings/groups sent to  */
    uint            rem_lanport_id;
    uint            rem_transport_id;
    mgd_timer       retry_tstamp;
    mgd_timer       del_tstamp;
    char            eat_test_resp;
    char            rif[SRB_MAXRIF];
    lportid_t       localsw_portid; /* ignore if trans_type != local switch */
    };
typedef struct resp_pend_ resp_pend_t;

struct icr_pend_ {
    char            saddr[IEEEBYTES];
    char            ssap;
    char            dsap;
    lportid_t       port_id;  /* port id the test frame came in */
    mgd_timer       del_tstamp;
    mgd_timer       respond_timer;
    long            mtu_size;
    char            mtu_flag;     /* sent to peer */
    char            ctrl; /* including poll/final bit */
    char            rif[SRB_MAXRIF];
    char            cmd_rsp;
    char            pad1[3];
    int             test_data_len;
    char            test_data[MAX_TESTDATA_SIZE]; /* data in test frame */
    };
typedef struct icr_pend_ icr_pend_t;

struct startdl_pend_ {
    char            saddr[IEEEBYTES];
    char            type;             /* DLX, SSP, CLSI */
    char            mtu_flag;
    long            mtu_size;
    u_msg_ptr_t     u_msg_ptr;
    u_portid_type_t prp_id;
    mgd_timer       del_tstamp;
    mgd_timer       respond_timer;
    mgd_timer       retry_tstamp;  /* only for CLSI when pod is not ready */
    int             retry_count;
    reach_entry_t   retry_reach_info;
    };
typedef struct startdl_pend_ startdl_pend_t;


struct nb_retry_info_ {
    e_xmission_type trans_type;
    u_portid_type_t prp_id[MAX_REACH];
    char            daddr[IEEEBYTES];
    char            saddr[IEEEBYTES];
    char            no_in_list;
    char            dsap;
    char            ssap;
    char            xcast_type;
    int             ui_data_len;
    char            ui_data[MAX_UIDATA_SIZE];
    };
typedef struct nb_retry_info_ nb_retry_info_t;

struct nr_pend_ {
    char          sname[NETBIOS_MAX_NAME_LEN];
    peerhandle_t  peer_id;    
    long          mtu_size;
    char          mtu_flag;     /* received from peer */
    char          csex_flags;
    char          nq_call_flag;   /* 0 is find name and 1 is call */
    char          pad;
    mgd_timer     del_tstamp;
    mgd_timer     retry_tstamp;
    nb_retry_info_t retry_info;
    ulong         remote_dlc_port_id;
    ulong         remote_transport_id;
    };
typedef struct nr_pend_ nr_pend_t;

struct dlxnr_pend_ {
    char        sname[NETBIOS_MAX_NAME_LEN];
    lportid_t   port_id;
    long          mtu_size;
    char          mtu_flag;     /* received from peer */
    char          input_rif[SRB_MAXRIF];
    char          nq_call_flag;   /* 0 is find name and 1 is call */
    mgd_timer     del_tstamp;
    nb_retry_info_t retry_info;
    };
typedef struct dlxnr_pend_ dlxnr_pend_t;

struct nbanq_info_ {
    mgd_timer       del_tstamp;
    mgd_timer       retry_tstamp;
    nb_retry_info_t retry_info;
    u_portid_type_t prp_id;
    char            direction;
    };
typedef struct nbanq_info_ nbanq_info_t;

/*
 * structure to store unresolved cache entries configured by user or
 * received through cap exchange.
 */
typedef struct {
    char              mac_addr[IEEEBYTES];
    char              mac_mask[IEEEBYTES];
    cache_source_t    cache_type;
    object_type_t     obj_type;
    config_obj_u      obj;
    reach_sense       sense;
    } wc_mac_cache_t;

typedef struct {
    char    fx_mac_mask[IEEEBYTES];
    char    fx_mac_addr[IEEEBYTES];
    } mac_cache_key_t;


typedef struct {
    char              nb_name[NETBIOS_MAX_NAME_LEN];
    cache_source_t    cache_type;
    object_type_t     obj_type;
    config_obj_u      obj;
    reach_sense       sense;
} wc_name_cache_t;


/* some macros */
#define CSM_DEBUG        if ((dlsw_csm_debug_event) || (dlsw_csm_debug_verbose)) buginf
#define CSMV_DEBUG       if (dlsw_csm_debug_verbose) buginf
#define CSME_DEBUG       if ((dlsw_csm_debug_event) || (dlsw_csm_debug_error) || (dlsw_csm_debug_verbose)) buginf
#define SNA_CSM_DEBUG    if (((dlsw_csm_debug_all) || (dlsw_csm_debug_sna)) && ((dlsw_csm_debug_event) || (dlsw_csm_debug_verbose))) buginf
#define SNA_CSMV_DEBUG   if (((dlsw_csm_debug_all) || (dlsw_csm_debug_sna)) && (dlsw_csm_debug_verbose)) buginf
#define SNA_CSME_DEBUG   if (((dlsw_csm_debug_all) || (dlsw_csm_debug_sna)) && ((dlsw_csm_debug_event) || (dlsw_csm_debug_error) || (dlsw_csm_debug_verbose))) buginf
#define NETB_CSM_DEBUG   if (((dlsw_csm_debug_all) || (dlsw_csm_debug_netb)) && ((dlsw_csm_debug_event) || (dlsw_csm_debug_verbose))) buginf
#define NETB_CSMV_DEBUG  if (((dlsw_csm_debug_all) || (dlsw_csm_debug_netb)) && (dlsw_csm_debug_verbose)) buginf
#define NETB_CSME_DEBUG  if (((dlsw_csm_debug_all) || (dlsw_csm_debug_netb)) && ((dlsw_csm_debug_event) || (dlsw_csm_debug_error) || (dlsw_csm_debug_verbose))) buginf


extern db_master_t    *lcache_mac_id;
extern db_master_t    *lcache_name_id;
extern db_master_t    *rcache_mac_id;
extern db_master_t    *rcache_name_id;
extern db_master_t    *wcache_mac_id;
extern db_master_t    *wcache_name_id;
extern db_master_t    *icr_pend_id;
extern db_master_t    *resp_pend_id;
extern db_master_t    *dlxnr_pend_id;
extern db_master_t    *nr_pend_id;
extern db_master_t    *nbanq_file_id;
extern db_master_t    *startdl_pend_id;
 
#define LCACHE_MAC    lcache_mac_id
#define LCACHE_NAME   lcache_name_id
#define RCACHE_MAC    rcache_mac_id
#define RCACHE_NAME   rcache_name_id
#define WCACHE_MAC    wcache_mac_id
#define WCACHE_NAME   wcache_name_id
#define ICR_PEND      icr_pend_id
#define RESP_PEND     resp_pend_id
#define DLXNR_PEND    dlxnr_pend_id
#define NR_PEND       nr_pend_id
#define NBANQ_FILE    nbanq_file_id
#define STARTDL_PEND  startdl_pend_id
 
/* function prototypes */
extern void mac_found_update_cache (char *, char, u_portid_type_t *, 
                                    char *, long);
extern void name_found_update_cache (char *, char, u_portid_type_t *, 
                                     char *, long);
extern void update_csm_cache (db_master_t *, char *, u_portid_type_t *,
                              char *, long);
extern void delete_csm_cache (db_master_t *, char *, u_portid_type_t *,
                              char, char);
extern void get_mac_cache_entries(char *, lcache_t **, record_handle *,
					  lcache_t **, record_handle *);
extern void get_name_cache_entries(char *, lcache_t **, record_handle *,
					   lcache_t **, record_handle *);
extern int get_reach_entry_mac(lcache_t *, long, u_portid_type_t *, boolean);
extern boolean mk_ssp_cntrl_header_from_clsipkt (ssp_control_frame_t *,
                                                 TCLSIMsg *, char);
extern int wc_strn_cmp (char *, char *, int);
extern int masked_mac_cmp (char *mac1, char *mac2, char *mask);
extern void free_clsi_msg (TCLSIMsg *);
extern int csm_lan_put (int, lportid_t, uchar, uchar, uchar, char *, char *,
                        uchar, uchar, uchar, char *, long, char *, int,
                        peerhandle_t);
extern int csm_send_to_disp (lportid_t, uchar, uchar, uchar, char *, char *,
                             uchar, uchar, uchar, char *, uchar, char *, int                         );
extern int csm_peer_put (peerhandle_t peer_id, paktype *pak, char *daddr, 
			  char *saddr, char dsap, char ssap, 
                          uchar flags, lportid_t origin_port_id);
void get_csm_cache_entries(db_master_t *, char *,
			   lcache_t **, record_handle *);

/* 
 * external variables 
 */


#endif /* __DLSW_CSM_H__ */





