/* $Id: isdn_bri.h,v 3.7.12.8 1996/08/29 22:22:00 gtaylor Exp $
 * $Source: /release/112/cvs/Xsys/les/isdn_bri.h,v $
 *----------------------------------------------------------------------
 * isdn_bri.h -- Definitions for ISDN
 *
 * December 1991, Greg Stovall
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *----------------------------------------------------------------------
 * $Log: isdn_bri.h,v $
 * Revision 3.7.12.8  1996/08/29  22:22:00  gtaylor
 * CSCdi36917: Japanese require only 2 calls to the same number in 3 mins
 * Branch: California_branch
 *
 * Revision 3.7.12.7  1996/07/19  19:48:10  wmay
 * CSCdi60728:  ISDN dialer holdQ not handled correctly
 * Branch: California_branch
 *
 * Revision 3.7.12.6  1996/07/10  02:49:12  anke
 * CSCdi56934:  Alphanumeric characters allowed in phone number field
 * Allow only #, - and numbers
 * Branch: California_branch
 *
 * Revision 3.7.12.5  1996/06/16  21:16:06  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.7.12.4  1996/05/19  03:10:10  hrobison
 * CSCdi56813:  CSCdi46413 breaks platform independent file modularity
 * Branch: California_branch
 * With the excuse of changes required by the new compiler, take the
 * opportunity to remove platform dependancies, improve code
 * modularity and do some general code clean-up.
 *
 * Revision 3.7.12.3  1996/04/27  06:33:14  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.7.12.2  1996/04/20  17:59:18  lol
 * CSCdi46005:  ISDN CLID not propagated to rem_addr field of T+ packet
 * Branch: California_branch
 *
 * Revision 3.4.2.4  1996/04/26  01:43:01  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.4.2.3  1996/04/23  00:33:34  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.4.2.2  1996/04/17  03:35:25  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.4.2.1  1996/02/23  20:51:55  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.7.12.1  1996/03/18  20:42:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.4.3  1996/03/16  07:12:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.4.2  1996/03/13  01:50:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.4.1  1996/02/20  01:09:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/15  18:02:33  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.6  1996/02/13  20:25:50  hampton
 * Move platform specific defines from common include files to platform
 * specific include files.  [CSCdi48844]
 *
 * Revision 3.5  1996/01/15  00:02:55  misko
 * CSCdi38915:  at bootup, a number of MBRI ports fail to come up NOMEMORY
 * error
 * At startup time, BRI interfaces on the MBRI seem to generate a lot of
 * layer 1 events.  These are all queued for the ISDN process, but prior
 * to getting processed, the queue overflows.  Through customer testing
 * it was determined that 1000 elements is a safe number.  So increase
 * the max queue elements allowed.  Could probably even use zero here
 * and just let it grow as big as required.
 *
 * Revision 3.4  1996/01/12  16:46:18  rbeach
 * CSCdi46101:  ISDN 4ESS Pri doesn't correctly handle outbound NSF.
 * The correct network specific facility values are now configurable.
 * We support outbound SDN, MEGACOMM and ACCUNET.
 *
 * Revision 3.3  1995/11/17  17:41:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:15:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/21  06:36:37  bdas
 * CSCdi36921:  ISDN MIB needs to contain info about active calls
 *
 * Revision 2.5  1995/10/17  22:15:34  rbeach
 * CSCdi36915:  voice calls are not handled correctly over ISDN.
 * Allow configuration for data over voice on both outgoing and
 * incoming calls. For outgoing there is a new option for the
 * dialer map command called class. For incoming there is a new
 * interface option "isdn incoming-voice data". This will allow
 * the isdn code to answer incoming voice calls.
 *
 * Revision 2.4  1995/08/12  04:18:37  bdas
 * CSCdi36875:  Need to provide a show isdn history command for call
 * history mib
 *
 * Revision 2.3  1995/08/07  20:31:33  bdas
 * CSCdi38212:  Dialing reason may be incorrect in some call history mib
 * entries
 *
 * Revision 2.2  1995/07/21  01:46:26  bdas
 * CSCdi36879:  Need to provide dialing reason in the call history mib.
 *
 * Revision 2.1  1995/06/07  21:37:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *----------------------------------------------------------------------
 * $Endlog$
 */

#ifndef ISDN_BRI_H
#define	ISDN_BRI_H

#define SERIALS_PER_BRI		2	/* number of serials per BRI */

#include "../isdn/switch_def.h"		/* T'works provided switch defs */

enum Chan { ISDN_Dchan = -1, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, B11,
		B12, B13, B14, B15, B16, B17, B18, B19, B20, B21, B22, B23,
		B24, B25, B26, B27, B28, B29, B30, B31 };

enum Enab { disable = 0, enable};   /* enable Tx and Rx on B channel */

enum Loopback {
        LOOPBACK_DISABLED = 0,       /* loopback on bri is diabled       */   
        SYSTEM_LOOPBACK_ENABLED = 1, /* loopback mode 1: system loopback */
        REMOTE_LOOPBACK_ENABLED = 2, /* loopback mode 2: remote loopback */
        PLUG_LOOPBACK_ENABLED = 3,   /* loopback mode 3: plug   loopback */
};

enum BriMsgType {
	/* messages to call control (via the isdn_command() routine) */
	ISDN_CALL, ISDN_HANGUP,

	/* messages from call control (via the isdn_report() routine,
	   internal to the isdn interface module) */
	ISDN_CONNECT, ISDN_INCOMING, ISDN_DISCON,

        /* msgs from the modem code, into call control (via isdn_command())*/
        VOICE_IDLE,
        VOICE_ANS,      /* incoming call, phone goes off hook */
        VOICE_ISDNCALL, /* phone goes off hook to make outgoing call */
        VOICE_DIGIT,    /* dialed digit */
        VOICE_HANGUP    /* done with call, local hangup, disconnect call */
};

enum activation {DEACTIVATED = 0, PENDING, ACTIVATED};

enum EncodeType {
		U_LAW_ENCODING = 0,
		A_LAW_ENCODING = 1
};

#define MAX_NUM_LEN 	80		/* Just a guess for max phone number */

/*
 * L1 primitives to be send up to isdn.
 */
typedef enum {
ISDN_PH_ACT_IND		= 0x1L,
ISDN_MPH_ACT_IND	= 0x2L,
ISDN_PH_DEACT_IND	= 0x3L,
ISDN_MPH_DEACT_IND	= 0x4L,
ISDN_MPH_EI1_IND	= 0x5L,
ISDN_MPH_EI2_IND	= 0x6L,
ISDN_MPH_CONNECT_IND	= 0x7L,
ISDN_MPH_DISCONNECT_IND	= 0x8L,
ISDN_L1_SHUT		= 0x9L,	
ISDN_L1_RESET   	= 0xaL,
} l1_prim_element_t;

/*
 * Definitions for the modem interface. Messages from ISDN stack to the
 * modem code.
 */
enum msg_for_modem {
    DEV_IDLE = 0,      /* hang up */
    DEV_INCALL,        /* incoming call */
    DEV_SETUP_ACK,     /* received Setup_Ack, start dialing */
    DEV_CALL_PROC,     /* stop dialing */
    DEV_CONNECTED      /* call is connected end to end */
};

/*
 * Definitions for the POTS BRI interface. State for the POTS driver.
 */
enum state_of_pots {
    POTS_IDLE_ST = 0,   /* 0 no voice calls active */
    POTS_OFFHOOK_ST,    /* 1 outgoing call phone offhook */
    POTS_SETUP_ACKP_ST, /* 2 outgoing call waiting for setup ack to send digs */
    POTS_RINGING_ST,    /* 3 incoming call ringing phone */
    POTS_DIALING_ST,    /* 4 outgoing call got setup ack now dialing */
    POTS_CONN_ACKP_ST,  /* 5 incoming call phone was answered sent CONNECT */
    POTS_CALL_PROC_ST,  /* 6 outgoing call done sending digits */
    POTS_CONNECTED_ST   /* 7 call is connected end to end */
};

/*
 * More # defines related to ISDN 
 */
#define ISDN_BRI_L1	0x01	/* indication of bri layer 1 source */
#define BRI_MAX_L1_PRIM	1000     /* Maximum L1 primitive elements to be
				   pre-allocated */
#define PRI_MAX_L1_PRIM	100     /* Maximum L1 primitive elements to be
				   pre-allocated */

#define NO_BUSY_CAUSE_SPECIFIED 0  /* The user is allowed to specify 
				    * cause code to return in call rejection 
				    * when no modems are available.  This value
				    * is used to indicate that no cause has
				    * been specified.
				    */
/*
 * The L1 states of the ISDN. (with slight modification). Note that,
 * on a deactivation, we go to state F2 instead of F3 as recommended
 * by ISDN, because of the National chip limitations.   
 */
typedef enum {
    l1_state_F0	= 0L,/* UNDEFINED/POWERED DOWN :we dont know anything*/
    l1_state_F1 = 1L,/* REMOTE POWER WAIT: Corresponding ISDN state is
			       F1.1. This is the state when we are powered up,
			       waiting for remote power, if loop current detect
			       is enabled. */
    l1_state_F2 = 2L,    /* DEACTIVATED & SENSING: ISDN state  F2 */
    l1_state_F3 = 3L,    /* DEACTIVATED: ISDN state F3 */
    l1_state_F4 = 4L,    /* AWAITING SIGNAL: ISDN state F4 */
    l1_state_F5 = 5L,    /* IDENTIFYING SIGNAL: ISDN state F5 */
    l1_state_F6 = 6L,    /* SYNCHRONIZED: ISDN state F6 */
    l1_state_F7 = 7L,    /* ACTIVATED: ISDN state F7 */
    l1_state_F8 = 8L,    /* LOST FRAMING: ISDN state F8 */
} l1_state_t;
 

/*
 * The L1 software element that is queued to ISDN. 
 */
typedef struct isdn_l1_primitive isdn_l1_primitive_t;
struct isdn_l1_primitive {
    struct isdn_l1_primitive *next;	/* ptr to next */
    ushort source;			/* BRI_L1, PRI_L1 */
    ushort dchan_num;			/* Unique identifier for
					   each interface. */
    l1_prim_element_t prim;		/* Which l1 primitive? */
};
			   
typedef struct call_id_string call_id_string_t;

struct call_id_string {
   uint		    call_id;		/* internal telenetworks id */
   uint		    speed;
   char 	    called_num [MAX_NUM_LEN];
   timer_t	    *carrier_timer;
   dialer_stat	    *stats;
   dialermaptype    *call_group;        /* ptr to map to dial multiple # */
   dialerdbtype     *netddb;            /* idb that originated the call */
   call_id_string_t *next;		/* for linked list */
   dialer_holdq_type *holdq_ptr;     	/* pointer to isdn hold queue */
   char             *dial_reason;     	/* dialing reason */
};

/*
 * structures to parse the LLC for incoming calls
 * v120_parameters_type contains the parameters for the V.120
 * definitions are in Q.931
 */
typedef struct v120_parameters_type {
    int		speed;			/* converted speed */
    boolean	sync;			/* Sync or async */
    boolean	header;			/* Have a V.120 header ? */
    boolean	multiframe;		/* Sending UI or IFrames */
    boolean	mode;			/* mode field */
    boolean	lli_negotiation;	/* Do we negotiate LLI ? */
    boolean	assign;			/* assign field for LLI neg. */
    uchar  	stopbits;		/* Async stop bits */
    uchar	databits;		/* Async databits */
    uchar	parity;			/* Async parity */
} v120_parameters_type;

typedef struct llc_parse_type {
    boolean	llc_present;
    boolean	llc_valid;		/* TRUE if llc parsed correctly */
    int		speed;			/* 64 or 56 */
    boolean	v120;			/* TRUE if we have V.120 call */
    v120_parameters_type v120_info;	/* V.120 parameters */
} llc_parse_type;

/*
 * Used to create a linked list of numbers dialed on ISDN. This includes a
 * timer to check that the same number isn't called more than twice in 3
 * minutes.  Required for Japanese homologation.
 */
typedef struct call_time_check call_time_check_t;

struct call_time_check {
	sys_timestamp		timer;
	char         		number [MAX_NUM_LEN];
	call_time_check_t	*next;
};

#define NTT_REDIAL_TIMEOUT	(90 * ONESEC)


/* 
 * Tacacs+ wants to know where the ISDN remote number came from before
 * including it in a T+ request. We only believe a number reported
 * directly from isdn. Everything else is just an optimistic guess. 
 */
enum rem_src {
    ISDN_SRC_UNKNOWN,
    ISDN_SRC_ISDN,
    ISDN_SRC_DIALER
};

/* ISDN idb struct */
struct isdn_bri_t_ {
   hwidbtype	*d_chan_idb;		/* Pointer to d-chan idb */

   /* This will have to be changed to 31 to support E1 */

   hwidbtype	*b_chan_idb[32];	/* Pointer to Bx-chan idb for BRI-PRI */
   enum Chan	chantype;		/* what type of channel this idb is */
   int		dsl;			/* interface number */
   boolean	primary_rate;		/* True if primary rate */
   enum Loopback loopback_mode;		/* loopback diabled or which loopback */
   ulong 	chan_num;		/* what channel no? 0, 1 or 2 */

   /* D channel specific information */
   l1_state_t current_bri_l1_state;/*SAPPHIRE*//*current layer1 state */
   
   /* B channel specific information */

   ushort	call_id;		/* internal to T'works code */
   uint		speed;			/* 64 or 56 Kb. */
   char		*remote_number;		/* # we are calling */
   enum rem_src remote_src;             /* where remote number came from */
   boolean	in_use;			/* B channel in use -- a semaphore */
   int		num_avail_b;		/* number of B's that we can use */
   call_id_string_t *call_tbl;		/* table of call_id's and strings */
   int		ces;			/* ces/tei use for dms100 */
   ushort	enctype;		/* Configured encapsulation type */
   int		last_Bchannel;		/* the last Bchannel, 23, 24 or 32 */
   char         *clear_reason;          /* call clearing reason */
   char         *clear_code;            /* call clearing code */
   ushort       disconnect_cause;       /* call disonnect cause */  
   sys_timestamp connect_time;          /* clock at the time of connect */
   char         connect_tod[33];
   char         disconnect_tod[33];
   boolean      outgoing;               /* outgoing call ? - a semaphore */
   long         history_index;          /* Call history Index value*/
   ulong        time_index;             /* Call history time */
   boolean      call_connected;         /* connected or disconnected*/
   ulong        tx_packets;             /* tx packets when call came up */
   ulong        tx_bytes;             /* tx bytes when call came up */
   ulong        rx_packets;             /* rx packets when call came up */
   ulong        rx_bytes;             /* rx bytes when call came up */
   ulong	num_screens;		/* number of calls screened */
   /* Specific to 4xxx platforms needing a master clock. */

   boolean 	master;			/* to flag if this i/f is the master */

   /* Error counters */
   ulong	d_chan_coll;/*SAPPHIRE*//* Number of d-chan collisions */

   /*
    * The below vectors are included in this so that we can have BRI
    * and PRI code included in the same image.  They are BRI routines -
    * and PRI code had stubs for them.  Now, PRI lines will have NULL vectors.
    */
   void		(*activate_line)(hwidbtype *);
   void		(*disable_bri_channel)(hwidbtype *);
   void		(*enable_bri_channel)(hwidbtype *);
   void		(*set_bri_bandwidth)(hwidbtype *, uint);
   boolean      spc;                    /* if it's a semi permanent call */
   boolean	answer_all_as_v120;
   v120_info_type *v120info;
   boolean      voice_call;             /* to make an outgoing voice call  */
   boolean      call_is_voice;          /* this call_id is a voice call    */
   boolean      voice_answer_data;      /* send incoming voice call to ISDN */
   boolean      voice_answer_modem;     /* send incoming voice call to Brasil */
   boolean      voice_answer;           /* to allow an incoming voice call */
   boolean      do_nsf;                 /* outgoing nsf support */
   ushort       nsf_type;               /* outgoing nsf value */
   boolean	leased_line;	/* if the bri interface is a leased line - no D */
   void     (*cstate_bri_leased)(hwidbtype *, int);
   call_time_check_t  *call_timers;		/* Used for NTT switch checks */
   int          modem_busy_cause;       /* Cause value to send if a call comes
					 * in that cannot be accepted because
					 * there are no modems available.
					 */
   hwidbtype * async_idb;               /* Async (modem) idb corresponding to 
                                         * this isdn channel.
                                         */
};

#define IS_BRI(idb) ((is_isdn(idb)) && (idb->isdn_info->primary_rate == FALSE))
#define IS_PRI(idb) ((is_isdn(idb)) && (idb->isdn_info->primary_rate == TRUE))

/*
 * Inline to determine if a hwidb is a leased line B channel on a BRI interface
 */
#define ISBRILEASED(hwidb) hwidb_is_bri_leased_line_inline(hwidb)
int     legal_phone_number (const char *number_str);
static inline boolean hwidb_is_bri_leased_line_inline (hwidbtype *hwidb)
{
	return((hwidb) && (hwidb->isdn_info) && (hwidb->isdn_info->leased_line));
}

extern int nbrinets;
extern int nprinets;
extern int is_pri_supp;

#define b1_chan_idb		b_chan_idb[B1]
#define b2_chan_idb		b_chan_idb[B2]

#define BRI_LBL1		0x18	/* Loopback B1 Towards Line IF */
#define BRI_LBL2		0x19	/* Loopback B2 Towards Line IF */
#define BRI_DUAL_LINE           0xFF    /* Loopback B1 & B2 Towards Line IF */
#define BRI_LBS			0x1A	/* Loopback 2B+D Towards Digtal IF */
#define BRI_LBB1		0x1C	/* Loopback B1 Towards Digital IF */
#define BRI_LBB2		0x1D	/* Loopback B2 Towards Digital IF */


				/* interface from router to ISDN */
int	isdn_fromrouter(enum BriMsgType, ushort, char *, uint, int, 
			 hwidbtype *, boolean);
void	L2D_Srq_Task(void);	/* interface from ISDN to driver */
void	isdn_cisco_check(void);	/* used by MTEX to learn when we need in */
void	isdn_init_core(hwidbtype *, int);	/* must be called before we begin executing */
void	show_statistics(void);	/* show D channel traffic statistics */
void	isdn_switchtype(SWITCH_TYPE);	/* set the switch type */
void	isdn_iqueue(hwidbtype *, paktype *);
void	isdn_carrier_timeout(void *, void *, void *);
SWITCH_TYPE isdn_getswitchtype(void);	/* return the switch type */
extern char *print_isdn_switch(SWITCH_TYPE);
void    show_isdn_pri(int);
void    isdn_memstats(void);
void    show_isdn_L2timers(void);
void    show_isdn_L3timers(void);
void    isdn_phys_ind(hwidbtype *, boolean, int);
void    Pri_Activate_L2(int);
void    CC_CHAN_Set_Chan_State(int, int, boolean, int);
boolean is_layer1_current_state_activated(hwidbtype *);

#endif
