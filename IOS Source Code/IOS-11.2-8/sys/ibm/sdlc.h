/* $Id: sdlc.h,v 3.5.12.4 1996/06/27 09:31:04 fox Exp $
 * $Source: /release/112/cvs/Xsys/ibm/sdlc.h,v $
 *------------------------------------------------------------------
 * sdlc.h -- Serial Data Link Control
 * 
 * August 1991, Joel P. Bion
 * from lapb.h, Greg Satz
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sdlc.h,v $
 * Revision 3.5.12.4  1996/06/27  09:31:04  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.5.12.3  1996/05/23  06:42:42  pmorton
 * CSCdi58502:  Fix for CSCdi42369 broke FRAD image
 * Branch: California_branch
 * Use 'debug sdlc qllc' instead of 'debug qllc sdlc'
 *
 * Revision 3.5.12.2  1996/05/22  05:00:07  pmorton
 * CSCdi42369:  sdlc-qllc conversion, no auto-reconnect
 * Branch: California_branch
 * 1. When qllc link state changes propagate to SDLC-QLLC conversion (SNX)
 * 2. Add 'debug qllc sdlc' to track this
 * 3. Correct SNX state names
 *
 * Revision 3.5.12.1  1996/05/17  11:21:15  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.2  1996/05/02  20:24:27  pmorton
 * CSCdi42678:  Router crashed after removing SDLC address
 * Branch: IbuMod_Calif_branch
 * Mark sdlc data block for deletion (zombie).  Don't remove
 * till CLS issues CLOSE Station request.
 *
 * Revision 3.5.2.1  1996/04/11  14:37:56  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5  1996/02/13  21:27:19  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.4  1995/12/31  18:17:27  fbordona
 * CSCdi46267:  DLSw sdlc secondary multidrop PUs do not connect.
 *
 * Revision 3.3  1995/11/17  09:23:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:35:54  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:45:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/10/19  08:09:32  bchan
 * CSCdi27730:  SNA Fragmentation break if no buffers available
 *
 * Revision 2.10  1995/10/04  17:17:38  dignacio
 * CSCdi36250:  SDLC Test Frame responds with twice as many sent.
 *
 * Revision 2.9  1995/08/07  05:35:22  richl
 * CSCdi38315:  makefile rework
 *              move utility routines to libibm_util.a
 *
 * Revision 2.8  1995/08/01  02:32:08  vrawat
 * CSCdi37993:  fix stun problems found at customer site
 *
 * Revision 2.7  1995/07/31  04:12:52  szdravko
 * CSCdi37829:  Two sdlc mib fields not maintained
 *
 * Revision 2.6  1995/07/28  22:05:26  vrawat
 * CSCdi36957:  Aborted SDLC STUN frames transmitted across local-ack
 * connection
 *
 * Revision 2.5  1995/07/24  13:07:10  fbordona
 * CSCdi34580:  DLS show circuit displays HALT PENDING NOACK after
 *              successful disc
 *
 * Revision 2.4  1995/07/06  01:32:35  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.3  1995/06/28 18:44:50  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *
 * Revision 2.2  1995/06/28  09:25:50  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:47:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SDLC_H__
#define __SDLC_H__

#include "../cls/dlc_public.h"

#define ISCLSSDLC(idb)							\
    (idb &&								\
     (idb->enctype == ET_SDLC))

#define SDLC_NONE	 	0
#define SDLC_PRIMARY		1
#define SDLC_SECONDARY		2
#define SDLC_PRIM_XID_POLL	3

#define ISPSDLC(idb) ((idb->enctype == ET_SDLCP) || 		\
		      ((idb->enctype == ET_STUN) &&		\
		       (idb->stun_sdlc_encap == ET_SDLCP)) || 	\
		      ((idb->enctype == ET_SDLC) &&		\
		       (idb->sdlc_role == SDLC_PRIMARY)))
#define ISSSDLC(idb) ((idb->enctype == ET_SDLCS) || 		\
		      ((idb->enctype == ET_STUN) && 		\
		       (idb->stun_sdlc_encap == ET_SDLCS)) ||	\
		      ((idb->enctype == ET_SDLC) &&		\
		       (idb->sdlc_role == SDLC_SECONDARY)))

typedef enum {
    SNX_DISCONNECT,			/* nothing going on */
    SNX_DW_SDLC,			/* QLLC down, disconnecting SDLC */
    SNX_AW_QLLC,			/* SDLC UP, awaiting QLLC up */
    SNX_AW_SDLC,			/* Awaiting SDLC up */
    SNX_CONNECT,			/* Both up */
} snx_state_t;

/*
 * sdlc_default values
 */
#define NO_SDLC    0x0000
#define FRAS_SDLC  0x0001
#define DLSW_SDLC  0x0002

/*
 * Structure of a frame header
 */

typedef struct sdlc_frametype_ {
    uchar address;			/* address field */
    uchar control;			/* format field */
} sdlc_frametype;

#define SDLCHEADERBYTES sizeof(sdlc_frametype)
#define DEFAULT_LARGEST_SDLC_FRAME	272
#define	SDLCTYPEBYTES	sizeof(ushort)
#define	SDLC_FRMR_DATA	3
#define sdlcdst	e.encaps[ENCAPWORDS-1]
#define msdlcdst e.encaps[ENCAPWORDS-2]
#define msdlctype e.encaps[ENCAPWORDS-1]
#define sdlcfragment e.encaps[ENCAPWORDS]

#define	SDLC_DEFAULT_T1	(3 * ONESEC)
#define SDLC_DEFAULT_N1(idb) sdlc_default_n1(idb)
#define	SDLC_DEFAULT_N2	20
#define SDLC_DEFAULT_XID0_N2 3 /* number of XID0's sent to CLS for prim 2.0 */
                               /* before restarting with XID NULL */
#define	SDLC_DEFAULT_K	(SDLC_MODULO - 1)
#define SDLC_DEFAULT_POLL_WAIT (40 * ONESEC)
#define	SDLC_DEFAULT_HOLDQ      200	
#define SDLC_MODULO	8
#define SDLC_DEFAULT_SLF       (265)
#define SDLC_XID_SIZE          (4)

#define SDLC_DEFAULT_RNRLIMIT 0         /* default is infinite, no timer*/
#define SDLC_MAX_RNRLIMIT     30        /* minutes             */  

#define SDLC_TXQ_HIWATER 15		/* 15% of sdlc_holdq 	*/
#define SDLC_CLS_TXQ_HIWATER 60		/* 60% of sdlc_holdq 	*/
#define SDLC_TXQ_LOWATER 5		/* 5% of sdlc_holdq 	*/
#define SDLC_CLS_FLOW_IND_DAMP 5	/* damp zone -scalar- 	*/

#define SDLC_DEFAULT_SLOW_POLL  (10 * ONESEC)
#define SDLC_DEFAULT_POLL_PAUSE_TIMER 10
#define SDLC_DEFAULT_POLL_LIMIT_VALUE 1
#define SDLC_MAX_ADDRESS (0xFF)
#define SDLC_MIN_ADDRESS (0x00)

#define SDLC_BCAST_ADDR 0xFF  /* SDLC broadcast address */

#define SDLC_PRCMD 1
#define SDLC_PRRSP 0

#define DEFAULT_TEST_ITERATIONS 10
#define MAX_TEST_STRING 80

/*
 * Address field
 */

#define	SDLC_ISIFRAME(frame) (((frame)->control & SDLC_SFRAME) == SDLC_IFRAME)
#define	SDLC_ISSFRAME(frame) (((frame)->control & SDLC_UFRAME) == SDLC_SFRAME)
#define	SDLC_ISUFRAME(frame) (((frame)->control & SDLC_UFRAME) == SDLC_UFRAME)

/*
 * Constants to Flip SDLC Address Echo Bit
 */
#define SDLC_ECHO_ON  0x80
#define SDLC_ECHO_OFF 0x7f

/*
 * Control Field.
 */

#define	SDLC_NOFRAME	0xff
#define SDLC_IFRAME	0x00
#define SDLC_IMASK      0x01
#define SDLC_SFRAME	0x01
#define SDLC_SMASK      0x03
# define SDLC_RR	0x01
# define SDLC_RNR	0x05
# define SDLC_REJ	0x09
#define SDLC_UFRAME	0x03
# define SDLC_DM	0x0f
# define SDLC_SABM	0x2f
# define SDLC_SNRM	0x83
# define SDLC_SIM	0x07
# define SDLC_XID	0xaf
# define SDLC_TEST	0xe3
# define SDLC_CFGR	0xc7
# define SDLC_DISC	0x43
# define SDLC_UA	0x63
# define SDLC_RIM	0x07
# define SDLC_FRMR	0x87
# define SDLC_RD	0x43
# define SDLC_BCN	0xef
# define SDLC_UI        0x0c  
#define SDLC_UPOLL      0x23
#define SDLC_PF		0x10

/*
 * FRMR reason bits
 */

/* Invalid or non-implemented command */
#define SDLC_FRMR_W	0x01
/* Prohibited information field received */
#define SDLC_FRMR_X	0x02
/* Buffer overrun (information field is too long) */
#define SDLC_FRMR_Y	0x04
/* Received N(r) count is invalid */
#define SDLC_FRMR_Z	0x08

/*
 * Protocol states. Some are used only by SDLC, and are marked as such
 */

typedef enum _sdlc_state_t {
    SDLC_DISCONNECT=0,
    SDLC_DISCPEND,  
    SDLC_SNRMSEEN,
    SDLC_DISCSENT,
    SDLC_ERROR,
    SDLC_XIDSENT,
    SDLC_XIDSTOP,
    SDLC_SNRMSENT,
    SDLC_CONNECT,
    SDLC_THEMBUSY,
    SDLC_USBUSY,
    SDLC_BOTHBUSY
} sdlc_state_t;

typedef enum _cls_state_t {
    CLS_STN_CLOSED = 0,
    CLS_DISC_IND_SENT,
    CLS_DISCCNF_PEND,
    CLS_ROSCNF_PEND,
    CLS_CLOSING,
    CLS_STN_OPENED,
    CLS_CONNECT_REQ_PEND,
    CLS_FULL_XID_PEND,
    CLS_CONNECTED_IND_PEND,
    CLS_CONNECT_RSP_PEND,
    CLS_IN_SESSION
} cls_state_t;

/*
 * Retrieve state information in "structured" manner...
 */
#define IS_SDLC_CONNECT(sdb) (sdb && sdb->sdlc_state >= SDLC_CONNECT)

/*
 * Constant for SDLLC
 */
#define SDLLC_DEFAULT_TLF       (RC_LF516)

/*
 * States for SDLLC
 *
 * SDLC Primary connection order:
 *
 *   DISCONNECT -> AW_SDLC_PRI -> AW_NET -> CONNECT
 *
 * SDLC Secondary connection order:
 *
 *   DISCONNECT -> AW_NET -> AW_SDLC_SEC -> CONNECT
 *
 * Note that the order of state definitions is important.  >= AW_NET
 * implies there maybe somekind of network thing that needs to be
 * torn down.  >= DW_NET indicates something needs to be torn down
 * when the network side async disconnects.
 */

typedef enum _sdllc_state_t {
    SDLLC_DISCONNECT = 0,               /* completely connected */
    SDLLC_DW_SDLC,                      /* waiting for sdlc to disconnect - not used */
    SDLLC_AW_SDLC_PRI,                  /* awaiting for primary sdlc to come up */
    SDLLC_DW_NET,                       /* waiting for network side to get torn down */
    SDLLC_AW_NET,                       /* waiting for net media, TR or NET */
    SDLLC_AW_SDLC_SEC,                  /* waiting for secondary sdlc to come up */
    SDLLC_CONNECT                       /* fully connected, data flows */
} sdllc_state_t;


/*
 * Fail Cause for SDLC MIB
 */
typedef enum _sdlc_last_fail_cause_t {
    UNDEFINED,    
    RX_FRMR,
    TX_FRMR,
    NO_RESPONSE,
    PROTOCOL_ERR,
    NO_ACTIVITY,
    RNR_LIMIT,      
    RETRIES_EXPIRED 
} sdlc_last_fail_cause_t;

/*
 * SDLC Initiation control.
 *
 * SDLC_EXPLORER_TRYS is the number of explorers this router will
 * generate during this (sdlc_periodic) interval before stopping.
 * TEST is only sent if the SDLLC state is AW_NET, or if the 
 * CLS_STATE is ??
 *
 * SDLC_THROTTLE_INTERVAL is the time waited before sending a
 * TEST again.
 *
 * Note that if an interface comes up, we don't strictly obey the
 * interval but will do so for the interface that came up.  Each
 * invocation of sdlc_initiate is told how many it is allowed to
 * fire up.
 */


#define SDLC_THROTTLE_INTERVAL (10*ONESEC)
#define SDLC_INIT_COUNT   10
#define SDLC_SEC_LINKUP_DEFAULT     (2 * 60 * ONESEC )
 /*
  * Approx. two minutes from the point we the last SNRM to the point 
  * we do not hear any snrm after we bring up the connection on the
  * network side
  */



/***********************************************************************
 *
 *                      SDLC Flags word in idbtype
 *
 ***********************************************************************/
#define SDLC_FLAG_NOFRMR        0x01            /* This side doesn't do FRMRs */
#define SDLC_FLAG_DCE           0x02            /* sdlc dce interface      */
#define SDLC_FLAG_NRM_TRNS      0x04            /* no xmit optimization    */
#define SDLC_FLAG_TWS_FULL      0x08            /* TWS DATMODE=FULL        */
#define SDLC_FLAG_VIRTUAL_MULTIDROP   0x10      /* enable sdlc broadcast   */


typedef enum sdlc_oqueue_status_ {
      SDLC_OQUEUE_EMPTY,
      SDLC_OQUEUE_LOW,
      SDLC_OQUEUE_NORMAL,
      SDLC_OQUEUE_HIGH,
      SDLC_OQUEUE_FULL
} sdlc_oqueue_status_t;

struct sdlc_data_block_ {
    /*
     * Data structure fields
     */
    int		next;			/* Next active SDLC address in array */
    int 	prev;			/* Prev active SDLC address in array */
    hwidbtype    *sdlc_idb;

    paktype	*inbuf;			/* hold inbound packet */
    paktype	*outbuf;		/* hold outbound packet */
    uint	u_sap_id;		/* user SAP id */
    void	*u_cep_id;		/* user control end point id */
    CepT	fCep;			/* Common CLS DLC data structure */
    boolean	xid_resp;		/* Got XID Command */
    cls_state_t	cls_state;		/* keep track of CLS state */
    uchar	addr_type;		/* address type - SDLC or IEEE */
    boolean     sdlc_fras_ban;          /* used for ban-sdlc */
    boolean	sdlc_dlsw;		/* used by dlsw */
    boolean     sdlc_zombie;		/* sdb should be deleted */
    boolean     sdlc_xid_configured;
    boolean     sdlc_xid_poll;          /* 2.1 multidrop, role primary, upstream doesn't explore */
    boolean     sdlc_xid_passthru;      /* 2.1 multidrop, role prim or sec, upstream explores */
    boolean     sdlc_switched;          /* 2.x, contact PU before xid'ing host side              */
    sys_timestamp sdlc_switched_startup_timer; /* 2.x, time to xid downstream stn                */
    uchar       sdlc_xid[4];
    ushort      sdlc_xid_count;
    ushort      sdlc_xid0_n2;           /* prim 2.0 : xid0 max retries   */
    int	        sdlc_largest_frame;
    uchar       sdlc_prtnr[IEEEBYTES];
    boolean     sdlc_have_prtnr;
    uchar       sdlc_ssap;
    uchar       sdlc_dsap;
    sdlc_oqueue_status_t curr_cong_level;  /* current txQ cong level      */
    sdlc_oqueue_status_t prev_cong_level;  /* previous txQ cong level     */    
    ushort lowater_lodamp;
    ushort lowater_hidamp;
    ushort hiwater_lodamp;
    ushort hiwater_hidamp;

    /*
     * SDLLC (media-translation) values
     */
    sdllc_state_t sdllc_state;
    llctype    *sdllc_llc;
    boolean    sdllc_lak_enable;
    boolean    sdllc_local_busy;       
    sys_timestamp sdlc_initiation_throttle_time;
    /*
     * Inbound and outbound are used to control whether this station 
     * initiates the connection.
     */
    boolean    sdlc_outbound;
    boolean    sdlc_inbound;	/* Note: inbound option was originally 
				   added to partner command to flag 
				   IBM(Stratus) system 88.  Modify startup
				   sequence because box doesn't handle RIFs
				   D bit properly.  Box wants to learn RIF
				   from our TEST(Final), and doesn't learn
				   it correctly from our TEST(Poll).  
				   Therefore, don't send TEST(Poll).
				   Change parm to general purpose "inbound"
				   connection from "system88".
				 */

    /*
     * STUN specific fields
     */
    boolean    stun_peer_busy;          /* remote sdlc holdq full */
    boolean    stun_local_busy;         /* local sdlc holdq full */
    boolean    stun_init_busy;          /* primary comes up in busy state */
    boolean    stun_addr_echo;          /* deal with FEP-to-FEP echo bit */
    boolean    stun_frame_ok;           /* used by STUN local-ack to decide if the  */
                                        /* I-frame should be sent to the other peer */
    uchar      tg_snrm_addr;            /* Address used on remote SNRM */

    /*
     * SDLC protocol dependent fields
     */
    boolean sdlc_testmode;		/* accept and toss all I-frames */
    boolean sdlc_twoaddrmode;		/* at last minute, change output addr to input */
    boolean sdlc_ackmode;               /* allow address FF to be local ack'ed */
    sys_timestamp sdlc_poll_starve_timer; /* time when we starve (secondary) */
    sdlc_state_t sdlc_state;		/* protocol state */
    uchar sdlc_upstate;			/* protocol state code when up */
    uchar sdlc_poll;			/* remember when we send a poll */
    uchar sdlc_vs;			/* send state variable */
    uchar sdlc_vr;			/* receive state variable */
    uchar sdlc_rvr;			/* remote receive packet count */
    uchar sdlc_seqmap;			/* bitmap of legal NR seqs. */
    uchar poll_seqmap;                  /* bitmap when the poll was sent */
    uchar sdlc_retcnt;			/* retransmission count */
    uchar sdlc_iframeRetCnt;            /* retransmission of same iframes */
    struct paktype_ **sdlc_paks;	/* pointer to array of packets */
    queuetype sdlc_holdq;		/* hold packets sent outside window */
    char *sdlc_buffer;			/* debugging buffer */
    uchar sdlc_xmtptr;			/* sdlc station current ns */
    ulong sdlc_retransmitsFrameCount;   /* Count of iframes retransmitted */
    ulong sdlc_pollsIn;                 /* total number of polls received */
    ulong sdlc_pollsOut;                /* total number of polls sent */
    ulong sdlc_pollRspsOut;             /* total number of poll rsps sent */
    ulong sdlc_pollRspsIn;              /* total number of poll rsps received */
    ulong sdlc_protocolErrs;            /* total number of inactivated links */
    ulong sdlc_iframessent;		/* total IFRAMEs sent */
    ulong sdlc_iframesrcvd;		/* total IFRAMEs received */
    ulong sdlc_rnrssent;		/* total RNRs sent */
    ulong sdlc_rnrsrcvd;		/* total RNRs received */
    ulong sdlc_rejssent;                /* total REJs sent */
    ulong sdlc_rejsrcvd;                /* total REJs received */
    ulong sdlc_snrmssent;		/* total SNRMs sent */
    ulong sdlc_snrmsrcvd;		/* total SNRMs received */
    ulong sdlc_discssent;		/* total DICSs sent */
    ulong sdlc_discsrcvd;		/* total DISCs received */
    ulong sdlc_frmrssent;		/* total FRMRs sent */
    ulong sdlc_frmrsrcvd;		/* total FRMRs received */
    ulong sdlc_dmssent;                 /* total DMs sent */
    ulong sdlc_dmsrcvd;                 /* total DMs received */
    ulong sdlc_uassent;                 /* total UAs sent */
    ulong sdlc_uasrcvd;                 /* total UAs received */
    ulong sdlc_simssent;                /* total SIMS sent */
    ulong sdlc_simsrcvd;                /* total SIMS received */
    ulong sdlc_rimssent;                /* total RIMS sent */
    ulong sdlc_rimsrcvd;                /* total RIMS received */
    ulong sdlc_uiframessent;            /* total UIFRAMES sent */
    ulong sdlc_uiframesrcvd;            /* total UIFRAMES received */
    ulong sdlc_testssent;               /* total TESTS sent */
    ulong sdlc_testsrcvd;               /* total TESTS received */ 
    ulong sdlc_xidssent;                /* total XIDS sent */
    ulong sdlc_xidsrcvd;                /* total XIDS received */ 
    ulong sdlc_octetssent;              /* total OCTETS sent */
    ulong sdlc_octetsrcvd;              /* total OCTETS received */ 
    ulong sdlc_retriesexps;             /* total times link is reset because 
                                           retry count exceeded */

    struct paktype_ *sdlc_frmrpak;	/* frame error packet */
    uchar sdlc_address;			/* Address of SDLC secondary */
    boolean sdlc_ready_for_poll;        /* Is the pu ready for another poll next round */
    ulong sdlc_poll_count;		/* poll count (for poll limit) */
    int sdlc_iframes_since_poll; 	/* # input i-frames since last poll */
    sys_timestamp time_linkup_sent;	/* When was a linkup_req sent? */
    sys_timestamp sdlc_sec_linkup_time; /* records when we fire a CR/SABME
                                         * resets we we get sdlc side connected.
                                         * If slow poll on sdlc side gives up
                                         * after a snrm when the network side is up
                                         * this helps to tear the connection down
                                         */

    /* parameters for the SDLC test feature */
    uchar test_cnt;                      /* 00    - test is OFF
					  * 1-254 - test iterations
					  * FF    - continuous
					  */
    char test_string[MAX_TEST_STRING+1]; /* string to be sent in the TEST frame */
    unsigned sdlctest_sendcnt;           /* Count of SDLC Test Frames sent */
    unsigned sdlctest_rcvcnt;            /* Count of SDLC Test Frames received */

    sdlc_last_fail_cause_t lastFailCause;/* For SDLC MIB */
    sys_timestamp sdlc_lastFailTime;

    sys_timestamp sdlc_rnrLimitTimer;   /* RNR Limit Timer for timing out 
                                         * remote busy conditions.
                                         */
    ulong sdlc_rnrLimitCount;           /* Total times RNR Limit Timer expired.
                                         */
    ulong sdlc_activityTOs;             /* Total times link station has inactivated the
                                         * link as a result of no activity on the link.
                                         */
    uchar sdlc_simRim;                  /* SIM and RIM supported or not.
                                         * Retrieved from XID1, XID2 or XID3.
                                         */                         
    uchar sdlc_xmitRcvCap;              /* Link Station transmit receive capabilities-
                                         * 2-way alternate or 2-way simultaneous. 
                                         * Retrieved from XID1, XID2 or XID3.
                                         */
    uchar sdlc_operName[11];            /* Link Station name from XID3, Control Vector 0x0E,
                                         * type 0xF7.
                                         */
    uchar sdlc_lastFailFRMRInfo[3];      /* FRMR Info: C-field, Ns Nr count and Status. 
                                         */
    uchar sdlc_lastFailCtrlOut;         
    uchar sdlc_lastCtrlOut;      
    uchar sdlc_lastFailCtrlIn;
    uchar sdlc_lastCtrlIn;
    ulong sdlc_lastFailReplyTO;         
    ulong sdlc_lastReplyTO;        /* Number of times no reply received to poll */
    boolean sdlc_firstTxFramesNsSet;   

    /* QLLC/SNX items */
    snx_state_t	snx_state;
    uchar      qllc_prtnr[IEEEBYTES];
    boolean    qllc_have_prtnr;
    boolean    qllc_active;
    qllctype   *qllcp;
    lnxtype    *lnxp;
    queuetype   sdlcOutQ;	/* For fragmentation Q */
};


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

extern boolean snx_running; /* true if qllc-sdlc conversion is initialized */
extern sdlc_qllc_debug;			/* set by 'debug sdlc qllc' */
extern boolean sdlcBLOCK(void);
extern boolean sdlc_ack(hwidbtype *, paktype *, uchar, uchar,
			int, sdlc_data_block *);
extern boolean sdlc_encapsulate(paktype *, int, sdlc_data_block *);
extern boolean sdlc_validcmdframe(sdlc_frametype *);
extern char *sdlc_prframe(hwidbtype *, paktype *, sdlc_data_block *,
			  uchar, uchar);
extern char *sdlc_prstate(sdlc_data_block *, int);
extern int sdlc_default_n1(hwidbtype *);
extern long sdlc_getlink(paktype *);
extern paktype *getsdlc(hwidbtype *, char *);
extern void sdlc_cstate(hwidbtype *);
extern void sdlc_dooutput(hwidbtype *, sdlc_data_block *);
extern void sdlc_encaps(parseinfo *);
extern void sdlc_init(subsystype *);
extern void sdlc_input(hwidbtype *, paktype *);
extern void sdlc_wrap_input(hwidbtype *, paktype *);
extern void sdlc_interface(hwidbtype *);
boolean sdllc_add_vring_interface(hwidbtype *idb, ushort local_ring,
				  uchar bridge_num, ushort target_ring);
extern void sdlc_reset(hwidbtype *, char *, sdlc_data_block *);
extern void sdlc_setup1(hwidbtype *);
extern void sdlc_setup2(hwidbtype *);
extern void sdlc_start(hwidbtype *, paktype *, boolean, boolean, sdlc_data_block *);
extern void sdlc_sendframe(int, boolean, hwidbtype *, paktype *, sdlc_data_block *);
extern void sdlc_holdq(hwidbtype *, sdlc_data_block *, paktype *);
extern void sdllc_output(hwidbtype *, paktype *);
extern void sdlc_settimer(hwidbtype *, int, sdlc_data_block *);
extern void sdlc_set_pause_timer(hwidbtype *, sdlc_data_block *);
extern void sdlc_set_pause_timer_rfp(hwidbtype *, sdlc_data_block *);
extern void sdlc_setstate(hwidbtype *, int, sdlc_data_block *);
extern void sdlc_set_clear_busy(hwidbtype *, unsigned, boolean);
extern void sdllc_setstate(hwidbtype *, sdlc_data_block *, sdllc_state_t, char *);
extern char *sdllc_prstate(sdlc_data_block *sdb, sdllc_state_t state);
extern void sdllc_ind_disconnect(hwidbtype *, sdlc_data_block *);
extern void sdllc_ind_connect(hwidbtype *, sdlc_data_block *);
extern void sdlc_ind_disconnect_snx(hwidbtype *, sdlc_data_block *);
extern void sdlc_ind_connect_snx(hwidbtype *, sdlc_data_block *);
extern uchar *snx_prstate(snx_state_t snx_state);
extern void snx_set_state(sdlc_data_block *sdb, snx_state_t newstate);
extern void sdllc_sec_conn_tear_down(void);
extern int sdllc_initiate_pu (hwidbtype *, sdlc_data_block *);
extern void sdlcSendXidToCLS(sdlc_data_block *, paktype *);
extern boolean IsPrimPu2(sdlc_data_block *);
extern boolean IsPrimXidPoll(sdlc_data_block *);
extern boolean IsSecPu2(sdlc_data_block *);
extern boolean IsDlswSdb(sdlc_data_block *);
extern boolean IsFrasBanSdb(sdlc_data_block *);
extern void sdlc_config_partner (parseinfo *, hwidbtype *);
extern void sdlc_config_xid (parseinfo *, hwidbtype *);
extern void sdlc_config_saps (parseinfo *, hwidbtype *);
extern void sdlc_config_sdlclf (parseinfo *, hwidbtype *);
extern void sdlc_sendfrag(sdlc_data_block *, paktype *);
extern void sdlc_flow_ctrl_ind(sdlc_data_block *);
extern void sdlc_exec_test_command (parseinfo *);
extern void print_sdlc_test_results (sdlc_data_block *);
extern void SdlcSendThruQ (sdlc_data_block *sdb, paktype *pak);
extern void sdlc_count_input(hwidbtype *idb, paktype *pak);

extern void sdlc_mark_deleted(sdlc_data_block *sdb);
extern boolean is_sdlc_marked_deleted(sdlc_data_block *sdb);
extern void SdlcDeleteClsSdb(sdlc_data_block *sdb);

/*
 * inline functions
 */

/*
 * sdlc_is_multidrop()
 *
 * In the future, this can be changed to check for multidrop in other
 * ways, but for now, take advantage of address chaining of sdb's to
 * determine if multiple sdb's exist for this idb. Could have used
 * sdlc_stn_count(), but this walks through all sdb's (slow).
 */

static inline boolean sdlc_is_multidrop (sdlc_data_block *sdb)
{
    if (sdb->sdlc_address != sdb->sdlc_idb->sdlc_data[sdb->sdlc_address]->next)
        return(TRUE);
    else
        return(FALSE);
}

#endif __SDLC_H__
