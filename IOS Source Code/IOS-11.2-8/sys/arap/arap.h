/* $Id: arap.h,v 3.3.42.2 1996/08/28 12:35:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/arap/arap.h,v $
 *------------------------------------------------------------------
 * Header file for arap.c
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: arap.h,v $
 * Revision 3.3.42.2  1996/08/28  12:35:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.42.1  1996/04/19  14:54:19  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.8.4  1996/03/30  01:33:21  carrel
 * cvs fixes
 *
 * Revision 2.1.8.3  1996/02/28  06:14:24  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.1.8.2  1995/12/19  03:31:37  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke
 *
 * Revision 2.1.8.1  1995/11/17  16:10:55  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 * Revision 3.3  1995/12/18  04:08:09  tkolar
 * CSCdi26866:  ARAP clients with local networks dont show up in chooser
 * Change incoming NBP LkUps to have the zone '*' before passing
 * to client.
 *
 * Revision 3.2  1995/11/17  08:38:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/09  16:18:33  jjohnson
 * CSCdi38326:  quicksort should be a utility function
 * delete arap_qsort from the arap subsystem
 * add new utility function qsort to libutil.a
 *
 * Revision 2.3  1995/07/21  01:47:13  billw
 * CSCdi36156:  No AAA accounting messages for ARAP or Xremote
 *
 * Revision 2.2  1995/06/18  19:54:29  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:04:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define UNLIMITED	 	-1		/* Used for arap_timelimit */
#define TICKLETIME		20*ONESEC	/* time between tickles */
#define MAX_UNACKED_TICKLES1    3               /* no of unacked tickles for
                                                   ARAP1.0 before disconnect */
#define MAX_UNACKED_TICKLES2    12              /* no of unacked tickles for
                                                   ARAP2.0 before disconnect */
#define MT_HEADERLEN		2		/* modem tool header length */
#define WARNINGTIME		3*ONEMIN	/* time when TIMELEFT message
							is sent */

#define ZONETABLESIZE		4096		/* max number of zones we will
							send to a client */

#define MAXTEXTLENGTH		200		/* length for a text_alert */

/* arbitration packet offsets */
#define ARA_LNKARB_TYPEHIGH     0   /* never used */
#define ARA_LNKARB_TYPELOW      1
#define ARA_LNKARB_RESHIGH      2
#define ARA_LNKARB_RESLOW       3

#define ARA_LNKARB_CRR          2

/* Error types, as defined in the ARAP spec */
#define ERR_VLD8_CALLBACK		-5819
#define ERR_VLD8_BADVERSION		-5820
#define ERR_VLD8_BADUSER		-5821
#define ERR_VLD8_BADPASSWORD		-5822
#define ERR_VLD8_BADLINK		-5823
#define ERR_VLD8_NOCALLBACKALLOWED	-5824
#define ERR_VLD8_ALLCBSERVERBUSY	-5825
#define ERR_VLD8_GUESTNOTALLOWED	-5826
#define ERR_VLD8_SERVERISIMPOSTER	-5827
#define ERR_VLD8_LOGINNOTENABLED	-5828
#define ERR_VLD8_MANUALPASSWORDREQUIRED	-5850

#define ERR_CONNECTION_REFUSED          -5915

#define NO_ERR 0x0

/* access Flags from the 2.0 spec */
#define kNoGuestAccess		0x1
#define kMultiPortSrvr		0x2
#define kManualPasswordEntry	0x4

/* next Actions for ARAP 2.0 spec */
#define kContinueConnection     0x0
#define kAbortConnection        0x1
#define kRetryDES		0x2

/* packet offsets */
#define ARAPAK_DATAFLAG         0


#define KAUTH_TWOWAYDES	1

#define ARAP_LONG_SIZE 4
#define ARAP_SHORT_SIZE 2

#define ARAP_MAXZONESIZE 34
#define ARAP_MAXNAMESIZE 34
#define ARAP_MAXZONEPACKETSIZE 500

#define SMARTBUFFER_VERSION 1

/* hex 0x01008000 */
#define SRVRVERSION1_LOW0	1
#define SRVRVERSION1_LOW1	0
#define SRVRVERSION1_LOW2	128
#define SRVRVERSION1_LOW3	0

/* hex 0x02002008 */
#define SRVRVERSION2_HIGH0	2
#define SRVRVERSION2_HIGH1	0
#define SRVRVERSION2_HIGH2	32
#define SRVRVERSION2_HIGH3	8

typedef struct msg_ack_ {
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
} msg_ack;

typedef struct msg_srvrversion_ {
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar lowVersion[ARAP_LONG_SIZE];
    uchar highVersion[ARAP_LONG_SIZE];
} msg_srvrversion;

typedef struct msg_auth_request_ {
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar result[ARAP_SHORT_SIZE];
    uchar randomNumber1[ARAP_LONG_SIZE];
    uchar randomNumber2[ARAP_LONG_SIZE];
    uchar resultNumber1[ARAP_LONG_SIZE];
    uchar resultNumber2[ARAP_LONG_SIZE];
    uchar guest[ARAP_SHORT_SIZE];
    uchar userName[ARAP_MAXNAMESIZE];
} msg_auth_request;

typedef struct msg_auth_challenge_ {
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar authType[ARAP_LONG_SIZE];
    uchar randomNumber1[ARAP_LONG_SIZE];
    uchar randomNumber2[ARAP_LONG_SIZE];
    uchar accessFlags[ARAP_LONG_SIZE];  /*ARAP 2.0*/
} msg_auth_challenge;

typedef struct msg_auth_response_ {
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar result[ARAP_SHORT_SIZE];
    uchar resultnumber1[ARAP_LONG_SIZE];
    uchar resultnumber2[ARAP_LONG_SIZE];
    uchar nextAction[ARAP_SHORT_SIZE];  /*ARAP 2.0*/
} msg_auth_response;

typedef struct msg_feature_Info_ {  /* whole packet is ARAP 2.0 */
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar featureFlags[ARAP_LONG_SIZE];
    uchar minPasswordLengtth;
    uchar reserved;
    uchar numberOfZones[ARAP_SHORT_SIZE];
    uchar passwordCreateDate[ARAP_LONG_SIZE];
    uchar passwordExpireDate[ARAP_LONG_SIZE];
    uchar serverCurrentTime[ARAP_LONG_SIZE];
} msg_feature_Info;

/* ARAP 2.0 extended security packets */

typedef struct msg_ext_auth_version_ { /* whole packet is ARAP 2.0 */
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar reserved[ARAP_LONG_SIZE];
    uchar result[ARAP_SHORT_SIZE];
    uchar securityVersion[ARAP_LONG_SIZE];
} msg_ext_auth_version;

typedef struct msg_ext_auth_result_ { /* whole packet is ARAP 2.0 */
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar reserved[ARAP_LONG_SIZE];
    uchar result[ARAP_SHORT_SIZE];
} msg_ext_auth_result; 

typedef struct msg_ext_auth_load_ { /* whole packet is ARAP 2.0 */
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar reserved[ARAP_LONG_SIZE];
    uchar securityType[ARAP_LONG_SIZE];
} msg_ext_auth_load; 

typedef struct msg_ext_auth_exchange_ { /* whole packet is ARAP 2.0 */
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar reserved[ARAP_LONG_SIZE];
    uchar result[ARAP_SHORT_SIZE];
    uchar securitydatalength;
    uchar data;
} msg_ext_auth_exchange; 

typedef struct msg_ext_auth_abort_ { /* whole packet is ARAP 2.0 */
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar reserved[ARAP_LONG_SIZE];
    uchar result[ARAP_SHORT_SIZE];
} msg_ext_auth_abort; 

typedef struct msg_ext_callback_ { /* whole packet is ARAP 2.0 */
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
} msg_ext_callback; 

/* --- end of ARAP 2.0 extended security --- */

typedef struct msg_startinfofromserver_ {
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar SB_SendBufSize[ARAP_LONG_SIZE];
    uchar SB_RcvBufSize[ARAP_LONG_SIZE];
    uchar RealNet[ARAP_SHORT_SIZE];
    uchar RealNode[ARAP_SHORT_SIZE];
    uchar maxConnectTimer[ARAP_LONG_SIZE];
    uchar AppearAsNet[ARAP_SHORT_SIZE];
    uchar AppearAsNode[ARAP_SHORT_SIZE];
    uchar serversZone[ARAP_MAXZONESIZE];
    uchar serversName[ARAP_MAXNAMESIZE];
    uchar smartBufferVersionMin[ARAP_LONG_SIZE];
    uchar smartBufferVersionMax[ARAP_LONG_SIZE];
} msg_startinfofromserver;

typedef struct msg_zonelistinfo_ {
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar lastflag;
    uchar data;
} msg_zonelistinfo;

/* Out of band packets */

typedef struct msg_password_change_result_ {  /*ARAP 2.0*/
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar result[ARAP_SHORT_SIZE];
} msg_password_change_result;

typedef struct msg_pw_client_must_change_ {  /*ARAP 2.0*/
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar why[ARAP_SHORT_SIZE];
} msg_pw_client_must_change;

typedef struct msg_tickle_ {
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar theNet[ARAP_SHORT_SIZE];
    uchar timeleft[ARAP_LONG_SIZE];
} msg_tickle;

typedef struct msg_timeleft_ {
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar timeleft[ARAP_LONG_SIZE];
} msg_timeleft;

typedef struct msg_text_alert_ {
    uchar dataflag;
    uchar seqnum;
    uchar cmdhighbyte;
    uchar cmd;
    uchar string[MAXTEXTLENGTH];
} msg_text_alert;


typedef union arap_packet_ {
    msg_ack			ack;
    msg_srvrversion		srvrversion;
    msg_auth_challenge		auth_challenge;
    msg_auth_request		auth_request;
    msg_auth_response		auth_response;
    msg_startinfofromserver	startinfofromserver;
    msg_zonelistinfo		zonelistinfo;
    msg_tickle			tickle;
    msg_timeleft		timeleft;
    msg_feature_Info		feature_Info; /*ARAP 2.0*/
    msg_ext_auth_version	ext_auth_version; /*ARAP 2.0*/
    msg_ext_auth_result 	ext_auth_result; /*ARAP 2.0*/
    msg_ext_auth_load 		ext_auth_load; /*ARAP 2.0*/
    msg_ext_auth_exchange 	ext_auth_exchange; /*ARAP 2.0*/
    msg_ext_auth_abort 		ext_auth_abort; /*ARAP 2.0*/
    msg_ext_callback 		ext_callback; /*ARAP 2.0*/
    msg_text_alert 		text_alert; /*ARAP 2.0*/
    msg_password_change_result 	password_change_result; /*ARAP 2.0*/
    msg_pw_client_must_change 	pw_client_must_change; /*ARAP 2.0*/
} arap_packet;


typedef struct bits64_ {	/* this gets used by the DES routines */
    ulong high;
    ulong low;
} bits64;

typedef struct arap_authtype_ {
    struct bits64_ myrandom;
    struct bits64_ theirresult;
    struct bits64_ theirrandom;
    struct bits64_ myresult;
    uchar password[PASSWORDLEN + 1];
    uchar user[ARAP_MAXNAMESIZE];
    short resultcode;
} arap_authtype;


struct araptype_ {
    mnp4type *mnp4ptr;          /* mnp4 state information */
    v42type *v42ptr;            /* v42bis state information */
    tt_soc *tty;                /* tty associated with this line */
    usernametype *info;         /* Keep callback info */
    config_t smartbuf_conf;     /* smartbuffering configuration structure */
    ring_t *smartbuf_input;     /* smartbuffering input ring */
    ring_t *smartbuf_output;    /* smartbuffering output ring */
    uchar arap_started,         /* boolean */
     arap_seqnum,               /* outgoing packet sequence number */
     arap_acked,                /* incoming packet sequence number */
     force_quit,                /* set this to quit ARAP */
     whichQ,                    /* which queue has input on it */
     dum1, dum2, dum3;		/* Restore alignment */

    boolean password_firsttime;

    boolean packet_ready;	/* is the connection ready for packets? */

    boolean smartbuffer;        /* are we smartbuffering? */

    queuetype v42InQ, ARAPInQ,  /* queues for communicating between */
     v42OutQ, MNP4OutQ;         /* ARAP, v42bis, and MNP4 */

    queuetype ttyOUT;

    uchar text[MAXTEXTLENGTH];

    int zonecount;

    arap_authtype auth;         /* authentication info */

    sys_timestamp tickletime;		/* time stamp of last tickle */
    sys_timestamp warningtime;          /* time when warning will be given */
    sys_timestamp disconnecttime;       /* time user will be disconnected */
    sys_timestamp timestarted;          /* time connection was started */
    long last_data;			/* time last ARAP pak was received */

    long ticklecount;           /* number of tickles since last pkt received */
    long at_out_pckts;		/* total AT packets out on this connection */
    long at_in_pckts;		/* total AT packets out on this connection */
    long overflow_pckts;	/* total AT packets dropped due
					to overflow on this connection */
    int accesslist;		/* The access list for this session. */
    int zonelist;		/* The list of zone to be sent to the user */
};


/* ------ Local Function Prototypes --------------*/
static int make_zone_array(ziptabletype **, int);
static int zonecmp(const void *, const void *);
static int arap_input(araptype * );
static void arap_make_atalk(araptype * , paktype * );
static void NBPTweak(araptype * , paktype * );
static void NBPUntweak(paktype * );
/*static void arap_refuse_conn(araptype * , paktype * ); */
static int arap_startup(araptype * , paktype * );
static boolean arap_resume(araptype *); 
static int arap_send_msg(araptype *, int , int );
static int wait_for_remote(araptype * , int );
static void arap_output(araptype * );
static boolean arapstartupBLOCK(int *);
static boolean arapBLOCK(int *);
static araptype *init_arapptr(tt_soc * );
static char *arap_msg_strings(int);
static char *arap_err_strings(int);
static void destroy_arapptr(araptype *);
static int arapsec(araptype *, arap_packet *);
static int arapsec_old(araptype *, arap_packet *);
static int arapsec_new(araptype *, arap_packet *);
static int arapsec_tacacs_account(araptype *, arap_packet *);
static int arapsec_local_account(araptype *, usernametype *);
static boolean arap_restart(tt_soc *);


/* ------ mnp4, v42bis, smartbuf hack ---- */
mnp4type *get_tty_mnp4ptr(tt_soc *);
v42type *get_tty_v42ptr(tt_soc *);
ring_t *get_tty_smartbufptr(tt_soc *, int);
