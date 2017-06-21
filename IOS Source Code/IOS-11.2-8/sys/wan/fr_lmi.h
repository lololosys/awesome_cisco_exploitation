/* $Id: fr_lmi.h,v 3.1.22.2 1996/06/18 23:21:41 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_lmi.h,v $
 *------------------------------------------------------------------
 * Frame Relay LMI
 *
 * January 1996, Charles Poe
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved. 
 *------------------------------------------------------------------
 * $Log: fr_lmi.h,v $
 * Revision 3.1.22.2  1996/06/18  23:21:41  widmer
 * CSCdi59768:  DLCI is not validated in EZSetup
 * Branch: California_branch
 *
 * Revision 3.1.22.1  1996/04/25  23:23:27  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.30.1  1996/04/24  04:11:23  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1996/02/13  23:31:07  ccpoe
 * Frame Relay LMI
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

# define MAX_IE_BYTES 8
# define FR_REPORT_TYPE_IE_LENGTH 1
# define REPORT_TYPE_IE_ID 1

/*			       
 * defs moved from interface.h  
 */
# define FR_REPORT_NOTSEEN        0xff
# define FR_REPORT_KEEPALIVE_ONLY 1
# define FR_REPORT_FULL_STATUS    0
# define MAX_LMI_EVENTS 11	
# define MAX_LMI_PROCESS 2	



# define LMI_STATUS_ENQUIRY 0x75
# define LMI_STATUS 0x7d
# define LMI_UPDATE_STATUS 0x7b
# define FRAME_RELAY_LMI_DBIT 0x4
# define NDBITS(x) ((x &0xc) >> 2)
# define FRAME_RELAY_LMI_OVERHEADBYTES 5
# define FRAME_RELAY_ANSI_LMI_OVERHEADBYTES 6
# define FRAME_RELAY_LMI_HEADER_SIZE 6
# define FRAME_RELAY_IE_OVERHEADBYTES 2

# define IS_FRAME_RELAY_LMI_PKT(x) ((x == UNNUMBERED_FRAME_ID))
# define IS_FRAME_RELAY_LMI_IE_PKT(x) ((x == MULTICAST_IE_ID)   || \
				       (x == PVC_IE_ID)         || \
				       (x == KEEPALIVE_IE_ID)   || \
				       (x == REPORT_TYPE_IE_ID) || \
 				       (x == CCITT_PVC_IE_ID)   || \
 				       (x == CCITT_REPORT_IE_ID)|| \
 				       (x == CCITT_KEEP_IE_ID))

# define IS_FRAME_RELAY_PVC_IE(x) ((x == PVC_IE_ID)         || \
				   (x == CCITT_PVC_IE_ID))

# define ANSI_PVC_LEN 5
# define CISCO_PVC_LEN 8
# define MULTICAST_IE_ID 5
# define FR_MULTICAST_IE_LENGTH 6
# define PVC_IE_ID 7
# define PVC_IE_LENGTH 6
# define ANSI_PVC_IE_LENGTH 3
# define KEEPALIVE_IE_ID 3
# define KEEPALIVE_IE_LENGTH 2

# define UNNUMBERED_FRAME_ID 3
# define LMI_PROTOCOL_DISCRIMINATOR 0x09
# define ANSI_LMI_PROTOCOL_DISCRIMINATOR 0x08
# define LMI_PROT_DISC 0x09
# define ANSI_LMI_PROT_DISC 0x08
# define LMI_CALL_REF 0
# define LOCK_SHIFT 0x95
# define FR_SVC_CALL_REF      0x02
# define INVALID_CALL_REF_VALUE		81
# define FR_SVC_PID                   0xFF
# define T_ACTIVITY_DEF               30 * ONESEC
# define LMI_ROUTE_INTERVAL 	10


/*
 * if the order of these enums are changed, be sure to change
 * the corresponding strings (disp_lmi) in fr_lmi.c
 */
enum FR_LMI_TYPE {
    FR_CISCO_LMI = 1,
    FR_CCITT_LMI  = 2,
    FR_ANSI_D = 3};

enum FR_LMI_RTN_VAL {
    FR_LMI_FAIL,
    FR_LMI_IE_FAIL,
    FR_LMI_PASS };

enum FR_LMI_MODE {
    FR_LMI_RCVD,
    FR_LMI_SENT
    };

enum LMI_TIMER {
    LMI_IDB_TIMER,
    LMI_ERR_TIMER,
    LMI_TUNNEL_TIMER,
    LMI_ROUTE_TIMER
};

typedef struct fr_lmi_protocol_discriminator_ {
	uchar lmi_type;
	uchar prot_disc;
} fr_lmi_protocol_discriminator;


/*
 * frame relay LMI structs
 */
typedef struct fr_lmi_stats_type_ {
    ulong invalid_4company_dlci;          /* invalid dlci rcvd */
    ulong invalid_annexd_dlci;            /* invalid annex d dlci */
    ulong invalid_ui;                     /* wrong unnumbered info value */
    ulong invalid_protocol_discriminator; /* wrong prot disc */
    ulong invalid_dummy_call_ref;         /* must always be 0 */
    ulong invalid_msg_type;               /* other than ENQUIRY */
    ulong invalid_status_msg;             /* if we are a DTE */
    ulong invalid_locking_shift;          /* must be 0x95 */
    ulong invalid_ie_id;                  /* neither report nor keep */
    ulong invalid_report_type_ie_len;
    ulong invalid_report_request;
    ulong invalid_keepalive_ie_len;
    ulong num_enquiries_rcvd;
    ulong num_enquiries_sent;             /* if we are a DTE */
    ulong num_status_rcvd;                /* if we are a DTE */
    ulong num_status_sent;
    ulong num_upd_status_rcvd;
    ulong num_upd_status_sent;
    ulong num_full_status_rcvd;           /* if we are a DTE */
    ulong num_full_status_sent;
    ulong num_dte_timeouts;
    ulong num_dce_timeouts;
} fr_lmi_stats_type;

typedef struct fr_report_type_struct_ {
    uchar report_type_ie_id;
    uchar report_type_ie_length;
    uchar report_type_ie_req;
} fr_report_type_struct; 

struct frame_relay_keepalive_struct {
    uchar keepalive_ie_id;
    uchar keepalive_ie_length;
    uchar current_seq;
    uchar received_seq;
};


struct fr_ansi_pvc_struct {
    uchar pvc_id_ie;
    uchar pvc_id_length;
    ushort pvc_dlci;
    uchar pvc_status;
};

union frame_relay_ie_types {
    struct frame_relay_multicast_struct frame_relay_multicast_ie;
    struct frame_relay_pvc_struct frame_relay_pvc_ie;
    struct fr_ansi_pvc_struct ansi_pvc_ie;  
    struct frame_relay_keepalive_struct frame_relay_keepalive_ie;
    fr_report_type_struct fr_report_type_ie;
    uchar ie_bytes[MAX_IE_BYTES];
};

struct frame_relay_lmi {
    ushort lmi_dlci;
    uchar unnumbered_frame_id;
    uchar lmi_protocol_discriminator;
    uchar lmi_call_ref;
    uchar message_type_id;
    union frame_relay_ie_types PACKED(frame_relay_ies);
};

struct fr_idb_lmi {
    ushort fr_lmi_dlci;
    uchar  fr_report_id;
    uchar  fr_keepalive_id;
    uchar fr_dte_myseq;	        /* current probe sequence number */
    uchar fr_dte_yoursseen;	/* what we believe of other side */
    uchar fr_dte_mineseen;	/* what other side believes of us */
    uchar fr_dte_tmp_yoursseen; /* save until we're sure pkt is good */
    boolean fr_liv_seen;        /* LIV seen in message? */
    boolean fr_mc_seen;         /* MC IE seen */ 
    uchar fr_dce_myseq;
    uchar fr_dce_yoursseen;
    uchar fr_dce_mineseen;

    boolean fr_lmi_type_configured;
    boolean lmi_autosense;
    uint lmi_autosense_retry_count;

    enum FR_LMI_TYPE fr_lmi_type;
    /*
     * lmi parameters 
     */
    ushort fr_lmi_n391dte;
    ushort fr_lmi_n392dte;
    ushort fr_lmi_n393dte;
    ushort fr_lmi_t392dce;
    ushort fr_lmi_n392dce;
    ushort fr_lmi_n393dce;
    /*
     * lmi events for periodic manipulation
     */
    ushort lmi_n391dte;
    ushort lmi_n392[MAX_LMI_PROCESS];
    uchar  lmi_events[MAX_LMI_PROCESS][MAX_LMI_EVENTS];
    ushort lmi_old[MAX_LMI_PROCESS], lmi_new[MAX_LMI_PROCESS];
    boolean lmi_lineup[MAX_LMI_PROCESS];

    boolean full_status_rcvd;
    boolean status_rcvd;
    boolean full_status_req;
    uchar fr_rpt_type_processing;
    uchar fr_dte_rpt_type_sent;
    boolean fr_dlci_status_change;
    ushort fr_last_pvc_seen;
    enum FR_LMI_MODE fr_lmi_state;
    fr_lmi_stats_type *lmi_stats;   /* statistics for LMI */
    mgd_timer *idb_timer;
    mgd_timer *err_timer;
    mgd_timer *route_timer;  /* maintain pvc stats for DCE */
};

typedef struct fr_ie_length_ {
	ushort ie_type;
	ushort ie_length;
	ushort ie_min_length;
	ushort ie_max_length;
	int case_success;
	int case_failure;
} fr_ie_length;

/*
 * CCITT parameter values for IE ID's
 */
 
# define CCITT_PVC_IE_ID          0x57
# define CCITT_REPORT_IE_ID       0x51
# define CCITT_KEEP_IE_ID         0x53

/*
 * default values for lmi parameters
 */
# define FRAME_RELAY_DEFAULT_LMI 1023
  
# define DEFAULT_LMI_T392DCE         15
# define DEFAULT_LMI_N391DTE          6
# define DEFAULT_LMI_N392DTE          3
# define DEFAULT_LMI_N393DTE          4
# define DEFAULT_LMI_N392DCE          2
# define DEFAULT_LMI_N393DCE          2

# define lmi_idb fr_idb->fr_lmi

extern watched_queue *fr_lmiQ;

extern void lmi_parse(hwidbtype *, paktype *);
extern void send_frame_relay_keepalive(hwidbtype *);
extern void fr_lmi_event(fr_idb_struct_type *, ushort, uchar, ushort, ushort);
extern void fr_reset_lmi_events(fr_idb_struct_type *, ushort, uchar, 
				ushort, ushort);
extern void fr_lmi_set(hwidbtype *, fr_idb_struct_type *,
		       enum FR_LMI_TYPE,boolean, boolean);
extern boolean fr_lmi_timer_create(hwidbtype *, ulong);
extern void fr_lmi_timer_delete(hwidbtype *, ulong);
extern void show_lmi_stats(parseinfo *);
extern uchar *add_ansi_ccitt_pvc(fr_idb_struct_type *, uchar *,
				  fr_pvc_info_type *, int, uchar);
extern uchar *add_cisco_pvc (fr_idb_struct_type *, uchar *, 
			     fr_pvc_info_type *, int index, uchar);
extern int fr_ie_length_check(ushort , ushort );
extern void send_fr_switch_lmi_status(hwidbtype *, boolean, uchar);
extern void show_lmi_idb_stats (fr_idb_struct_type *);
extern forktype fr_lmi_process (void);

