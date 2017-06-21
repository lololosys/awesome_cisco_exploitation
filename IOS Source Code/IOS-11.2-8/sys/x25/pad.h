/* $Id: pad.h,v 3.4.56.1 1996/04/02 19:24:57 swenson Exp $
 * $Source: /release/112/cvs/Xsys/x25/pad.h,v $
 *------------------------------------------------------------------
 * pad.h - Data structures for X.25/X.28/X.29  PAD
 *
 * December 1987,  Bill Westfield
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pad.h,v $
 * Revision 3.4.56.1  1996/04/02  19:24:57  swenson
 * CSCdi52237:  pad doesnt respond to set or set and read with no
 * parameters
 * Branch: California_branch
 *
 * Revision 3.4  1995/11/22  05:44:24  eschaffe
 * CSCdi42241:  PROTOCONVERT is TRUE for images that don't include the PT
 * option.
 * Obsolete PROTOCONVERT flag in makefiles.  The control for defining
 * protocolconversion for an image is now platform_specific.
 *
 * Revision 3.3  1995/11/20  21:00:32  eschaffe
 * CSCdi14369:  Cannot set a PAD profile for incoming calls to a PT
 *
 * Revision 3.2  1995/11/17  18:06:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/08  16:51:26  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.1  1995/06/07 23:21:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Number of X.3 parameters supported.  22 are in the 84 version of X.3,
 * but up to 255 are used if the extension mechanism suggested by RFCqqqq
 * is implemented at all (parameter 128 is an 'extension set', and 129-255
 * are dependent on which extension set is currently in use).
 */

#define X3_PRIVATE	0		/* extension to private params */
#define X3_RECALLCHAR	1		/* escape character */
#define X3_LOCALECHO	2		/* echo locally ? */
#define X3_DISPMASK	3		/* dispatch character mask, sort of */
#define X3_IDLETIME	4		/* dispatch time */
#define X3_FLOWOUT	5		/* output flow control ? */
#define X3_SERVICE	6		/* treatment of PAD service signals */
#define X3_BREAK	7		/* treatment of break signal */
#  define X3_BREAKINT	1		/*   xmit interrupt */
#  define X3_BREAKRES	2		/*   do RESET */
#  define X3_BREAKIND   4		/*   xmit break indication */
#  define X3_BREAKESC	8		/*   escape from data xfer */
#  define X3_BREAKDIS  16		/*   discard output */
#define X3_DISCARD	8		/* discard output */
#define X3_CRPAD	9		/* Carriage return padding */
#define X3_FOLDING	10		/* Line folding */
#define X3_SPEED	11		/* Line speed */
#define X3_FLOWIN	12		/* input flow control */
#define X3_LFINSERT	13		/* LF insertion after CR */
#define X3_LFPAD	14		/* Line feed padding */
#define X3_EDIT		15		/* Local editing */
#define X3_CHARDEL	16		/* Delete Character */
#define X3_LINEDEL	17		/* Line erase character */
#define X3_LINEDISP	18		/* Line redisplay character */
#define X3_SERVED	19		/* editing pad service signals */
#define X3_ECHOMASK	20		/* echo mask (sort of) */
#define X3_PARITY	21		/* parity treatment */
#define X3_PAGEWAIT	22		/* page size for page wait */
#define NX3PARAMETERS (22+1)

#define X3_ERRORBIT		0x80

#define pcbtype struct pcbtype_

pcbtype {
    pcbtype *next;	/* pointer to next pcb */
    lcitype *pcb_lci;	/* pointer to x25 connection (lci) block */
    conntype *pcb_conn;	/* pointer to connection block for this pad */
    ushort flags;	/* flag bits */
    int pcb_lsterr;	/* Contains error code or NULL */
    tt_soc *tty;	/* pointer to owning tty */
    uint ttynum;	/* number of owning terminal */
    paktype *curinputpak;/* pointer to current input packet, or NULL */
    paktype *curoutputpak;/* pointer to current output packet, or NULL */
    queuetype inqueue;	/* queue of packets waiting to be read */
    sys_timestamp dallytime; /* timer used for dallying */
    int state;		/* current state of the pad connection. */
    int wakeup;		/* non-zero if tiptop process should be activated */
    ulong unreadbytes;	/* count of unread data bytes  (all packets) */
    uchar paramsin[NX3PARAMETERS];	/* X.3 Parameters of foreign pad */
    uchar paramsout[NX3PARAMETERS];	/* our X.3 Parameters */
    uchar paramsorig[NX3PARAMETERS];	/* our original X.3 Parameters */

    int qinput;		/* packets queued for input */
    int qoutput;	/* total packets queued for output */

    long pakincount;	/* total datagrams received */
    long pakoutcount;	/* total datgrams sent (no retransmits) */

    long pakincontrol;	/* total input control packets */
    long pakoutcontrol;	/* total output control packets */

    long bytesincount;	/* total data bytes received */
    long bytesoutcount;	/* total data bytes sent */

};

/* bits in pcb->flags word */

#define PCB_INITED	  0x1	/* Pad did initial dialog (pad_defaults) */
#define PCB_IDLEUSER      0x2	/* set if user has idled the connection */
#define PCB_KEEPALIVE     0x4	/* set if keep alive timer is running */
#define PCB_ALIAS	  0x8	/* PCB local address is an IP alias */
#define PCB_DISCARD	 0x10	/* X3 discard output state */
#define PCB_ECHO	 0x20	/* local echoing is performed */
#define PCB_CTLREADY	 0x40	/* we have a packet with a CTRL cmd */
#define PCB_GETPARAM	 0x80	/* we need to read the remote parameters */
#define	PCB_CLEAR	0x100	/* we need to clear VC as soon as possible */
#define PCB_SETPARAM	0x200	/* accept an indicate as a set */
#define PCB_EORIN	0x400	/* x25 input packet seen without Mbit set */
#define PCB_EOROUT	0x800	/* Send and x.25 packet without Mbit set */

/* pad control message constants */

#define X3_INDICATE 0
#define X3_CLEAR 1
#define X3_SET	2
#define X3_BREAKRECEIVED 3
#define X3_READ	4
#define X3_ERROR 5
#define X3_SETANDREAD 6
#define X3_SETQUIETLY 0x82


/* Misc constants */

#define PAD_QUEUESIZE 7
#define PAD_PACKETSIZE 120

#define X29_INVITECLEAR_DEFAULT 5	/* Default time to wait for response */
					/*  to an invite-clear message */

/* conn->state values for CONN_PAD */

#define PAD_CLOSED	0x01		/* Connection is not opened */
#define PAD_CALLING	0x02		/* a call request has been sent */
#define PAD_ESTABLISHED 0x03		/* the connection is open */
#define PAD_ERROR	0x04		/* some error has occured */
#define PAD_CLEARED	0x05		/* the remote side cleared */
#define PAD_RESET	0x06		/* remote side did reset */
#define PAD_RESTART	0x07		/* dce did RESTART */
#define PAD_CLOSING	0x08		/* Sent invitation to clear */


/* codes returned by x3_readline */

#define X3_BLOCK	0		/* no characters to read */
#define X3_DONE		1		/* a dispatch char was typed */
#define X3_FULL		2		/* buffer became full */
#define X3_BREAKLINE	3		/* user typed a BREAK */

/*
 * definitions used by protocol translator.
 */

/*
 * X.3 profile definitions.
 */

#define DEFAULT_PROFILE "default"
#define PROFILE_NAMELEN 20
#define PROFILE_NPARAMS 50

struct profiletype_ {
    struct profiletype_ *next;
    char name[PROFILE_NAMELEN+1];
    int length;
    uchar params[PROFILE_NPARAMS*2];
};

/*
 * Translation data structure
 */

typedef struct tcptopadtype_ {
    struct tcptopadtype_ *next;
    ipaddrtype ipaddr;			/* (incoming) IP address */
    int tcpport;			/* (incoming) TCP port number */
    uchar x121addr[X25_ADDRESS_LENGTH]; /* (outgoing) x121 address */
    int x121addrlen;			/* address length */
    ulong cudprefix;			/* replacement for protocol id */
    uchar *cud;				/* additional call user data */
    ulong flags;			/* random flags */
    struct profiletype_ *profile;	/* initial X.3 profile to use */
    int accessclass;			/* access list */
} tcptopadtype;
;

/* codes returned by pad_acceptcall and pad_makecall */

#define PAD_FAILED      0               /* TCP or LAT Connection Failed */
#define PAD_OK          1               /* TCP or LAT Connection succeeded */
#define PAD_IN_PROGRESS 2               /* Its a translate connection,
					   let pad handle it */


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * x28.c
 */
extern boolean linemode_erasechar(tt_soc *, int);
extern boolean pad_check_access(tt_soc *, uchar *);
extern boolean pad_closing(pcbtype *);
extern boolean pad_optioncheck(conntype *, int, int );
extern boolean parse_x3pair(char *, uchar[]);
extern void pad_connect_command(parseinfo *csb);
extern conntype *pad_open_connection(idbtype *, uchar *, int *, boolean, uchar *, uint, int, boolean *);
extern int pad_telnet(conntype *, boolean);
extern int pad_unread(conntype *);
extern int padcloseBLOCK(pcbtype *);
extern int x3_getline(paktype *, uchar *, int (*mygetc)(ulong), ulong, tt_soc *);
extern lcitype *x25_open(idbtype *, pcbtype *, uchar *, uchar *, uint, int, int *);
extern pcbtype *getpcb(tt_soc *);
extern void freepcb(pcbtype *);
extern void pad_close(conntype *);
extern void pad_parse_resume(conntype *, parseinfo *);
extern void pcb_status(pcbtype *);
extern void show_padstatus(int);
conntype *pad_multiproto_open(parseinfo *, boolean *);
/*
 * x29.c
 */
extern void pad_init(subsystype *subsys);
extern boolean padBLOCK(conntype *);
extern int pad_acceptcall(lcitype *lci, uchar *cud, int cud_len);
extern boolean pad_activity(conntype *);
extern boolean pad_alive(conntype *);
extern int pad_makecall(lcitype *, int, translatetype *);
extern boolean pad_openBLOCK(pcbtype *);
extern boolean pad_outputblock(tt_soc *);
extern boolean pad_putbyte(pcbtype *, uchar, int);
extern boolean pad_putsetup(pcbtype *);
extern boolean pad_putclear(pcbtype *);
extern boolean pad_startoutput(tt_soc *);
extern boolean padputBLOCK(lcitype *);
extern boolean x25_pad_encaps(paktype *, lcitype *, boolean, int);
extern int pad_getbyte(pcbtype *, int *);
extern int pad_getc(tt_soc *);
extern int pad_getstring(pcbtype *, uchar **, long *, long);
extern void pad_putback(pcbtype *, int);
extern int pad_inputpending(tt_soc *);
extern int pad_outputpending(tt_soc *);
extern profiletype *pad_find_profile(char *);
extern void do_x3list(conntype *, uchar *, int, int );
extern void do_x3response(pcbtype *, int, int , boolean, int);
extern void pad_lci_delete(lcitype *);
extern void pad_clearcall(lcitype *, int, int );
extern void pad_clearinput(tt_soc *);
extern void pad_clearoutput(tt_soc *);
extern void pad_completecall(lcitype *);
extern void pad_control(paktype *, pcbtype *, conntype *);
extern void pad_defaults(pcbtype *);
extern void pad_defaults2(pcbtype *);
extern void pad_enqueue(hwidbtype *, paktype *, lcitype *, boolean);
extern void pad_interrupt(pcbtype *);
extern void pad_option_emit(pcbtype *, int, int , int );
extern void pad_putc(tt_soc *, int);
extern void pad_putcontrol(pcbtype *);
extern void pad_putpush(pcbtype *, boolean);
extern void pad_reset(lcitype *);
extern void pad_restart(lcitype *);
extern void pad_setsflow(tt_soc *);
extern void pad_x3init(pcbtype *, tt_soc *);
extern void padvty_init(void);
extern void print_paramlist(uchar *, int, void (*proc )(const char *, ...));
extern void print_x29pak(paktype *, int, void (*proc )(const char *, ...));
extern connvector pad_connvector;
extern int x29_inviteclear_time;
extern pcbtype *tty2pcb(tt_soc *);
extern boolean pad_enable;

/*
 * pad_chain.c
 */
extern void pad_parser_init(void);

/*
 * padname.c
 */
extern void padname_init(void);
