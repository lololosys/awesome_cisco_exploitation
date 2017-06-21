/* $Id: llc2.h,v 3.12.4.5 1996/08/28 13:00:16 thille Exp $
 * $Source: /release/112/cvs/Xsys/llc2/llc2.h,v $
 *------------------------------------------------------------------
 * llc2.h -- LLC2 header file
 * 
 * January 1991, Rick Watson
 * 
 * llc2.h   version 4.4  Copyright ERCC 1986,1987,1988 
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: llc2.h,v $
 * Revision 3.12.4.5  1996/08/28  13:00:16  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.12.4.4  1996/08/10  01:04:28  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.12.4.3  1996/05/17  11:27:08  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.9.2.8  1996/05/14  16:36:35  ppearce
 * CSCdi57596:  Spurious access from llc2_init_fair_sum()
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.2.7  1996/05/05  23:32:46  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.9.2.6  1996/05/02  15:52:03  sbales
 * CSCdi51340:  Memory Leak DLSw on activation of PU2.1
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.2.5  1996/04/15  00:09:16  kchiu
 * Branch: IbuMod_Calif_branch
 * Add dlsw+/fddi support
 *
 * Revision 3.9.2.4  1996/03/28  08:20:50  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.9.2.3  1996/03/21  05:12:57  ppearce
 * IBU modularity: - LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC2 defaults definition into the LLC subblock file
 *
 * Revision 3.9.2.2  1996/03/17  18:07:01  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.9.2.1  1996/03/01  16:31:14  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.12.4.2  1996/04/25  23:15:45  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.12.4.1  1996/04/17  13:44:52  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.12  1996/03/07  23:25:32  smackie
 * Fix inlines that could theoretically return without a value so that
 * the 95q4 toolchain will compile them. (CSCdi49069)
 *
 * Revision 3.11  1996/03/01  00:49:04  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up correctly
 * Restore CSCdi47275 w/fix to netbios_as.c
 *
 * Revision 3.10  1996/02/28  01:02:51  kchiu
 * CSCdi48822:  llc2 windowing mechanism doesn't work properly under
 * congestion
 *
 * Revision 3.9  1996/02/24  01:07:10  lpereira
 * CSCdi49162:  appn over frame relay broken!
 * Base the FR_L3_PID offsets off of LLC_MAX_DTE_SIZE so that it will
 * pick up any changes to the size of dtestrin
 *
 * Revision 3.8  1996/02/16  18:09:25  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Backout CSCdi47275 temporarily-- it breaks igs-c-l build
 *
 * Revision 3.7  1996/02/15  01:38:08  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Fix cleanup and change appl. cleanup from registry to vector
 *
 * Revision 3.6  1996/01/29  23:28:48  vrawat
 * CSCdi47386:  FRAS should ignore l3pid in frames received from
 * frame-relay side
 *
 * Revision 3.5  1996/01/23  18:06:48  dbilling
 * CSCdi46491:  APPN & DSPU link act may fail when mult. link activated
 * Added llc2_build_hwaddr_rmac to build hwaddr from llc
 *
 * Revision 3.4  1995/11/21  19:34:00  sberl
 * CSCdi42181:  More than 1000 LLC2 sessions causes excessive tracebacks
 * Remove the check for > 1000 llc sessions from llc2_tick().
 * Add a counter to keep track of how many llc2 sessions there really are.
 * Add this count to banner for "show llc2".
 * Make llc2_starttimer() a real function instead of an inline. It was way
 * too big.
 *
 * Revision 3.3  1995/11/17  17:43:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:01:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:35:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:15:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/03  21:09:09  hampton
 * Convert LLC2 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38203]
 *
 * Revision 2.4  1995/07/06 21:46:46  sberl
 * CSCdi36777:  WFQ should count llc2 sessions as conversations
 * Add logic to classify LINK_LLC2 packets into LLC2 conversations.
 *
 * Revision 2.3  1995/06/28  09:29:03  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/09  19:28:28  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * All dlus hard code the default behaviour until they get around to
 * adding configuration.
 *
 * Revision 2.1  1995/06/07  21:41:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LLC2_H__
#define __LLC2_H__

#include "address.h"
#include "logger.h"
#include "ieee.h"
#include "../if/rif.h"
#include "../cls/dlc_public.h"

/* 
 * Compile time variables 
 */

/* 
 * We use 20% of 200 for the defaults so that backpressure can be
 * sent very early on high speed wan links where the tx q can get
 * blown out before the quench makes it back to the originating 
 * host.  This should also work well for quenching SDLLC, etc...
 */

#define LLC2_PRINT_LENGTH	40	/* length of printable llc2 id */
#define LLC2_CONGESTED  2               /* caller show flowcntl */

#define LLC_OQUEUE_HIGH_WATER_MARK  20  /* 20% of txQ           */
#define LLC_OQUEUE_LOW_WATER_MARK    5  /* 5% of txQ            */
#define LLC_CLS_FLOW_IND_DAMP        5  /* damp zone -scalar-   */

#define LLC_DTE_HASH_SIZE 256		/* must be power of 2 */
#define LLC_DTE_HASH_MASK (LLC_DTE_HASH_SIZE-1)
#define LLC_MAX_DTE_SIZE  18
#define LLC2_FAIR_SEQ_DEFAULT FAIR_QUEUE_BASE_WEIGHT

/* 
 * The values for the control PDUs 
 */
#define SABME_IND   0
#define DISC_IND    1
#define XID_IND     2
#define UA_IND      3
#define DM_IND      4
#define XID_RSP_IND 5
#define RR_CMD_P0   6
#define RNR_CMD_P0  7
#define REJ_CMD_P0  8
#define RR_CMD_P1   9
#define RNR_CMD_P1  10
#define REJ_CMD_P1  11
#define RR_RSP      12
#define RNR_RSP     13
#define REJ_RSP     14
#define FRMR_IND    15
#define I_CMD_IND   16
#define I_RSP_IND   17
#define TXTOTSIZE   20

/* 
 * The command formats 
 */
#define UI	 0x0003
#define RIM	 0x0007			/* rsp only, with F bit */
#define LLC_SIM	 0x0007			/* cmd only, with P bit */
#define DM       0x000F
#define LLC_UP	 0x0023			/* unnumbered poll */
#define RD	 0x0043			/* rsp only, with F bit */
#define DISC     0x0043			/* cmd only, with P bit */
#define UA       0x0063
#define SABME    0x006F
#define SNRM	 0x0083			/* sdlc only? */
#define FRMR     0x0087
#define XID      0x00AF
#define CFGR	 0x00C7
#define SNRME	 0x00CF
#define LLC_TEST 0x00E3
#define BCN	 0x00EF

#define RR      0x01
#define RNR     0x05
#define REJ     0x09

#define CMDBIT  0
#define RSPBIT  1
#define P_BIT   (0x10)

/*
 * LLC pdu types
 */
#define LLC_IFRAME       0
#define LLC_IMASK        1
#define LLC_SUPERVISORY  1
#define LLC_UNNUMBERED   3
#define LLC_SMASK        3


/* 
 * The states 
 */
#define LLS_NONE       0
#define LLS_ADM        1
#define LLS_SETUP      2
#define LLS_RESET      3
#define LLS_D_CONN     4
#define LLS_ERROR      5
#define LLS_NORMAL     6
#define LLS_BUSY       7
#define LLS_REJECT     8
#define LLS_AWAIT      9
#define LLS_AWAIT_BUSY 10
#define LLS_AWAIT_REJ  11


/* 
 * The events 
 */
#define REC_I_CMD       1
#define REC_I_RSP       2
#define REC_RR_CMD      3
#define REC_RR_RSP      4
#define REC_RNR_CMD     5
#define REC_RNR_RSP     6
#define REC_REJ_CMD     7
#define REC_REJ_RSP     8
#define REC_SABME_CMD   9
#define REC_DISC_CMD    10
#define REC_UA_RSP      11
#define REC_DM_RSP      12
#define REC_FRMR_RSP    13
#define REC_BAD_PDU     14	/* what is it's use? */
#define REC_BAD_CMD     14
#define REC_BAD_RSP     15
#define TX_DM_RSP       16
#define REC_INV_LSAP    17
#define REC_INV_ILEN    18
#define REC_INV_IFIELD  19
#define REC_TOO_SMALL   20
#define CONN_REQ        21
#define DATA_REQ        22
#define RESET_REQ       23
#define DISC_REQ        24
#define P_TIMER_EXP     25
#define ACK_TIMER_EXP   26
#define REJ_TIMER_EXP   27
#define BUSY_TIMER_EXP  28
#define XID_TIME_EXP    29
#define IDLE_TIME_EXP   30
#define CLOK_TX_ACTION  31
#define LOCK_TX_ACTION  32
#define SET_LOCAL_BUSY  33
#define CLEAR_LOCAL_BUSY  34
#define INIT_PF_CYCLE   35
#define REC_XID_CMD     36
#define REC_XID_RSP     37
#define REC_TEST_CMD    38
#define REC_TEST_RSP    39

/* 
 * Definitions of indtypes 
 */
#define LC_CONNECT   1
#define LC_CONCNF    2
#define LC_DATA      3
#define LC_DATACNF   4
#define LC_DISC      5		/* Original, obsolete, left from compat */
#define LC_DISCNF    6
#define LC_DM	     7		/* Rxd DM from LAN */
#define LC_RESET     8		/* REC_SABME_CMD in d_connstate */
#define LC_RSTCNF    9
#define LC_REPORT    10
#define LC_FLOWCONTROL 11
#define LC_TIMEOUT   12		/* n2 timer expiry */
#define LC_DISC_TIMEOUT   13		/* n2 timer expiry */
#define LC_INTDOWN   14		/* interface went down (llc2_cstate*/
#define LC_FRMR	     15		/* rxd a FRMR from LAN */
#define LC_ABORTED   16		/* Abort request from Lack 2
				   or from DLC */
#define LC_USER_CLOSE 17	/* Abort request from CLSI DLU */
#define LC_DISC_LAN   18	/* Rxd DISC from LAN */
#define LC_DISC_CNF   19	/* Confirm an internal (DLU/CLS) disc request */

/* 
 * Definitions of status values 
 */
#define LS_SUCCESS         1
#define LS_RESETTING       2
#define LS_RESET_DONE      3
#define LS_DISCONNECT      4
#define LS_FAILED          5
#define LS_CONFLICT        6
#define LS_RESET_FAILED    7
#define LS_RESET_REFUSED   8
#define LS_RESET_DECLINED  9
#define LS_FRMR_RECEIVED  10
#define LS_FRMR_SENT      11
#define LS_REM_BUSY       12
#define LS_REM_NOT_BUSY   13
#define LS_WAITING	  14

/* 
 * Return codes from analysis of input 
 */
#define INV_CMD    1
#define INV_RSP    2
#define INV_DSAP   3
#define INV_SUP    4
#define NO_CIRC    5
#define IFORMAT    6

/* 
 * VWXYZ bits for FRMR 
 */
#define FRMR_VBIT 16
#define FRMR_WBIT 1
#define FRMR_XBIT 2
#define FRMR_YBIT 4
#define FRMR_ZBIT 8

/*
 * Length  of the Info field in FRMR frame
 */
#define FRMR_INFO_LENGTH   5
  

#define DISC_R_BIT  1
#define RESET_R_BIT 2
#define CONN_R_BIT   4
#define REQ_AK_BIT   8
#define TXDATA_R_BIT 16
#define XID_R_BIT    32

/* 
 * the statistics table definitions 
 */
#ifdef notdef
#define IG_FRAME_SUM  0
#define RE_Q_AK_SUM   1
#define MDC_DELAY_SUM 2
#define REM_BUSY_SUM  3
#define NO_RESOURCE_SUM  4
#define NO_BUFFS_SUM     5
#define EV_FAIL       6
#define LAST_EVFAIL   7
#define LLC2_STATMAX  8
#endif

/*
 * Timer values 
 */
#define XID_RTRY_TIMER 0
#define ACK_TIMER      1
#define P_TIMER        2
#define IDLE_TIMER     3
#define REJ_TIMER      4
#define BUSY_TIMER     5
#define AKDELAY_TIMER  6
#define STATUS_TIMER   7
#define LLC1_TIMER     8
#define ADM_TIMER      9
#define LLC_NTIMERS   10

/*
 * Timer flag masks when set in timer_flags indicate that 
 * a timer is running.
 */
#define XID_RTRY_TIMER_M (1<<XID_RTRY_TIMER)
#define ACK_TIMER_M      (1<<ACK_TIMER)
#define P_TIMER_M        (1<<P_TIMER)
#define IDLE_TIMER_M     (1<<IDLE_TIMER)
#define REJ_TIMER_M      (1<<REJ_TIMER)
#define BUSY_TIMER_M     (1<<BUSY_TIMER)
#define AKDELAY_TIMER_M  (1<<AKDELAY_TIMER)
#define STATUS_TIMER_M     (1<<STATUS_TIMER)
#define LLC1_TIMER_M     (1<<LLC1_TIMER)
#define ADM_TIMER_M      (1<<ADM_TIMER)

/* Various vlaues of the Flag used to pass messages to LLC2 */
#define LLC_NO_RNR             0x01

/* 
 * On timer queue mask.  Be sure not to overwrite with other masks.
 * This flag indicates we are on the timer queue since stoptimer
 * just clears the above bits and lets the clock routine remove us
 * from the queue.
 */
#define ON_TIMERQ_M 0x8000

/*
 * status timer
 */
#define LLCSF_OPEN	0x0001		/* non blocking open */
#define LLCSF_RESET	0x0002		/* link level reset */
#define LLCSF_IN_HASH   0x0004          /* in hash table */

/* 
 * structures 
 */

/* 
 * The offsets within the datalink dte field of the SAPs and mac addresses
 */
#define RMAC_DTE  0			/* Offset of remote mac (48 bit) */
#define LSAP_DTE  6			/* Offset of local sap  (8 bit)  */
#define RSAP_DTE  7			/* Offset of remote sap (8 bit)  */
#define LMAC_DTE  8			/* Offset of local mac  (48 bit) */
                                        /*  [ two byte pad for smds ]    */
#define RIF_DTE   18			/* Offset of rif      (18 bytes) */
/*
 * Offsets of parts of the dte string for Frame Relay 10 bit DLCI addressing
 */
#define FR_DLCI_DTE 0			/* Offset of the DLCI */
#define FR_LSAP_DTE 2			/* Offset of the local sap */
#define FR_RSAP_DTE 3			/* Offset of the remote sap */
#define FR_L3_PID_DTE  LLC_MAX_DTE_SIZE /* Offset of the Level 3 */
					/* protocol ID byte,changed from 4, see CSCdi47386 */
/*
 * Offsets of parts of the dte string for AIP VCD addressing
 */
#define ATM_VCD_DTE 0			/* Offset of the VCD */
#define ATM_LSAP_DTE 2			/* Offset of the local sap */
#define ATM_RSAP_DTE 3			/* Offset of the remote sap */
#define ATM_L3_PID_DTE 4		/* Offset of the Level 3 */
					/* protocol ID byte */
/*
 * L3 PID is used over frame-relay and ATM-rfc1483
 */
# define NULL_L3_PID	0
# define NULL_DLCI      0
/*
 * struct to define the string that is passed on to FR.
 */
struct llc2_fr_dte_str {
    ushort dlci;
    uchar  lsap;
    uchar  rsap;
    uchar  pad[LLC_MAX_DTE_SIZE - 4];      /* added, see CSCdi47386 */
    uchar  l3pid;
};

/*
 * Offsets of parts of the dte string for PPP 
 */
#define PPP_LSAP_DTE 0			/* Offset of the local sap */
#define PPP_RSAP_DTE 1			/* Offset of the remote sap */

struct llc2_ppp_dte_str {
    uchar    lsap;
    uchar    rsap;
};

/*
 * Offsets of parts of the dte string for SMDS addressing
 */
#define SMDS_RMAC_DTE 0			/* Offset of the remote mac */
#define SMDS_LMAC_DTE 8                 /* Offset of the local mac */
#define SMDS_LSAP_DTE 16		/* Offset of the local sap */
#define SMDS_RSAP_DTE 17		/* Offset of the remote sap */

/*
 * struct to define the string that is passed on to SMDS
 */
struct llc2_smds_dte_str {
    uchar  rmac[STATIONLEN_SMDS64];
    uchar  lmac[STATIONLEN_SMDS64];
    uchar  lsap;
    uchar  rsap;
};

/*
 * LLC2_SECONDARY_CONTACTED is a 'slimy' state used to maintain the 'state info'
 * that DLC has contacted CLI with a llc2ClsNotify(llc, CLS_CONNECTED_IND)
 * LLC2_SECONDARY is strongly recommended to be left set to 0.  The value of zero
 * allows a compiler optimization which allows test to be executed in one less
 * instruction.
 */
typedef enum LlcRoleT_ 
{
    LLC2_UNDEFINED = -1,	/* set initial value non-zero for debugging */
    LLC2_SECONDARY = 0,		/* link station receives UA */
    LLC2_SECONDARY_CONTACTED, /* CLS_CONNECTED_IND sent to CLS */
    LLC2_PRIMARY		/* link station sends SABME */


#ifdef ORIGINAL_CLS_IMPLEMENTATION
    LLC2_SECONDARY_CONTACT_PENDING, /* link rx'd first I or S frame, but CLS
				     *  module hasn't been notified yet.
				     */

    LLC2_SECONDARY_RSP_RXD
#endif

} LlcRoleT;

/*
 * More slime to get the current code to work with the CLS interface/model.
 * The current legacy code always responds immediately to DISC U-frames sent
 * on the LAN.  Therefore, there is sufficient state context to know which
 * response is appropriate.  For example, given the following LLC2 states, here
 * is the appropriate response to a DISC:
 *                State               Response
 *                adm                 DM
 *                setup               DM
 *                reset               DM
 *                d_conn              UA
 *                error               UA
 *                normal, busy
 *                reject, await_*     UA
 * 
 * Because CLS controlled DLC do not respond to Disconnect.ind's until DLU's 
 * send a Disconnect.rsp, it is necessary to store the state context, so the
 * appropriate response can be made...
 */
typedef enum DiscAckRspTypeT_ 
{
    LLC2_DISC_RSP_NONE = 0,	/* no response required */
    LLC2_DISC_RSP_DM,		/* respond with a DM frame */
    LLC2_DISC_RSP_UA,		/* respond with a UA frame */
    LLC2_DISC_RSP_SENT		/* appropriate response has been sent... */
	
} DiscAckRspTypeT;

typedef enum CauseFlagTypeT_
{
    LLC2_External_Cause = 0,	/* matches value in IEEE 802.2-1985 */
    LLC2_Internal_Cause = 1
} CauseFlagTypeT;

/*
 * Signal new return codes for failures within llc2_open1.
 * CLS LLC's require a bit more information to return to DLU's
 */
typedef enum llc2OpenFailureCode_
{
    LLC2_LlcInUse  = -1,
    LLC2_LlcNoMem  = -2,
    LLC2_LlcBadIdb = -3
} llc2OpenFailureCode;

/*
 * Various things marked as (kept << 1) mean that they are kept shifted
 * one.  This is a convenience to keep them aligned as they will 
 * be in the transmitted/received frame.  This is also why things
 * like N(R) are incremented by 2 (1<<1) in the code.
 */

typedef struct
{
    void * q_next;                      /* next element in queue */
    struct llctype_ * q_context;        /* back pointer to llctype */
} LlcQElement;

struct llctype_ {
    struct llctype_ *timer_queue;	/* next in timer list */
					/* next in freelistQ  */
    struct llctype_ *next_hash;		/* next in hash list */
    struct llctype_ *llc_sap_link;	/* next in sap_entry's list */
    LlcQElement      q_elem;            /* Linkage for freelistQ & cleanupQ */

    void		*u_cep_id;	/* clsi handle for cls layer 	*/
    CepT	fCep;			/* Common fields for all dlc types */

    boolean		llc_req_opn_stn_sent; /* reply outstanding	*/
    boolean		llc_dlu_xid_rsp_pending; /* sent Cmd to DLU, await RSP */
    LlcRoleT            llcRole; /* Are we primary/secondary T/F? */
    DiscAckRspTypeT     llcDiscAckRspType; /* response type for rx'd DISC cmd,
					    * see comment above typedef...
					    */

    idbtype *idb;			/* pointer to idb */
    int linktype;			/* linktype for connection */
    int open_block;			/* zero if blocking else result */
    int adm_reason;			/* reason for going back to ADM  */
    boolean blocking;			/* true if non blocking open */
    void (*statusproc)(llctype *, int);	/* external status indication */

    /* state, nsref, txaction, txqnum and txq are also common to lapb */
    unsigned char state;
    unsigned char txaction;
    unsigned char txqnum;		/***** ? *****/

    /* the buffer queues for transmit and ack (emptied by zero_v_vals) */
    queuetype txQ;
    queuetype akQ;
    /* current input buffer */
    paktype *inbuf;

    /* 
     * smac, dsap, lsap, dmac.  First 8 bytes used by llc2.  Last 6 bytes
     *  used by rsrb local ack to indicate "dce" side of connection.
     * (0 if local ack not being used for a connection)
     *
     * To support Frame Relay, a new format is added. 
     * 2 DLCI number, 1 byte LSAP, and 1 byte RSAP.
     * All other bytes are set to zero.
     *
     * 
     * CLS Rif enhancement to allow Rif storage in LLC structure.
     * llc2_dte is passed into *(idb->hwptr->vencap)(), so rather than
     * butcher current interface, append rif information into current
     * string.  For transparent or locally attached rings, Rif RC
     * information will be 0x0000.  Append 'SRB_MAXRIF' here and not
     * in LLC_MAX_DTE_SIZE definition because latter is used for hash
     * key purposes and RIF information is used only for storage and
     * not for hash calculations.
     */
    unsigned char llc2_dte[LLC_MAX_DTE_SIZE + SRB_MAXRIF];
    uchar llc2_addrtype;		/* STATION_IEEE48 or */
					/* STATION_FR10. This is used */
					/* to decide how to interpret */
					/* the dte string. */
    
    /* 
     * The above array consists of a six byte remote dte address followed
     * by the local LSAP followed by the remote LSAP, enabling a rapid
     * compare to be carried out on incoming frames 
     */
    /* xid and window parameters are maintained regardless of the state */
    unsigned char rem_wind;		/* (kept << 1) */
    unsigned char loc_wind;		/* (kept << 1) */

    /*
     * Control priority of traffic for this session.
     *
     */
    int llc2_queuing_type;
    ulong llc2_queuing_parameter;
    short llc2_fair_sum;
    int llc2_fair_sequence;
    
    /* 
     * cause_flag is only used in D_CONN and RESET states to remember
     * originator of request, and is only changed on entry to these states
     */
    CauseFlagTypeT  cause_flag;

    /* 
     * non-xid timer values and p_flag cleared by stop_all_timers 
     */
    unsigned char p_flag;

    /* 
     * the following are the sequence bytes and retrycount which are zeroed by
     * zero_v_vals, which also empties the queues and clears rem_busy,
     * akqnum and txqnum
     */
    unsigned char v_r;			/* (kept << 1) */
    unsigned char v_s;			/* (kept << 1) */
    unsigned char last_n_r;
    unsigned char retry_count;
    unsigned char rem_busy;
    unsigned char event;		/* updated by stateswitch function */
    unsigned char akqnum;

    /* the following are zeroed by llc2_goto_adm */
    unsigned char s_flag;	     /* mechanism to remember clashing SABME */
    unsigned char data_flag;

    /* 
     * the following are evaluated when an appropriate input frame 
     * is received 
     */
    unsigned char p_f_val;
    unsigned char cmdrspbit;
    unsigned char need_f_bit;
    unsigned char f_bit_set;
    unsigned char new_n_s;		/* received packet n(s) */
    unsigned char new_n_r;		/* received packet n(r) */
    unsigned char inval_n_s;
    unsigned char inval_n_r;
    unsigned char unexp_n_s;

    /* 
     * the number of aks which are currently being delayed for 
     * this connection
     */
    unsigned char notaknum;

    /*
     * Each timer has a bit in timer_flags that indicates 
     * if the timer is running. next_timer is the time of the 
     * next timer to expire.
     */
    ushort timer_flags;
    ushort status_flags;		/* flags for status timer */
    sys_timestamp timers[LLC_NTIMERS];
    sys_timestamp next_timer;

    /*
     * the values constructed when a frame reject is to be transmitted
     */
    unsigned char frmrchars[5];

    /* 
     * parameters for llc1 state management 
     */
    ulong		llc1_timer;	/* Timer and counter for explorers */
    uchar		llc1_max_retries;
    ulong               xid_timer;	/* Timer and counter for XIDs */
    uchar               xid_max_retries;
    paktype		*llc_xid_pak;	/* xid we might retransmit */
    uchar               llc_explorer_options;

    /*
     * For the llc test commands
     */
    int test_cookie;

    ulong        fFlags;                 /* General Flag to pass params */
    /*
     * RSRB Local ACK
     */
    boolean rsrb_lack;
    lak_t *l2_lakptr; /* Back-pointer to LACK hashtable */
    boolean lack_llc2_open_ok;	/* Is this llc2 open okay to do? */
    boolean rnr_disable;
  
    /*
     * SDLLC
     */
    boolean sdllc_active;		/* SDLLC active on this vc? */
    int sdllc_address;			/* SDLLC address for this thing */
    hwidbtype *sdllc_idb;		/* SDLLC interface */
    
    /*
     * QLLC
     */
    boolean lnx_active;			/* QLLC active on this thing */

    /* 
     * media_llc_cleanup is a vector pointing to the appropriate application
     * cleanup routine for this llc.  sdllc, lnx, and lack set this vector
     * to point to the specialized llc cleanup required for their functions.
     */
    void (*media_llc_cleanup)(llctype *);

    /*
     * CONS specific fields
     */
    considbtype *considb;

    /*
     * LLC2 values
     */
    ulong llc2_akmax;
    ulong llc2_akdelay_time;		/* time to delay ack */
    ulong llc2_wind;			/* local window size */
    ulong llc2_T1_time;			/* T1 time */
    ulong llc2_n2;			/* N2 retransmission count */
    ulong llc2_trej_time;
    ulong llc2_tpf_time;
    ulong llc2_tbusy_time;
    ulong llc2_idle_time;
    ulong llc2_xid_rtry_time;
    ulong llc2_xid_neg_val_time;
    ulong llc2_txQ_max;			/* queue depth of the llc transmit queue */
    ulong llc2_adm_timer_value;

    /*
     * Dynamic window fields
     */
    ulong nw;                /* number of good consecutive I-frame */
    ulong fr_dwc;            /* window size divisor */
    ulong ww;                /* working window */
    ulong good_frame_count;  /* good consecutive I-frame count */
    ulong orig_loc_wind;     /* original local window size */
    /* frame relay fields (for BAN encapsulation) */
    ushort fDlci;
    ushort enc_flags;        /* dlsw/fddi */

};
  
/*
 * External references
 */
extern llctype *llc2stationptr;
extern sys_timestamp llc2_nexttime;
extern queuetype llctQ;
#ifdef NOBODY_READS_ME
extern int llc_stats_ignored;		/* ignored frames */
#endif
extern queuetype llc2_cleanupQ;
extern queuetype llc2_freelistQ;
extern paktype *llc1_gen_explr (uchar *dmac, uchar *smac, uchar dsap, uchar ssap, hwidbtype *);
extern int llc_in_hash_count;
extern void SapSNAHandleSnaXid(idbtype *idb, paktype *pak, void *uSapId,
                               boolean isResponse);

/* the offsets within frmrchars */
#define FRMR_V_S   2
#define FRMR_V_R   3
#define FRMR_WXYZ  4

#define LLC2_DEBUG_PACKET (llc2_debug_packet && \
			   llc2_debug_filter(&llc2_debug_packet, llc))
#define LLC2_DEBUG_ERR (llc2_debug_err && \
			   llc2_debug_filter(&llc2_debug_err, llc))
#define LLC2_DEBUG_STATE (llc2_debug_state && \
			   llc2_debug_filter(&llc2_debug_state, llc))
#define LLC2_DEBUG_DYNWIND (llc2_debug_dynwind && \
			   llc2_debug_filter(&llc2_debug_dynwind, llc))


typedef struct llc2_register_type_ {
    void (*proc)(llctype *);	    /* notify of new connection */
    char link;
} llc2_register_type;

/*
 * external debugging.
 */

extern boolean llc2_debug_state;
extern boolean llc2_debug_err;
extern boolean llc2_debug_packet;
extern boolean llc2_debug_dynwind;
extern boolean llc2_debug_test;		/* for testing only */


/* llc2.c */

/* Find llctype based on various search criteria */
extern llctype *llc2_by_pak (paktype *pak, uchar *dtestring, uchar *addrtype,ushort *Dlci);
extern llctype *llc2_by_ucepid(void* u_cep_id);
extern llctype *llc2_findcirc(uchar *srcdteptr, idbtype *idb);
extern llctype *llc_by_dte(uchar *srcdtestring, int lsap, int dsap, idbtype *idb);

extern void llc2_build_dte_string(uchar* dtestring,
			   hwaddrtype* remoteMac, hwaddrtype* localMac,
			   uchar remoteSap, uchar localSap,
			   uchar l3pid);

extern llctype *llc2_findfree(int locallsap, idbtype *idb);
extern llctype *get_llctype(void);
extern void llc2_input(paktype *pak);
extern boolean fast_llc2_input(paktype *pak, llctype *llc);
extern void llc2call(llctype *llc);
extern void llc2_init(subsystype *subsys);
extern boolean llc2BLOCK(void);
extern forktype llc2_timer(void);
extern void llc2_init_idb(idbtype *, boolean);
extern lak_t *llc2_get_lackie(llctype *llc);
extern void llc_remove_dte_hash(llctype *target);
extern boolean validate_llc(llctype *target);
extern boolean llc2_register(int, int, void (*notifproc)(llctype *));
extern llctype *llc2_open(int dsap, int ssap, hwaddrtype *destmac, hwaddrtype *srcmac, idbtype *idb, int linktype, int wsize, void (*statusproc )(llctype *, int), boolean blocking, boolean local_ack);
extern llctype *llc2_open1(int dsap, int ssap, 
			     hwaddrtype *destmac, hwaddrtype *srcmac, 
			     idbtype *idb, int linktype, 
			     int wsize, void (*statusproc)(llctype *, int),
			     boolean blocking, boolean local_ack, 
			     boolean clsi_call, uchar nlpid, ushort Dlci);
extern llctype *llc2_open2(llctype *, boolean);
extern boolean llc2_attach(llctype *llcp, int linktype, int wsize,void (*statusproc)(llctype *, int));
extern boolean llc2_close(llctype *cookie);
extern dlc_oqueue_status_t llc_get_oqueue_status(llctype *cookie);
extern void llc2_send(llctype *cookie, paktype *pak, int len);
extern llctype *llc2_find_cb(uchar *mac, int lsap, int dsap, idbtype *idb);
extern boolean llc2_openBLOCK(llctype *cookie);
extern char *llc2_prreason(int reason);
extern boolean llc2_start(void);
extern boolean llc_encapsulate(paktype *pak, int type, int length, llctype *llc);
extern void llc_output(paktype *pak, int type);
extern void llc2_transmit(llctype *llc, paktype *pak, int size, int type);
extern int llc_sendframe(paktype *pak);
extern void show_llc2_command(parseinfo *);
extern void show_llctype(llctype *llc, boolean brief);
extern void hexout(uchar *buf, int bc, char *leader);
extern void init_test_command(void);
extern void llc_statusproc(llctype *llc, int status);
extern void test_lanmgr_enqueue(paktype *pak);
extern void llc2_cstate(hwidbtype *idb);
extern char *llc2_prid(llctype *);
extern char *llc2_prstate(llctype *, int);
extern char *llc2_prreason(int);
extern char *llc2_prevent(int);
extern char *llc2_prstatus(int);
extern char *llc2_prctlchr(int);
extern boolean llc2_debug_filter (boolean *pflag, llctype *llc);

/* These values are only used with llc2_store_rif() */

#define CHANGE_DIRECTION 1
#define DONT_CHANGE_DIRECTION 0

extern void llc2_store_rif(llctype * llc, uchar * rif, boolean change_direction);

extern boolean llc2_on_idb(idbtype *);
extern void llc2_respond_with_dm(paktype *);

/* llstate.c */
extern void ind_concnf(llctype *llc, int status);
extern void send_control(llctype *llc, int ctype);
extern void llc2_goto_adm(llctype *llc, int reason);
extern void llc2_stateswitch(llctype *llc, int newevent);
extern void llc2_release_UA(llctype *llc);
extern void llc2_examaction(llctype *llc);
extern void llc2_sendRR(llctype *llc);
extern short llc2_fair_queue_flow_id (paktype *pak);

/* llclocks.c */
extern void llc2_tick(void);

/* llc2_chain.c */
extern void llc2_parser_init(subsystype *);

/* llc2_media.c */
extern void llc2_media_registry(void);

/* Extract various parts of the dtestring based on address type value */

static inline uchar llc2_get_addrtype(llctype* llc)
{
    return(llc->llc2_addrtype);
}

static inline uchar llc2_get_lsap(llctype* llc)
{
    if (llc->llc2_addrtype == STATION_IEEE48 || llc->fDlci) /* second for fras ban */ 
	return(llc->llc2_dte[LSAP_DTE]);
    else if (llc->llc2_addrtype == STATION_FR10)
	return(llc->llc2_dte[FR_LSAP_DTE]);
    else if (llc->llc2_addrtype == STATION_PPP)
        return(llc->llc2_dte[LSAP_DTE]);
    else if (llc->llc2_addrtype == STATION_SMDS64)
	return(llc->llc2_dte[SMDS_LSAP_DTE]);
    else if (llc->llc2_addrtype == STATION_ATMVC)
	return(llc->llc2_dte[ATM_LSAP_DTE]);

    /*
     * Error case
     */
    return(0);
}	
	
static inline uchar llc2_get_rsap(llctype* llc)
{
    if (llc->llc2_addrtype == STATION_IEEE48 || llc->fDlci) /* second for fras ban */ 
	return(llc->llc2_dte[RSAP_DTE]);
    else if (llc->llc2_addrtype == STATION_FR10)
	return(llc->llc2_dte[FR_RSAP_DTE]);
    else if (llc->llc2_addrtype == STATION_PPP)
        return(llc->llc2_dte[RSAP_DTE]);
    else if (llc->llc2_addrtype == STATION_SMDS64)
	return(llc->llc2_dte[SMDS_RSAP_DTE]);
    else if (llc->llc2_addrtype == STATION_ATMVC)
	return(llc->llc2_dte[ATM_RSAP_DTE]);

    /*
     * Error case
     */
    return(0);
}	
	
static inline uchar* llc2_get_lmac(llctype* llc)
{
    if (llc->llc2_addrtype == STATION_IEEE48 || llc->fDlci) /* second for fras ban */ 
	return(&llc->llc2_dte[LMAC_DTE]);
    else if (llc->llc2_addrtype == STATION_FR10)
	return(&llc->llc2_dte[FR_DLCI_DTE]);
    else if (llc->llc2_addrtype == STATION_PPP)
        return(&llc->llc2_dte[LMAC_DTE]);
    else if (llc->llc2_addrtype == STATION_SMDS64)
	return(&llc->llc2_dte[SMDS_LMAC_DTE]);
    else if (llc->llc2_addrtype == STATION_ATMVC)
	return(&llc->llc2_dte[ATM_VCD_DTE]);

    /*
     * Error case
     */
    return(NULL);
}	
	
static inline uchar* llc2_get_rmac(llctype* llc)
{
    if (llc->llc2_addrtype == STATION_IEEE48 || llc->fDlci) /* second for fras ban */  
	return(&llc->llc2_dte[RMAC_DTE]);
    else if (llc->llc2_addrtype == STATION_FR10)
	return(&llc->llc2_dte[FR_DLCI_DTE]);
    else if (llc->llc2_addrtype == STATION_PPP)
        return(&llc->llc2_dte[RMAC_DTE]);
    else if (llc->llc2_addrtype == STATION_SMDS64)
	return(&llc->llc2_dte[SMDS_RMAC_DTE]);
    else if (llc->llc2_addrtype == STATION_ATMVC)
	return(&llc->llc2_dte[ATM_VCD_DTE]);

    /*
     * Error case
     */
    return(NULL);
}		

static inline void llc2_build_hwaddr_rmac(hwaddrtype* hwaddr, llctype* llc)
{
    uchar* address = NULL;

    if (llc->llc2_addrtype == STATION_IEEE48 || llc->fDlci) /* second for fras ban */  
	address = &llc->llc2_dte[RMAC_DTE];
    else if (llc->llc2_addrtype == STATION_FR10)
	address = &llc->llc2_dte[FR_DLCI_DTE];
    else if (llc->llc2_addrtype == STATION_PPP)
        address = &llc->llc2_dte[RMAC_DTE];
    else if (llc->llc2_addrtype == STATION_SMDS64)
	address = &llc->llc2_dte[SMDS_RMAC_DTE];
    else if (llc->llc2_addrtype == STATION_ATMVC)
	address = &llc->llc2_dte[ATM_VCD_DTE];

    snpa_init(hwaddr, llc->llc2_addrtype, TARGET_UNICAST, address);
}


#endif __LLC2_H__

