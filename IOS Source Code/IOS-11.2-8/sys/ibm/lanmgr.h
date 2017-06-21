/* $Id: lanmgr.h,v 3.3.26.5 1996/09/10 03:14:09 ioakley Exp $
 * $Source: /release/112/cvs/Xsys/ibm/lanmgr.h,v $
 *------------------------------------------------------------------
 * lanmgr.h -- Token Ring Lan Manager Support Vectors Definitions
 *
 * January 1991, David R. Hampton
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lanmgr.h,v $
 * Revision 3.3.26.5  1996/09/10  03:14:09  ioakley
 * CSCdi60079:  Correct clsi compliant LNM dlu clean-up problems
 *              that result in LNM clsi related error messages when
 *              "no source-bridge"/"source-bridge" commands are
 *              entered on token ring interfaces.
 * Branch: California_branch
 *
 * Revision 3.3.26.4  1996/08/28  12:46:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.26.3  1996/08/16  07:04:29  ioakley
 * CSCdi63385:  Prevent router crash caused by CLS Connect_Req failure
 *              and resolve LNM conn pending/reporting link issues.
 * Branch: California_branch
 *
 * Revision 3.3.26.2  1996/07/25  05:46:49  kchiu
 * CSCdi35303:  Token ring shut down with different segment number
 * Branch: California_branch
 *
 * Revision 3.3.26.1  1996/05/17  11:20:30  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.14.1  1996/03/29  19:36:53  ioakley
 * CSCdi52877:  VDLC merge into IbuMod_Calif - Resolve various LNM/VDLC
 *              inconsistencies & include LNM/VDLC disable port updates.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3  1996/01/18  22:15:24  ioakley
 * CSCdi41845:  Reliably associate reporting link and connection when
 *              establishing local clsi compliant LNM reporting links
 *
 * Revision 3.2  1995/11/17  09:22:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:01:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/28  09:25:33  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/15  22:00:57  hampton
 * Fix 24.5 day bugs in Lan Manager support.  While there, eliminate
 * direct references to msclock.  [CSCdi35914]
 *
 * Revision 2.1  1995/06/07 20:47:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *
 *                  MAC LEVEL PACKET DEFINITIONS
 *
 **********************************************************************/
#include "address.h"
#include "../cls/clsi_msg.h"

#define MAC_MAXSIZE 80
#define IEEEBYTES 6
#define NLNM_HASH_SIZE 20
#define NLNM_HASH_MASK (NLNM_HASH_SIZE - 1)

typedef struct _major_vector {
    ushort length;
    uchar  dst_class:4;
    uchar  src_class:4;
    uchar  command;
    charlong data;
} major_vector;

#define major_vector_headstart(ptr) ((ptr)->network_start)

#define MAC_MVID_LEN 4

#define MAX_TR_FRAMESIZE 17800

typedef struct _subvector_t {
    uchar length;
    uchar command;
    charlong PACKED(data);
} subvector_t;

#define SVID_HDR_LEN 2

typedef struct _long_subvector {
    uchar  length;	/* 0xFF by definition */
    uchar  command;
    ushort real_length;
    charlong data;
} long_subvector;

/*
 * Source and Destination Server Processes
 */
#define CLASS_RINGSTATION 0
#define CLASS_DLCLNMGR	  1
#define CLASS_CRS	  4
#define CLASS_RPS	  5
#define CLASS_REM	  6

/*
 * MAC Major Vectors - i.e. commands
 */
#define MVID_RESPONSE 		0x00
#define MVID_BEACON		0x02
#define MVID_CLAIM		0x03
#define MVID_RINGPURGE		0x04
#define MVID_AMP		0x05
#define MVID_SMP		0x06
#define MVID_DUPTEST		0x07
#define MVID_LOBETEST		0x08
#define MVID_TRANSMITFWD	0x09
#define MVID_REMOVE		0x0B
#define MVID_CHANGEPARAM	0x0C
#define MVID_INITIALIZE		0x0D
#define MVID_REQADDRESS		0x0E
#define MVID_REQSTATE		0x0F
#define MVID_REQATTACH		0x10
#define MVID_REQINIT		0x20
#define MVID_RPTADDRESS		0x22
#define MVID_RPTSTATE		0x23
#define MVID_RPTATTACH		0x24
#define MVID_RPTNEWMON		0x25
#define MVID_RPTNAUN		0x26
#define MVID_NNINCOMP		0x27
#define MVID_RPTMONERR		0x28
#define MVID_SOFTERR		0x29
#define MVID_RPTTXMTFWD		0x2A
#define MVID_LAST		MVID_RPTTXMTFWD

/*
 * MAC SubVectors - i.e. data
 */
#define SVID_BEACONTYPE		0x01	/* unused */
#define   BEACON_RECOVERYMODE 1
#define   BEACON_SIGNALLOSS   2
#define   BEACON_STRM_N_CLAIM 3
#define   BEACON_STRM_CLAIM   4
#define SVID_NAUN		0x02	/* received */
#define SVID_LOCALRING		0x03	/* sent */
#define SVID_ASSGN_PHYS_LOC	0x04	/* sent */
#define SVID_SOFTERRTIMER	0x05	/* sent */
#define SVID_ENABLEDCLASS	0x06	/* sent/received */
#define SVID_ALLOWEDPRIORITY	0x07	/* sent/received */
#define SVID_CORRELATOR		0x09	/* sent/received */
#define SVID_LASTADDRESS	0x0A	/* received */
#define SVID_PHYS_LOC		0x0B	/* received */
#define SVID_RESPONSE		0x20	/* received */
#define   RESP_ACK          0x0001
#define   RESP_ERROR	    0x8000
#define   RESP_NO_MVID	    0x8001
#define   RESP_MV_LEN	    0x8002
#define   RESP_UNKN_MVID    0x8003
#define   RESP_BAD_SCLASS   0x8004
#define   RESP_SV_LEN	    0x8005
#define   RESP_BAD_FWD	    0x8006
#define   RESP_NO_REQ_SVID  0x8007
#define   RESP_UNKN_SVID    0x8008
#define   RESP_TOO_LARGE    0x8009
#define   RESP_FN_DISABLED  0x800A
#define SVID_ADDRESS_MOD	0x21	/* received */
#define SVID_PRODUCTID		0x22	/* received */
#define   IDLEN 18
#define SVID_UCODELEVEL		0x23	/* received */
#define   UCODELEN 10
#define SVID_WRAPDATA		0x26	/* unused */
#define SVID_FRAMEFORWARD	0x27	/* sent */
#define SVID_STATIONID		0x28	/* received */
#define SVID_STATUSSUBVEC	0x29	/* received */
#define   STATUSLEN 6
#define SVID_XMITSTATUS		0x2A	/* received */
#define SVID_GROUPADDRESS	0x2B	/* received */
#define SVID_FUNCTIONALADDRESS	0x2C	/* received */
#define SVID_ISOLATINGCOUNTS	0x2D	/* received */
#define SVID_NONISOLATINGCOUNTS	0x2E	/* received */
#define SVID_ERROR		0x30	/* received */
#define SVID_LAST		SVID_ERROR

/*
 * LNM Pathtrace Reporting Display Status
 */
#define PATHTRACE_NONE          0x00
#define PATHTRACE_BRIDGE        0x01
#define PATHTRACE_CONFIG        0x02
#define PATHTRACE_STATION       0x03

/*
 * Data Structures for SubVectors
 */
typedef struct _response {
    ushort code;
    uint   dst_class:4;
    uint   src_class:4;
    uchar  mvid;
} response_t;

typedef struct _ucodelevel {
    uchar feature[3];
    uchar ec_level[3];
} ucodelevel_t;

typedef struct _isolating_counts {
    uchar line;
    uchar internal;
    uchar burst;
    uchar ac;
    uchar abort;
    uchar reserved;
} isolating_counts_t;

typedef struct _nonisolating_counts {
    uchar lost_frame;
    uchar congestion;
    uchar frame_copied;
    uchar frequency;
    uchar token;
    uchar reserved;
} nonisolating_counts_t;

#define MAX_MAC_PAK   20

/*
 * Data pointer structure returned by MAC frame parser
 */

typedef struct mac_data_ptrs_ {
    ushort     *beacontype;
    boolean    beacontypeDone;
    uchar      *naun;
    boolean    naunDone;
    ushort     *localring;
    boolean    localringDone;
    ulong      *phys_location;
    boolean    physAssgnLocDone;
    ushort     *soft_err_timer;
    boolean    softerrTimerDone;
    ushort     *enabledclass;
    boolean    enabledclassDone;
    ushort     *allowedpriority;
    boolean    allowedPriDone;
    ushort     *correlator;
    boolean    corrDone;
    uchar      *lastaddress;
    boolean    lastaddrDone;
    ulong      *phys_loc;
    boolean    phyLocDone;
    response_t *response;
    boolean    respDone;
    ushort     *address_mod;
    boolean    addrModDone;
    uchar      *productid;
    boolean    prodIdDone;
    uchar      *ucodelevel;
    boolean    ucodeLvlDone;
    uchar      *forward_data;
    boolean    forDataLenDone;
    int	        forward_len;
    uchar      *stationid;
    boolean    statIdDone;
    uchar      *statussubvec;
    boolean    statSubvDone;
    ushort     *xmitstatus;
    boolean    xmitstatDone;
    ulong      *group;
    boolean    groupDone;
    ulong      *functional;
    boolean    funcDone;
    isolating_counts_t *isolating;
    boolean    isoDone;
    nonisolating_counts_t *noniso;
    boolean    nonisoDone;
    ushort *error;
    boolean    errDone;
} mac_data_ptrs;

/*
 * Bit Masks
 */
#define SV_NONE			0x00000000
#define SV_BEACONTYPE		0x00000001
#define SV_NAUN			0x00000002
#define SV_LOCALRING		0x00000004
#define SV_ASSGN_PHYS_LOC	0x00000008
#define SV_SOFTERRTIMER		0x00000010
#define SV_ENABLEDCLASS		0x00000020
#define SV_ALLOWEDPRIORITY	0x00000040
#define SV_CORRELATOR		0x00000080
#define SV_LASTADDRESS		0x00000100
#define SV_PHYS_LOC		0x00000200
#define SV_RESPONSE		0x00000400
#define SV_ADDRESS_MOD		0x00000800
#define SV_PRODUCTID		0x00001000
#define SV_UCODELEVEL		0x00002000
#define SV_WRAPDATA		0x00004000
#define SV_FRAMEFORWARD		0x00008000
#define SV_STATIONID		0x00010000
#define SV_STATUSSUBVEC		0x00020000
#define SV_XMITSTATUS		0x00040000
#define SV_GROUPADDRESS		0x00080000
#define SV_FUNCTIONALADDRESS	0x00100000
#define SV_ISOLATINGCOUNTS	0x00200000
#define SV_NONISOLATINGCOUNTS	0x00400000
#define SV_ERROR		0x00800000

#define MVR_RESPONSE 		SV_RESPONSE
#define MVO_RESPONSE 		SV_CORRELATOR | SV_RESPONSE
#define MVR_BEACON		SV_BEACONTYPE | SV_NAUN | SV_PHYS_LOC
#define MVO_BEACON		SV_CORRELATOR
#define MVR_CLAIM		SV_NAUN
#define MVO_CLAIM		SV_PHYS_LOC
#define MVR_RINGPURGE		SV_NAUN
#define MVO_RINGPURGE		SV_PHYS_LOC
#define MVR_AMP			SV_NAUN
#define MVO_AMP			SV_PHYS_LOC
#define MVR_SMP			SV_NAUN
#define MVO_SMP			SV_PHYS_LOC
#define MVR_DUPTEST		SV_NONE
#define MVO_DUPTEST		SV_NONE
#define MVR_LOBETEST		SV_WRAPDATA
#define MVO_LOBETEST		SV_WRAPDATA
#define MVR_TRANSMITFWD		SV_FRAMEFORWARD
#define MVO_TRANSMITFWD		SV_FRAMEFORWARD
#define MVR_REMOVE		SV_NONE
#define MVO_REMOVE		SV_NONE
#define MVR_CHANGEPARAM		SV_NONE
#define MVO_CHANGEPARAM		SV_LOCALRING | SV_ASSGN_PHYS_LOC | \
				SV_SOFTERRTIMER | SV_ENABLEDCLASS | \
				SV_ALLOWEDPRIORITY | SV_CORRELATOR
#define MVR_INITIALIZE		SV_NONE
#define MVO_INITIALIZE		SV_LOCALRING | SV_ASSGN_PHYS_LOC | \
				SV_SOFTERRTIMER | SV_CORRELATOR
#define MVR_REQADDRESS		SV_NONE
#define MVO_REQADDRESS		SV_CORRELATOR
#define MVR_REQSTATE		SV_NONE
#define MVO_REQSTATE		SV_CORRELATOR
#define MVR_REQATTACH		SV_NONE	
#define MVO_REQATTACH		SV_CORRELATOR
#define MVR_REQINIT		SV_NAUN | SV_PRODUCTID | SV_UCODELEVEL
#define MVO_REQINIT		SV_ADDRESS_MOD
#define MVR_RPTADDRESS		SV_NAUN | SV_CORRELATOR | SV_PHYS_LOC | \
				SV_GROUPADDRESS | SV_FUNCTIONALADDRESS
#define MVO_RPTADDRESS		SV_ADDRESS_MOD
#define MVR_RPTSTATE		SV_CORRELATOR | SV_UCODELEVEL | \
				SV_STATUSSUBVEC
#define MVO_RPTSTATE		SV_STATIONID
#define MVR_RPTATTACH		SV_ENABLEDCLASS | SV_ALLOWEDPRIORITY | \
				SV_CORRELATOR | SV_PRODUCTID | \
				SV_FUNCTIONALADDRESS
#define MVO_RPTATTACH		SV_NONE
#define MVR_RPTNEWMON		SV_NAUN | SV_PHYS_LOC | SV_PRODUCTID
#define MVO_RPTNEWMON		SV_NONE
#define MVR_RPTNAUN		SV_NAUN | SV_PHYS_LOC
#define MVO_RPTNAUN		SV_NONE
#define MVR_NNINCOMP		SV_LASTADDRESS
#define MVO_NNINCOMP		SV_NONE
#define MVR_RPTMONERR		SV_NAUN | SV_PHYS_LOC | SV_ERROR
#define MVO_RPTMONERR		SV_NONE
#define MVR_SOFTERR		SV_NAUN | SV_PHYS_LOC | SV_ISOLATINGCOUNTS | \
				SV_NONISOLATINGCOUNTS
#define MVO_SOFTERR		SV_NONE
#define MVR_RPTTXMTFWD		SV_XMITSTATUS
#define MVO_RPTTXMTFWD		SV_NONE

/**********************************************************************
 *
 *                     Local data structures
 *
 **********************************************************************/

#define MAXLINKS 4
#define LMKEYLEN 8

/*
 * Station Specific information
 */
typedef struct isolating_counts_l_ {
    long line;
    long internal;
    long burst;
    long ac;
    long abort;
} isolating_counts_l;

typedef struct nonisolating_counts_l_ {
    uchar lost_frame;
    uchar congestion;
    uchar frame_copied;
    uchar frequency;
    uchar token;
} nonisolating_counts_l;

typedef struct station_info_ {
    struct station_info_ *next;
    struct station_info_ *nexterror;
    hwidbtype *idb;
    uchar  address[IEEEBYTES];		/* ?? */
    sys_timestamp temp;			/* created by lanmgr search request */
    int removing;			/* in the process of removing */
    int beaconing;			/* testing because of beaconing on ring */

    boolean error_xmt;
    uchar  naun[IEEEBYTES];		/* 02 */
    ushort enabled_functions;		/* 06 */
    ushort allowed_priority;		/* 07 */
    ulong  phys_location;		/* 0b */
    ushort address_modifier;		/* 21 */
    uchar  product_id[IDLEN];		/* 22 */
    uchar  ucode[UCODELEN];		/* 23 */
    uchar  station_id[IEEEBYTES];	/* 28 */
    uchar  status_vector[STATUSLEN];	/* 29 */
    ushort transmit_status;		/* 2a */
    ulong  group;			/* 2b */
    ulong  functional;			/* 2c */
    ushort attach_status;		/* rps received ACK or NACK */

    /*
     * RPS Dialogue
     */
    ushort rps_correlator;		/* 09 */
    ushort rps_response;
    ushort rps_count;
    sys_timestamp rps_time;
    paktype *rps_pak;

    /*
     * CRS Dialogue
     */
    crs_data *request_data;		/* lan mgr data for relaying response */
    ushort crs_correlator;		/* 09 */
    ushort crs_response;
    ushort crs_count;
    sys_timestamp crs_time;
    paktype *crs_pak;

    /*
     * REM Dialogue
     */
    isolating_counts_l isolating;	/* 2d */
    int   weight;			/* sum of isolating errors */
    int   error_state;			/* what kind of errors do we have? */
    int   decay_count;			/* # times indicated errs decaying */
    int   congestion_count;
    sys_timestamp congestion_time;
} station_info;

/*
 * State flags.
 */
#define INFO_REMOVING    0x01
#define INFO_BEACONING   0x02

/*
 * REM error state flags.  The first two are defined by IBM.  They were
 * discovered by awtching the output of an IBM bridge.
 */
#define ERR_ME_WEIGHT    0x80
#define ERR_ME_PREWEIGHT 0x40
#define ERR_ME_CONGEST   0x20
#define ERR_UPSTREAM     0x10
#define ERR_DOWNSTREAM   0x08
#define ERR_EXIST	 0x01

#define ERR_LANMGR       0xC0		/* Lan Manager is intersted in these */
#define ERR_ME           0xE0		/* I am in error state */
#define ERR_FAULT        0xD8		/* I am in a fault domain */


/*
 * Interface related information
 */
struct ring_info_ {
    struct ring_info_	*next;
    int			ringno;
    uchar 		amp[IEEEBYTES];
    boolean		watch_poll;
    int			beaconing;
    station_info	*beacon_info;
    station_info	*beacon_naun;
    uchar	        last_beacon_address[IEEEBYTES];
    uchar	        last_beacon_naun[IEEEBYTES];
    sys_timestamp	last_beacon_time;
    ushort		last_beacon_type;
    uchar	        last_purge_address[IEEEBYTES];
    sys_timestamp	last_purge_time;
    uchar	        last_claim_address[IEEEBYTES];
    sys_timestamp	last_claim_time;

    /*
     * LRM
     */

    /*
     * RPS
     */
    ulong		location_count;
    ushort 		softerr_timer;
    boolean		rps_state;

    /*
     * REM
     */
    int			pass;				/* count passes to dec at right time */
    uchar 		last_nnin_addr[IEEEBYTES];	/* 0a */
    sys_timestamp	last_nnin_time;
    uchar 	        last_monerr_address[IEEEBYTES];/* 30 */
    sys_timestamp	last_monerr_time;
    ushort		last_monerr_code;
    ushort		rem_state;			/* normal, beaconing */
    ushort		notif_flags;			/* set by LM */
    ushort		ring_inten_mask;		/* set by LM */
    ushort		auto_inten_mask;		/* set by LM */
    ulong		decrement;
    int 		num_entries;
    station_info	  *station_errors;		/* linked list of stations with errors */
    nonisolating_counts_l noniso;
    int                   congestion_thresh;		/* non-congestion count thresholds */
    boolean		new_amp;
    boolean		num_mismatch_allowed;
};

#define BCN_NONE	  0
#define BCN_BEACONING	  1
#define BCN_PERMANENT	  2
#define BCN_POSTMORTEM	  10
#define BCN_POSTMORTEM1	  11
#define BCN_POSTMORTEM2	  12
#define BCN_POSTMORTEM3	  13

#define RPS_STATE_OFF		0
#define RPS_STATE_DETERMINE	1
#define RPS_STATE_PASSIVE	2
#define RPS_STATE_ACTIVE	3

#define RPS_MAX_TRIES     3
#define CRS_MAX_TRIES     3

#define CRS_REMOVE_TRIES 3


#define RPS_SOFTERR_DEFAULT   2 * ONESEC/10      /* 2 seconds, units are 10ms */

#define REM_WORK_INTERVAL      (5 * ONESEC)
#define REM_DECREMENT_INTERVAL (30 * ONESEC)
#define REM_DECREMENT           1

#define REM_THRESH_PREWEIGHT   0x40
#define REM_THRESH_WEIGHT      0x80
#define REM_WEIGHT_LIMIT       0x80
#define REM_WEIGHT_DECREASING  0x7E

/*
 * The following were determined by looking at the values sent by a
 * bridge when reporting non-isolating error counts.
 */
#define REM_NONISO_LIMIT       0x7f
#define REM_AUTOIN_DEF	       0xffff
#define REM_RINGIN_DEF	       0xffff
#define REM_NOTIF_DEF	       0xfe00





#define REM_CONGEST_INIT       5
#define REM_CONGEST_TIME       (5 * ONESEC)

#define REM_SOFTERR_INIT       5		/* 5 errors in 5 seconds */
#define REM_SOFTERR_TIME       (5 * ONESEC)

#define IBMNM_TEMP_RETAIN   (60 * ONESEC)

/*
 * Per Bridge Lan Manager related attributes
 */
#define REPORT_LRM   0x80000000
#define REPORT_LBS   0x00000100
#define REPORT_CRS   0x00000010
#define REPORT_REM   0x00000008
#define REPORT_RPS   0x00000002
#define REPORT_AVAIL 0x8000011a
#define REPORT_ALL   0x8000011a

#define LRM_CONTROLLING 1
#define LRM_OTHER       2
#define LRM_ALL         3

#define LM_DEF_ALT_LMS  0
#define LM_MAX_ALT_LMS  MAXLINKS-1
/*
 * The following were determined by looking at the values sent by a
 * bridge when reporting lbs parameters
 */
#define LM_DEF_ALT_LMS      0
#define LM_DEF_LOSS_THRESH 10
#define LM_DEF_NOTIF_INT    0
#define LM_DEF_CALC_INT     1

typedef struct bridge_link_info_ {
    llctype    *cookie;
    boolean    controlling;
    uchar      address[IEEEBYTES];	/* who is the manager */
    hwidbtype   *idb;			/* on which interface */
    int        correlator;
    char       key[LMKEYLEN];
    char       newkey[LMKEYLEN];
    ulong      reporting_idb1;
    ulong      reporting_idb2;
} bridge_link_info;

typedef struct bridge_info_ {
    struct bridge_info_ *next;
    /*
     * Definition
     */
    hwidbtype           *idb1;
    hwidbtype           *idb2;

    /*
     * General
     */
    int			announce_count;
    sys_timestamp	announce_time;

    /*
     * LRM
     */
    int                 num_alt_lanmgrs;
    bridge_link_info    links[MAXLINKS];

    /*
     * LBS
     */
    ulong		calculation_interval;
    ulong		notification_interval;
    sys_timestamp	notification_time;
    int			frame_lost_thresh;
} bridge_info;

typedef enum {
    lnmst_undefined = 0,          /* no reporting link active          */
    lnmst_disconnect,             /* Awaiting Set Reporting Point      */
    lnmst_circuit_pending,        /* Test Response returned to LNM PC  */
    lnmst_circuit_established,    /* Set Reporting Point received      */
    lnmst_contact_pending,        /* Awaiting UA for SABME             */
    lnmst_connected,              /* LANMGR Accepted sent              */
    lnmst_max = lnmst_connected
} lnmst_t;

typedef enum {
    clnmst_open_wait,
    clnmst_opened,
    clnmst_conn_wait,
    clnmst_stop_flow,
    clnmst_connected,
    clnmst_disc_wait,
    clnmst_disc_ind_pend,
    clnmst_disconnect,
    clnmst_close_wait,
    clnmst_closed
} clsi2lnm_state_t;

typedef struct _lnm_clsi_t {
    struct _lnm_clsi_t *next;
    hword slot_number;          /* Slot number */
    hword port_number;          /* Interface number */
    hword port_type;		/* port type */
    hword sub_number;           /* sub interface number */
    idbtype *swidb;             /* sw idb */
    idbtype *vswidb;            /* vdlc sw idb */
    uchar *port_id;             /* common to tr/vdlc port */
    word  correlator;           /* tr/vdlc port correlator */
    hword ring_number;          /* source ring */
    hword ring_group;           /* target-virtual ring */
    hword bridge_number;	/* bridge number */
    hword trans_bridge_group;   /* for ethernet ... transparent bridge group */
    word  mtu_size;	        /* MTU  */
    uchar enabled:1;            /* true if port enable is successful */
    word  p_sap_id; 		/* This is the LNM SAP ID for the entire i/f */
    uchar sap_enabled:1;        /* true if actsap is successful */
    uint p_cep_id;
    queuetype lnm_sapQ;
} lnm_clsi_t;

extern queuetype lnm_clsiQ;

typedef struct _lnm_sap_t {
    struct _lnm_sap_t *next;
    idbtype    *swidb;             /* sw idb */
    uchar 	sap;
    word  	mtu_size;	        /* MTU  */
    word  	p_sap_id;
    uchar enabled:1;           /* true if actsap is successful */
} lnm_sap_t;

typedef struct _lnm_dlc_id {
    uchar 		client_mac_address[IEEEBYTES];
    uchar 		server_mac_address[IEEEBYTES];
    uchar 		client_link_sap;
    uchar 		server_link_sap;
} lnm_dlc_id_t;
    
typedef struct _lnm_hash {
    struct _lnm_hash     *id;
    struct _lnm_hash     *next;
    union {
      ushort             print_index;
      struct {
	uchar            hash; 
	uchar            list; /* index into the hash list */
      }entry;
    } lnm_index;
    lnmst_t           lnm_state;    /* lnm reporting link state */
    /*
     * clsi interface context
     */
    paktype          *cs_pak;
    uchar            *cs_result;     /* stored context during conn setup */
    boolean           conn_pend;
    uint              linkid;
    uint              u_cep_id;     
    uint              p_cep_id;
    lnm_dlc_id_t      dlc_id;
    clsi2lnm_state_t  clsi_state;
    boolean           my_ckt;
    lnm_clsi_t        *lnm_port_id_dgra;  /* dispatcher access pointer */
    uint 	      our_port_id;
    ulong             fc_llc_flowlevel;
    paktype           *pak;
    TCLSIMsg          *clsi_msg;
    ushort            initial_correlator;
    ushort            initial_num_alt_lnm;
    uchar             *payload_ptr;
    ushort            payload_len;
    uchar             local_rif_len;
    uchar             local_rif[18];   /* SRB_MAXRIF */
    uchar             remote_rif_len;
    uchar             remote_rif[18];
} lnm_t;


/* ################################################################
 * 		ERROR MESSAGE DEFINITIONS
 * ################################################################
 */
#include "logger.h"			/* Required for next include */
#include "../ibm/msg_lanmgr.c"		/* Not a typo, see logger.h */


/**********************************************************************
 *
 *                          Externs
 *
 **********************************************************************/
extern int     lanmgr_pid;
extern boolean lanmgr_running;
extern sys_timestamp lanmgr_timer;


/**********************************************************************
 *
 *                          Inlines
 *
 **********************************************************************/
static inline void start_lanmgr_timer (void)
{
    if (!TIMER_RUNNING(lanmgr_timer))
	TIMER_START(lanmgr_timer, ONESEC);
}

static inline void set_lanmgr_timer (sys_timestamp time)
{
    lanmgr_timer = TIMER_SOONEST(lanmgr_timer, time);
}

static inline void bump_lanmgr_timer (void)
{
    TIMER_UPDATE(lanmgr_timer, ONESEC);
}

static inline void stop_lanmgr_timer (void)
{
    TIMER_STOP(lanmgr_timer);
}


/**********************************************************************
 *
 *                          Prototypes
 *
 **********************************************************************/
extern boolean lanmgr_BLOCK(void);
extern boolean lanmgr_background(void);
extern boolean parse_mac_subvectors(subvector_t *, ulong, uchar, mac_data_ptrs *, int *);
extern boolean rem_background(ring_info *);
extern char *mvid_to_str(int);
extern forktype lanmgr_process(void);
extern hwidbtype *ring_to_idb(ring_info *);
extern hwidbtype *address_to_idb(uchar *);
extern paktype *setup_macpacket(int, station_info *, int);
extern paktype *setup_macreply(paktype *, uchar, subvector_t **);
#ifdef NOBODY_CALLS_ME
extern ring_info *address_to_ring(uchar *);
#endif /* NOBODY_CALLS_ME */
extern ring_info *idb_to_ring(hwidbtype *);
extern ring_info *lanmgr_create_ring(hwidbtype *);
extern ring_info *ringno_to_ring(int);
extern station_info *find_or_build_station(uchar *, hwidbtype *, boolean);
extern station_info *find_station(uchar *, hwidbtype *);
extern station_info *newrold_station(paktype *);
extern void crs_init(ring_info *);
extern void crs_input(major_vector *, station_info *, hwidbtype *);
extern void display_ring_info(ring_info *);
extern void free_crs_pak(station_info *);
extern void free_rps_pak(station_info *);
extern void lanmgr_destroy_ring(hwidbtype *);
extern void lanmgr_enqueue(paktype *);
extern void lanmgr_init(subsystype *);
extern void lanmgr_input(void);
extern void lanmgr_ringstat(hwidbtype *, ushort);
extern void lanmgr_start(void);
extern void lnm_set_srb_params(hwidbtype *idb, boolean sense);
extern lnm_t *lnm_pre_proc_clsi(TCLSIMsg* clsi_msg);
extern lnm_t *lnm_proc_clsi(TCLSIMsg* clsi_msg, lnm_clsi_t *port_id);
extern void lnm_send_req_open_stn(lnm_t *lnm_ptr);
extern void lnm_send_close_stn(lnm_t *lnm_ptr);
extern void lnm_send_connect_req(lnm_t *lnm_ptr);
extern void lnm_send_signalstn_req(lnm_t *lnm_ptr);
extern boolean lnm_is_bad_clsi(TCLSIMsg* clsi_msg);
extern boolean lnm_is_context_valid(TCLSIMsg *clsi_msg);
extern boolean lnm_is_init_clsi_op(ushort primitive);
extern void lnm_init_clsi_op_proc(TCLSIMsg* clsi_msg);
extern lnm_clsi_t *lnm_get_clsi(hword slot_number, hword port_number, hword port_type);
extern lnm_clsi_t *lnm_get_clsi_swidb(idbtype *swidb);
extern lnm_clsi_t *lnm_get_clsi_vdlc(word correlator);
extern void lnm_clsi_cleanup(TCLSIMsg *clsi_msg);
extern lnm_t *ucepid2lnm(uint u_cep_id);
extern lnm_clsi_t *lnm_usapid2dgra(uint u_sap_id);
extern void lnm_get_rif_from_clsi(TCLSIMsg *clsi_msg, uchar *new_rif);
extern void lnm_proc_disc(lnm_t *lnm_ptr);
extern void lnm_proc_openreq_cnf(lnm_t *lnm_ptr, boolean status);
extern lnm_t *lnm_add_entry(lnm_dlc_id_t *lnm_hash_dlc_id, lnm_clsi_t *port_id,
			                                           uchar *rif);
extern void lnm_init_fields(lnm_t *lnm_ptr);
extern void lnm_delete_entry(lnm_t *lnm_ptr);
#ifdef NOBODY_CALLS_ME
extern uint calc_lnm_hash(uchar *string);
#endif /* NOBODY_CALLS_ME */
extern char *lnm_id(lnm_t *lnm_ptr, char *str);
extern void lnm_enable_port(CLSPortID *clsPortID);
extern void lnm_disable_port(CLSPortID *clsPortID);
extern void ldisp_to_lfsm(int msg_type, TCLSIMsg *clsi_msg);
extern void ldisp_to_init(int msg_type);
extern void lnm_proc_info(TCLSIMsg *clsimsg, lnm_t *lnm_ptr);
extern void rem_init(ring_info *);
extern void rem_input(major_vector *, station_info *, hwidbtype *);
extern void rem_purge_softerr(ring_info *);
extern void resend_crs_pak(station_info *);
extern void resend_rps_pak(station_info *);
extern void ringstation_input(major_vector *, station_info *, hwidbtype *);
extern void rps_init(ring_info *);
extern void rps_input(major_vector *, station_info *, hwidbtype *);
extern void rps_startup(hwidbtype *, int);
extern boolean send_crs_general(station_info *, uchar, int);
extern boolean send_crs_remove(station_info *);
extern boolean send_crs_set_sta(station_info *, ushort *, ulong  *,
				ushort *, ulong  *, ushort *);
extern void watch_ring_poll(hwidbtype *, boolean);
extern void lanmgr_terminate(void);
extern void lanmgr_restart(void);

/*
 * lanmgr_chain.c
 */
extern void lanmgr_parser_init(subsystype *subsys);

