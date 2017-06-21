/* $Id: lnx.h,v 3.4.12.3 1996/08/21 13:15:12 rbatz Exp $
 * $Source: /release/112/cvs/Xsys/srt/lnx.h,v $
 *------------------------------------------------------------------
 * lnx.h -- Lan to X.25 translation (LNX) header file
 *
 * January 1993,   Percy P. Khabardar
 * September, 1993 Phil Morton
 * September, 1993 Ramin  Naderi
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lnx.h,v $
 * Revision 3.4.12.3  1996/08/21  13:15:12  rbatz
 * CSCdi62155:  LNX timers not working correctly. Result is TEST
 *              and NULL XID frames sent in the middle of
 *              XID(3) Negotiation.
 * Branch: California_branch
 *
 * Revision 3.4.12.2  1996/05/22  05:00:36  pmorton
 * CSCdi42369:  sdlc-qllc conversion, no auto-reconnect
 * Branch: California_branch
 * 1. When qllc link state changes propagate to SDLC-QLLC conversion (SNX)
 * 2. Add 'debug qllc sdlc' to track this
 * 3. Correct SNX state names
 *
 * Revision 3.4.12.1  1996/05/17  12:13:01  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.2  1996/04/11  14:38:20  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.2.1  1996/04/06  01:05:58  pmorton
 * CSCdi45577:  QLLC w/PVCs regression with QLLC in CLS.
 * Build a map for managing PVCs, so that they can be claimed either by
 * CLS or by RSRB
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4  1996/02/17  16:47:46  rbatz
 * CSCdi44435:  Outbound connection from FEP to QLLC device fails.
 *              Respond to XID Commands with XID Responses.
 *
 * Revision 3.3  1996/01/30  21:57:29  pmorton
 * CSCdi36695: QLLC disards XID packet (assuming that end station will
 * retransmit)
 * Hold the XID packet and send it to remote x.25 station after receiving
 * Call Accept
 *
 * Revision 3.2  1995/11/17  18:54:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/19  08:10:33  bchan
 * CSCdi27730:  SNA Fragmentation break if no buffers available
 *
 * Revision 2.1  1995/06/07  23:00:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if	!defined(__LNX_H__)
#define __LNX_H__

/* Bridge number is always 1 */

#define LNX_BRIDGE_NUMBER    1

/*
 * States for QLLC Conversion
 *
 * QLLC Primary connection order:
 *
 *   DISCONNECT -> AW_QLLC_PRI -> AW_NET -> CONNECT
 *
 * SDLC Secondary connection order:
 *
 *   DISCONNECT -> AW_NET -> AW_QLLC_SEC -> CONNECT
 *
 * Note that the order of state definitions is important.  >= AW_NET
 * implies there maybe somekind of network thing that needs to be
 * torn down.  >= DW_NET indicates something needs to be torn down
 * when the network side async disconnects.
 */

typedef enum _lnx_state_t {
    LNX_DISCONNECT,		/* completely disconnected */
    LNX_DW_QLLC,		/* waiting for qllc to disconnect - not used */
    LNX_AW_QLLC_PRI,		/* awaiting for primary sdlc to come up */
    LNX_AW_NET_QLLC_SEC,
    LNX_NET_CONTACT_PENDING,	/* received QSM on DLC side, sent SABME/LINKUP_REQ, await UA */
    LNX_DW_NET,			/* waiting for network side to get torn down */
    LNX_AW_NET,			/* waiting for net media, TR or NET */
    LNX_AW_QLLC_SEC,		/* waiting for secondary sdlc to come up */
    LNX_CONNECT			/* fully connected, data flows */
} lnx_state_t;

/* bogus event */
#define    LNX_INVALID		-1		 /* unacceptable input */
/* map packets from the network to events */
 
typedef enum _lnx_net_event_t {
    LNX_CONN_REQ,		/* SABME */
    LNX_DISC_REQ,               /* DISC */
    LNX_UA,                     /* UA (only if not local_ack) */
    LNX_QUENCH_ON,              /* RNR  (saved for future flow control */
    LNX_QUENCH_OFF,             /* RR   (saved for future flow control */
    LNX_DM,                     /* DM */
#ifdef notdef
    LNX_SFRAME,			/* RR/RNR/REJ */
#endif
    LNX_DATA                    /* I frame */
    } lnx_net_event_t;

/* interesting events from the DLC layer */
 
typedef enum _lnx_dlc_event_t {
    LNX_DLC_LINKUP_IND,		/* virtual circuit established/reset	*/
    LNX_DLC_LINKDOWN_IND,	/* virtual circuit lost			*/
    LNX_DLC_CONTACT_IND,	/* received QSM				*/
    LNX_DLC_CONTACTED_IND,	/* received QUA				*/    } lnx_dlc_event_t;


#define LNX_HASH_SIZE 64           /* must be power of 2 */
#define LNX_HASH_MASK (LNX_HASH_SIZE-1)


#define LNX_XID_SIZE         4
#define SNX_XID_SIZE         6
#define SNX_X25_PASS_SIZE    8

#define DEFAULT_SDLC_FRAG    265


#define LNX_MIN_THRESHOLD 40
#define LNX_MAX_THRESHOLD 80

/*
 * Event types for the lnx_linkup_process
 */

enum {
    LNX_NET_LINKUP_EVENT = 1,
    LNX_DLC_LINKUP_EVENT,
};

/* as far as possible make the initialization to zero be a useful
 * starting/reset value
*/

struct lnxtype_ {
    lnx_state_t		lnx_state;
    lnxtype		*next_hash;
    idbtype		*idb;
    uchar 		vmac_addr[IEEEBYTES];
    boolean 		configured_prtnr;
    boolean 		actual_prtnr;
    uchar 		prtnr[IEEEBYTES];
    ushort 		this_ring;
    ushort 		bridge_num;
    ushort 		target_ring;
    ushort  		qllc_largest;

    /* receptacles for UGLY variables used to create a lak2_lackie */
    paktype		*lnx_startup_pak;
    tr_ventrytype	*lnx_vre;
    
    /* manage XID command/response protocol for lnx
     * flag set means ROUTER received XID Command from net,
     * should send XID Response next time
     * only used in XID pass-through mode
     * 	 SET in lnx_startup_seq when XID Command received
     * 	 TEST and CLEAR in lnx_llc_xid when XID response pkt built
     */
    boolean		lnx_xid_rsp_pending;
    
    /* flag so that ROUTER handles only the first TEST response it sees
     * 		SET when we send explorer(TEST) Command
     * 		TEST, when we received explorer Response
     * 		CLEAR, if set - only true for first explorer back
     */
    boolean		lnx_test_rsp_pending;
    
    /* flag to let us know that partner is replying to our XIDs  */
    boolean		lnx_xid_seen;
    paktype             *lnx_xid_pak;
    boolean		lnx_xid1;
    paktype		*lnx_xid1_pak;
    
    /* flag to mark state variables for upstream/downstream startup */
    boolean      	lnx_net_data_seen;      /* lets us know that llc
						 * device has received 
						 * our UA
						 */
    uchar 		dsap;
    uchar 		ssap;
    boolean 		qllc_set_largest;
    llctype 		*llc;
    qllctype		*qllc;
    /*XXX*/
    sdlc_data_block	*sdlcp;
    boolean 		passwd_valid;
    uchar 		x25_passwd[SNX_X25_PASS_SIZE];
    
    boolean 		lnx_lak_enable;
    boolean 		xid_valid;
    boolean		xid_poll;
    uchar 		xid[LNX_XID_SIZE];
    
    /* end station characteristics */
    ushort 		lnx_net_maxbtu;       /* from the LLC side */
    ushort 		lnx_dlc_maxbtu;       /* from the QLLC side */
    
    /* XID 3 spoofers */
    uchar 		lnx_llc_abm;
    uchar 		lnx_qllc_abm;
    
    /* statistics */
    uint 		lnx_quench_on_count;
    uint 		lnx_quench_off_count;
};


/* NT 2.0 support
 * Reverse QLLC connection to a FEP requires that we not send XID(NULL)
 * for a PU2.0.  Solution is NOT to forward  NULL XID from PU, but
 * to poll Downstream (LAN-attached) device with NULL XID
 */

typedef struct xidnulltype_ xidnulltype;

struct xidnulltype_ {
    uchar       xid_ctl;        /* the 'BF' control byte */
};


/* NT 2.1 Support
 *
 * lnx is used to carry xid's from one side to the other, and to
 * change as little as possible
 *
 * To marry an NRM protocol such as SDLC or QLLC to and ABM protocol
 * such as LLC2 it's necessary to spoof the XID3 ABM bit.
 *
 * XID Format 1 handling:
 *
 * THIS ONLY APPLIES TO PU2.  IT DOES NOT COVER PU1
 *
 * Map Incoming XID 1 T2 (secondary) to XID 0 T2, 
 * 	on outgoing just send NULL XID 
 *
 * Map Incoming XID 1 T2 (primary) to NULL XID,
 *  	on outgoing send XID 1 T 2
 *
 * (If we get an XID3 from the LAN side, fail and bail)
 */

#define		XID0_SIZE	6	/* Fixed length */
#define         XID_XID1_PU2    0x12    /* PU2 XID format 1 */
 
#define         XID_XID3        0x30    /* XID format 3 */
#define         XID3_ABM        0x40    /* bit setting */
   
/*
 * Provide references to XID1 fields as offsets - 
 * Reference IBM Formats manual: GA27 3136-12
 */

#define		XID1_MIN_SIZE		18
#define		XID1_ROLE_TWS_OFF	8
#define		XID1_MAXBTU_OFF		10
#define		XID1_MAX_WINDOW_OFF	16

#define		XID1_PRIMARY_MASK	0x20	/* 0: secondary, 1: primary */
 
typedef struct xid3type_ xid3type;
 
struct xid3type_ {
    uchar       xid_ctl;        /* the 'BF' control byte */
    uchar       xid_fmt;        /* 0: 0x32 for XID-3    */
    uchar       xid_len;        /* 1: length            */
    uchar       xid_idblk_idnum[LNX_XID_SIZE];          /* 2-5  */
 
    /* xid 3 continuation */
 
    uchar       xid_fill1[2];   /* 6,7:                 */
    uchar       xid_properties[2]; /* 8,9: init_self, binds, xchg_state etc */
    uchar       xid_bind_pace;  /* 10:                  */
    uchar       xid_fill2[6];   /* 11-16                */
    uchar       xid_dlc_type;   /* 17: always 1 'SDLC'  */
    uchar       xid_dlc_length; /* 18: always 11        */
 
    /* SDLC  (and QLLC, LLC2) continuation */
 
    uchar       xid_lsflags;    /* 19: ABM, role        */
    uchar       xid_fill3;      /* 20:                  */
    uchar       xid_maxbtu[2];  /* 21,22 max btu size the ls can receive */
    uchar       xid_profile;    /* 23: command and response set always 0 */
    uchar       xid_sim_rim;    /* 24: SIM and RIM support */
    uchar       xid_fill4[2];   /* 25,26:               */
    uchar       xid_maxiframe;  /* max number of i-frames in the window */
} ;


/*
 * QLLC/LNX enternal variables
 */

extern boolean   lnx_running;
extern boolean   lnx_debug;
extern boolean   lnx_error_debug;
extern boolean   lnx_lak_enable;

/*
 * Prototypes
 */


/* lnx.c */

extern void lnx_init(subsystype *subsys);
extern void lnx_start(void);
extern lnxtype *lnx_findfree(void);
extern void lnx_freeup(lnxtype *lnx);
extern boolean lnx_x25_map_add(uchar *vmac, x25map *map);
extern void lnx_x25_map_delete(lnxtype *lnx);
extern boolean lnx_busy(addrtype *addr);
extern boolean lnx_linkupBLOCK(queuetype *queue);
extern forktype lnx_net_linkup_proc(void);
extern forktype lnx_dlc_linkup_proc(void);
extern void lnx_dlc_ind_connect(lnxtype *lnx);
extern void lnx_startup_seq(paktype *pak, lnxtype *lnx, 
			    hwidbtype *inidb, tr_ventrytype *vre);
extern void lnx_qllc_notify(lnxtype *lnx, lnx_dlc_event_t event);
extern void lnx_contact_net(lnxtype *lnx);
extern boolean lnx_local_ack_this(lnxtype *lnx);
extern void lnx_net_disc_ack(lnxtype *lnx, paktype *pak);
extern void lnx_disconnect_net(lnxtype *lnx);
extern void lnx_net_ind_disconnect(lnxtype *lnx);
extern void lnx_net_ind_connect(lnxtype *lnx);
extern lnx_net_event_t lnx_map_net_event(lnxtype *lnx, 
					 int opcode, uchar conch);
extern void lnx_input(hwidbtype *inidb, paktype *pak, 
		      int opcode, tr_ventrytype *vre);
extern void lnx_quench(lnxtype *lnx, boolean quench_on);
extern void lnx_initiate(lnxtype *lnx);
extern void lnx_net_fwd_xid(lnxtype *lnx, paktype *pak, boolean qllc_response);
extern paktype *lnx_to_llc_xid(lnxtype *lnx, paktype *pak, 
			       uchar *rif, hwidbtype *vidb);
extern void lnx_output(paktype *pak, qllctype *qllc);
extern void lnx_vrforward_data(lnxtype *lnx, paktype *pak);
extern void lnx_cleanup_llc(llctype *llc);
extern void lnx_fix_llc(llctype *llc, paktype **ppak);
extern void lnx_fix_qllc(qllctype *qllc, uchar *macaddr);
extern boolean lnx_create_lackie(lnxtype *lnx);
extern void lnx_lak2_dlc_event(lnxtype *lnx, int minor);
extern void lnx_setstate(lnxtype *lnx, lnx_state_t newstate);

extern void lnx_vidb_input(hwidbtype *idb, paktype *pak, 
			   ushort opcode, tr_ventrytype *vre);
extern hwidbtype *lnx_new_vidb(hwidbtype *inidb, 
			       int thisring, int bridgenum, int targetring);
extern boolean rsrb_lnx_oqueue(hwidbtype *idb,
			       paktype *pak, enum HEADTAIL dummy);
extern void lnx_forward_held_xid(lnxtype *lnx, paktype *pak);
extern lnxtype *lnx_by_macaddr(uchar *macaddr);
extern void lnx_enter_macaddr_hash(lnxtype *lnx);
extern void lnx_remove_macaddr_hash(lnxtype *target);
extern boolean lnx_validate(lnxtype *target);
extern int lnx_hash_macaddr(uchar *macaddr);
extern void lnxif_command(parseinfo *csb);
extern void lak_lnx_register(llctype *cookie);
extern char *lnx_pr_event(lnx_net_event_t event);
extern char *lnx_prstate(lnx_state_t state);
extern void snx_periodic(void);
extern void snx_upstream(hwidbtype *sdlc_idb);
extern void snx_output(hwidbtype *idb, paktype *pak);
extern void snx_input(paktype *pak, qllctype *qllc);
extern void snx_clean_sdlc(qllctype *qllc);
extern void snx_disconnect_sdlc(lnxtype *lnx);
extern void snx_config_pass(parseinfo *csb, idbtype *idb);
extern void lnx_upstream(lnxtype *lnx);
extern void pr_xid(paktype *pak, int xidoffset);
extern void show_lnx(void);

#endif __LNX_H__
