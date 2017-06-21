/* $Id: ttysrv.h,v 3.1 1995/11/09 09:07:04 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/ttysrv.h,v $
 *------------------------------------------------------------------
 * ttysrv.h -- terminal parameters and data structures
 *
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ttysrv.h,v $
 * Revision 3.1  1995/11/09  09:07:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:54  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

# define nVTTYs 5		/* number of pseudo-terminals */
#ifdef SMALLASM
#  define MAXLINES (10+nVTTYs+1) /* maximum possible lines */
#else
#  define MAXLINES ((16*6)+nVTTYs+4+1) /* maximum possible lines */
#endif
# define MAXPCLINES 48		/* Maximum protocol conversion lines */
# define TTY_ESCAPECHAR 036	/* default escape character - ^^ */
# define MAXINCHARS 2000	/* input buffering - one 25x80 screen */
# define MAXOUTCHARS 500	/* output buffering - one max TCP dgram */

/*
 * Seconds to wait for modem answer
 */
# define MODEMWAIT 15		/* wait 15 seconds for modem answer */
# define RINGWAIT 5		/* ignore RING transitions this long */

# define IDLE_SESSION_DEFAULT 0	/* default to no session timeout */
# define IDLE_EXEC_DEFAULT (10*ONEMIN) /* and ten minutes exec timeout */
# define LENGTH_DEFAULT 24	/* default screen length */
# define WIDTH_DEFAULT 80	/* default screen width */
# define MAXTTYLENGTH  512	/* maximum screen length */
# define MAXTTYWIDTH   512	/* maximum screen width */

# define CH_NUL 0
# define CH_SOH 1
# define CH_STX 2
# define CH_ETX 3
# define CH_EOT 4
# define CH_ENQ 5
# define CH_ACK 6
# define BELL 07		/* the bell character */
# define CH_BS 010
# define CH_HT 011
# define LINEFEED 012		/* the linefeed character */
# define CH_VT 013
# define CH_FF 014
# define RETURN 015		/* the carriage return character */
# define CH_SO 016
# define CH_SI 017
# define CH_DLE 020
# define CTRLQ 021		/* CTRL/Q, XON */
# define CH_DC2 022
# define CTRLS 023		/* CTRL/S, XOFF */
# define CH_DC4 024
# define CH_NAK 025
# define CH_SYN 026
# define CH_ETB 027
# define CH_CAN 030
# define CH_EM 031
# define CH_SUB 032
# define CH_ESC 033
# define CH_FS 034
# define CH_GS 035
# define CH_RS 036
# define CH_US 037
# define CH_DEL 0177

#define NSPECIAL 20		/* chars with special network significance */
# define NVT_IP	0		/* Interrupt Process */
# define NVT_ABORT 1		/* Abort Process */
# define NVT_EOF 2		/* EOF Signal */
# define NVT_SUSP 3		/* Suspend Process */
# define NVT_EW 4		/* erase word */
# define NVT_RP 5		/* reprint line */
# define NVT_LNEXT 6		/* Literal next - quote */
# define NVT_STOPOUT 7		/* Stop output */
# define NVT_STARTOUT 8		/* start output */
# define NVT_AO 9		/* abort output */
# define NVT_EC 10		/* Erase Character */
# define NVT_EL 11		/* Erase Line */


# define BREAK_CHAR 0x100	/* the break character -- 9th bit set! */

# define PARITY_NO	0	/* no parity on serial lines */
# define PARITY_ODD	1	/* set odd parity */
# define PARITY_EVEN	2	/* set even parity */
# define PARITY_SPACE	3	/* set space parity */
# define PARITY_MARK	4	/* set mark parity */

# define STOPBITS15	0	/* 1.5 stop bits */
# define STOPBITS1	1	/* 1 stop bit */
# define STOPBITS2	2	/* 2 stop bits */

/*
 * Terminal status bits
 */

# define TXENABLE    0x1L	/* enable UART transmitter */
# define FIRSTESC    0x2L	/* first char in escape seq was received */
# define ESCAPED     0x4L	/* full escape sequence was typed */
# define PSIENA	     0x8L	/* pseudo interrupts enabled */
# define RINGIN	     0x10L	/* Ring Indicate transition received */
# define IDLE	     0x20L	/* the modem is idle */
# define READY	     0x40L	/* the modem is ready */
# define HANGUP	     0x80L	/* user hung up modem */
				/* UNUSED */
# define CARDROP     0x200L	/* carrier dropped or vty died */
# define HASPROC     0x400L	/* a process is associated with this line */
# define LASTWASCR   0x800L	/* last character was a carriage return */
# define HOLDING     0x1000L	/* set if user typed hold character */
# define WANTXON     0x2000L	/* an XOFF was received, wait for XON */ 
# define SENTXOFF    0x4000L	/* we sent XOFF, must send XON later */
# define AUTOBAUDING 0x8000L	/* line is running a setspeed process */
# define NOBANNER    0x10000L	/* never print a logout banner */
# define IDLETIMEOUT 0x20000L	/* interactive session is very idle */
# define LASTWASBREAK 0x40000L	/* last "character" was a BREAK */
# define SLIPMODE    0x80000L	/* tty line is in SLIP mode */
# define SENDINGBREAK 0x100000L	/* we are sending a break on the line */


# define PSIFLAGS (TXENABLE+FIRSTESC+ESCAPED+PSIENA+LASTWASCR+HOLDING+WANTXON+SENTXOFF+IDLETIMEOUT+LASTWASBREAK)
# define LINESTATE (IDLE+READY+HANGUP+CARDROP+AUTOBAUDING)

/*
 * Terminal capability flags
 */

# define ENABLED     0x1L  	/* user has given privileged password */
# define WANTSLOGIN  0x2L	/* must supply password to use terminal */
# define NOTIFY	     0x4L	/* notify on new data arrival */
# define NOEXEC	     0x8L	/* never create an EXEC on this line */
# define PRIVATE     0x10L  	/* terminal settings remain over sessions */
# define TRANSPARENT 0x20L	/* enforce 8-bit binary path */
# define SFLOWIN     0x40L	/* input software flowcontrol */
# define SFLOWOUT    0x80L	/* output software flowcontrol */
# define AUTOBAUD    0x100L     /* line does full range autobaud detect */ 
# define LOGGING     0x200L	/* line receives logging output */
# define HFLOWIN     0x400L	/* input hardware flowcontrol */
# define HFLOWOUT    0x800L	/* output hardware flowcontrol */
# define MODEMIN     0x1000L	/* modem control in - passive */
# define MODEMOUT    0x2000L	/* modem control out - active */
# define MODEMCTS    0x4000L	/* CTS must be high on this line */
# define MODEMRI     0x8000L	/* modem control.  RI must be high */
# define MODEMINOUT (MODEMRI|MODEMOUT) /* behave as RI or as tty, depending */
# define MAYSLIP    0x10000L	/* tty line is allowed to do slip */
# define MUSTSLIP   0x20000L	/* tty line is always in slip mode */
# define PADDED	    0x40000L	/* this line has padding */
# define AUTOHANGUP 0x80000L	/* hang up on close */
# define REFUSEECHO 0x100000L   /* refuse remote echo */
# define REFUSESGA  0x200000L   /* refuse request to SGA */
# define REFUSEALL  (REFUSEECHO|REFUSESGA)
# define BREAKSYNC  0x400000L   /* send SYNC on break */

	/*
	 * When adding new capability bits,
	 * please update displays in ttysrv.c
	 */

# define MODEMMASK  (MODEMIN|MODEMOUT|MODEMCTS|MODEMRI)

/*
 * terminal line and printer types
 */

# define BAD_LINE 0 		/* unknown */
# define CTY_LINE 1		/* console line */
# define TTY_LINE 2		/* normal async tty line */
# define LPT_LINE 3		/* MLP2000 parallel line printer */
# define VTY_LINE 4		/* virtual terminal */
# define STR_LINE 5		/* really a string pointer */
# define AUX_LINE 6		/* cpu card 2nd port */

/*
 * tt_soc terminal data structure
 */

#define MAXASCII 128		/* size of padding array */

typedef struct tt_soc_ {
	/*
	 * General variables.  Beware: "type" must be first field in struct.
	 */
	int type;			/* terminal type code */
	int ttynum;			/* terminal number */
	struct vectortype_ *ivector;	/* input dispatch vector */
	struct vectortype_ *ovector;	/* output dispatch vector */
	ulong resetstat;		/* status word upon line reset */
        ulong dallytimer;		/* useful timer */
    	int answercountdown;		/* current modem answer count down */
	int isavedchar;			/* saved input char, or -1 if none */
        int tiptop;			/* process number of tiptop */
	mempointer conntop;		/* pointer to VTY connection */
	queuetype connhead;		/* connection list queue header */
	int nconns;			/* current connection count */
	int conncount;			/* cumulative connection count */
	int specialchars[NSPECIAL];	/* chars with special significance */

	/*
	 * Variables changed only by privileged configuration.
	 */
	int passspeed;			/* default speed for negotiation */
	int maxpassspeed;		/* max speed for negotiation */
	int rotary;			/* if ROTARY, our group number */
	int accessin;			/* access class for incoming conns */
	int accessout;			/* access class for outgoing conns */
	uint answertimeout;		/* initial modem answer timeout */
	uint exectimeout;		/* idle exec timeout interval */
	uint sessiontimeout;		/* idle session timeout interval */
	int logoutchar;			/* session disconnect character */
	int activatechar;		/* session activation character */
    	char *location;  	    	/* pointer to location string */
	char *password;			/* pointer to password string */
	char *vacantstring;		/* pointer to vacant banner string*/
	char *tty_autohost;		/* host name to connect to */
	char *tty_autocommand;		/* command to execute */
	int telnet_paksize;		/* size of packets to send */

	/*
	 * Current line parameters.  Settable by unprivileged configuration.
	 */
        ulong statbits;			/* status word */
	ulong capabilities;		/* capability word */
	int txspeed;			/* transmit speed */
	int rxspeed;			/* receive speed */
	int parity;			/* parity setting */
	int databits;			/* count of data bits */
	int stopbits;			/* count of stop bits */
	int stopchar;			/* stop output character (XOFF) */
	int startchar;			/* start output character (XON) */
	int width;			/* screen width */
	int length;			/* screen length */
	int escapechar;			/* escape character */ 
	int holdchar;	 		/* local hold character */
	ulong dispatchtimeout;		/* dispatch timeout in milliseconds */
	uchar dispatch[MAXASCII];	/* dispatch on character support */
	uchar padding[MAXASCII];	/* padding character support */
    	char *termtype;    	    	/* terminal type string */

	/*
	 * Default line parameters.  Set by privileged configuration.
	 */
        ulong def_statbits;		/* status word */
	ulong def_capabilities;		/* capability word */
	int def_txspeed;		/* transmit speed */
	int def_rxspeed;		/* receive speed */
	int def_parity;			/* parity setting */
	int def_databits;		/* count of data bits */
	int def_stopbits;		/* count of stop bits */
	int def_stopchar;		/* stop output character (XOFF) */
	int def_startchar;		/* start output character (XON) */
	int def_width;			/* screen width */
	int def_length;			/* screen length */
	int def_escapechar;		/* escape character */ 
	int def_holdchar; 		/* local hold character */
	ulong def_dispatchtimeout;	/* dispatch timeout in milliseconds */
	uchar def_dispatch[MAXASCII];	/* dispatch on character support */
	uchar def_padding[MAXASCII];	/* padding character support */
	char *def_termtype;		/* terminal type string */


	/*
	 * Variables for UART based lines
	 */
	volatile ushort *uartdata;	/* pointer to uart data register */
	volatile ushort	*uartstatus;	/* pointer to uart status register */
	volatile ushort	*board_txstatus; /* pointer to board TX status */
	volatile ushort	*board_rxstatus; /* pointer to board RX status */
	void (*rxservice)();		/* receive interrupt service */
	void (*txservice)();		/* transmit interrupt service */
	void (*txfinish)();		/* dispose of xmit buffer */
	mempointer txfinisharg;		/* arg for dispose */
	mempointer txnext;		/* next buffer to use */
	uchar *inhead;
	uchar *intail;
	uchar *instart;
	uchar *inend;
	uchar *outhead;
	uchar *outtail;
	uchar *outstart;
	uchar *outend;	
	int incount;
	int outcount;	
	int insize;			/* input buffer size in bytes */
	int outsize;			/* output buffer size in bytes */
	int totalin;			/* total characters input */
	int totalout;			/* total characters output */
	int linenoise;			/* count of bad chars when inactive */

	/*
	 * SLIP support
	 */
	queuetype slip_oqueue;		/* SLIP packet output queue */
	mempointer slip_inpak;		/* current SLIP input packet */	
	mempointer slip_outpak;		/* Current SLIP output packet */
	int slip_version;		/* version of SLIP being used */
	int slip_istate;		/* SLIP input state */
	int slip_ostate;		/* SLIP output state */
	ipaddrtype slip_address;	/* IP address of line */
	int slip_inpaks;		/* statistics - input packets */
	int slip_outpaks;		/* output packets */
	int slip_inerrs;		/* input errors */
	int slip_outdrop;		/* outputs dropped */
} tt_soc;

/*
 * Global tt_soc variables
 */
extern tt_soc *MODEMS[];	/* the array of terminal data blocks */
extern tt_soc *stdio;		/* pointer to current terminal */
extern tt_soc *console;		/* pointer to console terminal */

/*
 * Character device vector definitions.
 */

typedef struct vectortype_ {
	void (*vsetup)();	/* set up input and output buffers */
	void (*vmodemline)();	/* set up a modem line */

	int (*vgetc)();		/* character input, no wait */
	void (*vstartinput)();	/* start character input */
	void (*vclearinput)();	/* clear buffered input */
	int (*vinputpending)();	/* number of bytes buffered for input */

	void (*vputc)();	/* character output */
	boolean (*vstartoutput)(); /* start character output */
	void (*vclearoutput)();	/* clear buffered output */
	int (*voutputpending)();/* number of bytes buffered for output */

	boolean (*vsetspeed)();	/* set device speed */
	void (*vstopbits)();	/* set stop bits */
	void (*vparitybits)();	/* set parity handling */
	void (*vdatabits)();	/* set number of data bits */
	void (*vsendbreak)();	/* send a break "character" */
	void (*vdtrstate)();	/* set DTR state */
	void (*vsetflow)();	/* set hardware flow control */
	void (*vsetsflow)();	/* set software flow control */
	void (*vputs)();	/* put string */
} vectortype;

#define NVECTORS (sizeof(vectortype) >> 2)   /* number of dispatch vectors */

#define setup(tty,in,out) ((*(tty->ovector->vsetup)) (tty,in,out))
#define modemline(tty,arg) ((*(tty->ovector->vmodemline)) (tty,arg))

#define getc(tty) ((*(tty->ivector->vgetc)) (tty))
#define clearinput(tty) ((*(tty->ivector->vclearinput)) (tty))
#define startinput(tty) ((*(tty->ivector->vstartinput)) (tty))
#define inputpending(tty) ((*(tty->ivector->vinputpending)) (tty))

#define putc(tty,ch) ((*(tty->ovector->vputc)) (tty,ch))
#define puts(tty,s,l) ((*(tty->ovector->vputs)) (tty,s,l))
#define clearoutput(tty) ((*(tty->ovector->vclearoutput)) (tty))
#define startoutput(tty) ((*(tty->ovector->vstartoutput)) (tty))
#define outputpending(tty) ((*(tty->ovector->voutputpending)) (tty))

#define setspeed(tty,tx,rx) ((*(tty->ovector->vsetspeed)) (tty,tx,rx))
#define setstopbits(tty,arg) ((*(tty->ovector->vstopbits)) (tty,arg))
#define setparity(tty,arg) ((*(tty->ovector->vparitybits)) (tty,arg))
#define setdatabits(tty,arg) ((*(tty->ovector->vdatabits)) (tty,arg))
#define sendbreak(tty) ((*(tty->ovector->vsendbreak)) (tty))
#define dtrstate(tty,arg) ((*(tty->ovector->vdtrstate)) (tty,arg))
#define setflow(tty) ((*(tty->ovector->vsetflow)) (tty))
#define setsflow(tty) ((*(tty->ovector->vsetsflow)) (tty))

/*
 * Character I/O routines
 */

extern int serial_getc();
extern int serial_peekc();
extern int serial_ungetc();
extern void serial_clearinput();
extern void serial_putc();
extern void serial_puts();
extern void generic_puts();
extern void serial_clearoutput();
extern int serial_inputpending();
extern int serial_outputpending();
tt_soc *tty_allocate();
tt_soc *tty_init();
