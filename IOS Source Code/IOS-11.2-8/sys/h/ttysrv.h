/* $Id: ttysrv.h,v 3.18.4.13 1996/09/05 01:18:05 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/h/ttysrv.h,v $
 *------------------------------------------------------------------
 * ttysrv.h -- terminal parameters and data structures
 *
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ttysrv.h,v $
 * Revision 3.18.4.13  1996/09/05  01:18:05  sjackson
 * CSCdi67473:  big buffers created not trimmed (queued on TCP input with
 * rtelnet)
 * Enable coalescing of TCP input packets by default for reverse telnet
 * Branch: California_branch
 *
 * Revision 3.18.4.12  1996/08/19  18:52:12  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.18.4.11  1996/08/08  17:57:52  sjackson
 * CSCdi65288:  Modem autoconfig should always redetect and attempt to
 * lock speed
 * Branch: California_branch
 * Allow the display of the presence of modem autoconfig for a line.
 *
 * Revision 3.18.4.10  1996/08/07  08:58:36  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.18.4.9  1996/07/29  19:02:00  snyder
 * CSCdi64402:  declare arrays in lat that should be const, const
 *              save 1084 data, 68 out of image
 * Branch: California_branch
 *
 * Revision 3.18.4.8  1996/07/11  19:23:44  sjackson
 * CSCdi60440:  Unable to Allocate IP Address From Local (Non-Default) Pool
 * Branch: California_branch
 * Configuration IDB containing pool information determined from physical
 * interface when required
 *
 * Revision 3.18.4.7  1996/07/04  00:28:38  irfan
 * CSCdi61001:  Autoselect throws away characters on startup
 * Branch: California_branch
 * Add "flush-at-activation" line subcommand to the ts system.
 *
 * Revision 3.18.4.6  1996/06/12  14:26:33  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.18.4.5  1996/06/05  15:05:40  widmer
 * CSCdi57346:  Add printf style formatting to readline prompt
 * Branch: California_branch
 *
 * Revision 3.18.4.4  1996/04/16  18:52:44  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.18.4.3  1996/04/15  14:57:53  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.18.4.2  1996/03/27  10:44:04  irfan
 * CSCdi33144:  telnet should be configurable to refuse some options
 * Branch: California_branch
 * Add a bit in tty->capabilities to allow telnet/rlogin to silently
 * ignore remote host's flow control change requests.
 *
 * Revision 3.18.4.1  1996/03/18  19:37:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.12.2.4  1996/03/16  06:45:03  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.12.2.3  1996/03/09  05:05:17  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.12.2.2  1996/02/20  00:49:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.18  1996/03/07  12:09:45  billw
 * CSCdi46565:  <uses> in  <show line> doesnt include PPP, Xremote, etc
 *
 * Revision 3.17  1996/03/01  16:38:50  vandys
 * CSCdi50393:  No way to monitor TTY lines remotely
 *
 * Revision 3.16  1996/02/13  20:42:09  hampton
 * Remove platform specific dependencies from common tty header files.
 * [CSCdi48844]
 *
 * Revision 3.15  1996/02/09  02:30:43  sjackson
 * CSCdi42583:  Dispatch-character doesnt work for reverse telnet
 *
 * Revision 3.14  1996/02/07  21:14:58  widmer
 * CSCdi48343:  Remove unused #ifdef STANDALONE from parser code
 *
 * Revision 3.13  1996/01/29  00:30:14  billw
 * CSCdi13190:  Increase password characters encryted
 * Make sure that the maximum length password that can be encrypted is
 * the maximum length that will be accepted.
 *
 * Revision 3.12.2.1  1996/01/24  21:43:44  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.12  1996/01/19  20:22:16  eschaffe
 * CSCdi46961:  Vty-async: The IP address is missing in XTA_SLIPOFF
 * message.
 *
 * Revision 3.11  1996/01/12  20:04:00  sjackson
 * CSCdi44704:  modem busyout command is needed
 * Command added as an extension of the line modem autoconfigure
 *
 * Revision 3.10  1995/12/31  12:23:38  irfan
 * CSCdi46266:  unused field in tt_soc structure
 * xremote doesn't need a compression struct ptr in tt_soc. It uses
 * the one in serial context.
 *
 * Revision 3.9  1995/12/15  23:01:23  billw
 * CSCdi45506:  Save 124 bytes per tty by handling padding differently
 * and a in statbits, and maybe an instruction or two as well!
 *
 * Revision 3.8  1995/12/09  00:43:40  tkolar
 * CSCdi45302:  Need a clean interface between modem hardware and
 * commserver
 * Make it easy to claim a tty for exclusive use by a process, and add a
 * modem_busyout registry.
 *
 * Revision 3.7  1995/11/22  03:19:31  irfan
 * CSCdi41430:  Async lines may reset their configuration
 * Obituary: tty_reset_line_defaults() died today. All time favourite pain
 * in the neck.
 *
 * Revision 3.6  1995/11/20  22:23:43  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.5  1995/11/18  22:17:52  tkolar
 * CSCdi44151:  Modem autoconfig gets line speeds confused
 * Change the way that the modemcap SPD attribute is used by modem
 * configuration.
 *
 * Revision 3.4  1995/11/18  06:30:47  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Provide prototypes for routines to change the controlling tty for a
 * process.
 *
 * Revision 3.3  1995/11/17  09:13:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:24:38  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:36:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  20:59:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/15  18:53:42  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.8  1995/09/18  19:34:38  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.7  1995/09/11  21:42:45  tkolar
 * CSCdi40022:  Telnet speed is retained even after the session is closed
 * Move passspeed and maxpassspeed into the userdata struct.
 *
 * Revision 2.6  1995/07/12  23:43:59  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.5  1995/06/20 04:14:06  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.  (Should have been in initial commit,
 * but something got screwed up.)
 *
 * Revision 2.4  1995/06/14  19:22:40  vandys
 * CSCdi35265:  Should use intfc description if no line description
 * Don't reference tty->location directly; pass tt_soc to a
 * function, let it take from either tt_soc or IDB.
 *
 * Revision 2.3  1995/06/13  16:34:40  syiu
 * CSCdi35685:  Async callback rotary should be in reverse oder
 *
 * Revision 2.2  1995/06/09  23:39:44  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.1  1995/06/07  20:36:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TTYSRV_H__
#define __TTYSRV_H__

/* 
 * All platforms with or without the PT option allocate a default
 * of 5 Virtual Terminal line (VTY).
 *
 * Increasing the number of VTY lines is not allowed unless the PT option
 * is included in the software.
 *
 * MAXLINES can vary depending on the platform type.
 */

# define DEFAULT_NVTYS 5        /* Default Virtual Terminal lines */

# define TTY_ESCAPECHAR 036	/* default escape character - ^^ */
# define MAXINCHARS 2000	/* input buffering - one 25x80 screen */
# define MAXOUTCHARS 512	/* output buffering */
# define TTY_MESSAGEQ 50 	/* Size of terminal message queues */
# define TTY_ASYNCMSGQ 50	/* Size of queue allocated for async msg */
# define DEF_TCP_COALESCE 20	/* Size of TCP input queue for coalescing */

/* for logging synchronous default */
# define LOG_SYNC_DEF_SIZE   20 /* Default size for synchronous log messages */
# define LOG_SYNC_DEF_LEVEL   2 /* Default severity level for async errmsg */
# define LOG_SYNC_IS_UNLOCK   0 /* Can print message synchronously */
# define LOG_SYNC_IS_LOCK     1 /* Cannot print message asynchronously */
# define LOG_SYNC_IS_REQ      2 /* Request for LOG_SYNC_DEF_LOCK */
# define LOG_SYNC_IS_USED     3 /* Asynchronous message has been printed */
# define LOG_SYNC_RPRNT_REQ      (1<<3)  /* Request reprint */
# define LOG_SYNC_RPRNT_GRANTED  (1<<4)  /* Acknowledged reprint */
# define LOG_SYNC_RPRNT_BUFFER   1       /* Prompt is stored in buffer */
# define LOG_SYNC_RPRNT_READLINE 2       /* use readline() style reprint */
# define LOG_SYNC_RPRNT_BUFSIZE 80       /* Size for reprint buffer */

# define LOG_SYNC_LOCK(TTY)     TTY->log_sync_lock = LOG_SYNC_IS_LOCK
# define LOG_SYNC_UNLOCK(TTY)   TTY->log_sync_lock = LOG_SYNC_IS_UNLOCK
# define LOG_SYNC_USED(TTY)     TTY->log_sync_lock = LOG_SYNC_IS_USED
# define LOG_SYNC_REQUEST(TTY)  \
         if (TTY->log_sync_lock == LOG_SYNC_IS_UNLOCK) \
	     TTY->log_sync_lock = LOG_SYNC_IS_LOCK; \
	 else if (TTY->log_sync_lock == LOG_SYNC_IS_USED ) \
             TTY->log_sync_lock = LOG_SYNC_IS_REQ

# define LOG_SYNC_REPRINT_REG(TTY,MSG) \
         if (MSG) { \
	     strcpy(TTY->log_sync_reprint_msg, MSG); \
	     TTY->log_sync_reprint_type = LOG_SYNC_RPRNT_BUFFER; \
	 }

# define LOG_SYNC_REPRINT_END(TTY) \
         TTY->log_sync_reprint_type = 0

/* Starting autobaud speed */
# define AUTOSPEED (2400)	 /* For standard autobaud */
# define AUTOSPEEDHIGH (19200)	 /*  ...for reliable 19.2K autobaud */

#define ASYNC_MINSPEED 50        /* Min speed on async lines */
#define ASYNC_MAXSPEED 115200    /* Max speed on async lines */
#define ASYNC_DEFSPEED 9600      /* Default speed on async lines */

# define MDMDBG if (modem_debug) buginf

/*
 * Seconds to wait for modem answer
 */
# define MODEMWAIT 15		/* wait 15 seconds for modem answer */
# define RINGWAIT 5		/* ignore RING transitions this long */

# define IDLE_SESSION_DEFAULT 0	/* default to no session timeout */
# define SESSIONTIMEOUT_WARNING_DEFAULT 0 /* default session timeout warning */
# define SESSIONTIMEOUT_WARNING_MIN 10	/* min value to be sure warning occurs*/
					/* before the timeout */
# define IDLE_EXEC_DEFAULT (10*ONEMIN) /* and ten minutes exec timeout */
# define ABSOLUTE_TIMEOUT_DEFAULT 0  /* Default to no absolute timeout */
# define LOGOUT_WARNING_DEFAULT 20 /* notification sent to line countdown */
# define MAX_TTY_OUTPUT_WAIT 10 /* Wait-secs, for output to finish on tty */
                                /* Before clearing tty as in death */
# define LENGTH_DEFAULT 24	/* default screen length */
# define WIDTH_DEFAULT 80	/* default screen width */
# define MAXTTYLENGTH  512	/* maximum screen length */
# define MAXTTYWIDTH   512	/* maximum screen width */

# define HISTORY_SIZE_DEFAULT	10	/* default history size */
# define MAX_HISTORY_SIZE	256	/* maximum history size */

# define CH_NUL 0			/* ^@ */
# define CH_SOH 1			/* ^A */
# define CH_STX 2			/* ^B */
# define CH_ETX 3			/* ^C */
# define CH_EOT 4			/* ^D */
# define CH_ENQ 5			/* ^E */
# define CH_ACK 6			/* ^F */
# define BELL 07			/* the bell character */
# define CH_BS 010			/* ^H */
# define CH_HT 011			/* ^I (TAB) */
# define LINEFEED 012			/* the linefeed character */
# define CH_VT 013			/* ^K */
# define CH_FF 014			/* ^L */
# define RETURN 015			/* the carriage return character */
# define CH_SO 016			/* ^N */
# define CH_SI 017			/* ^O */
# define CH_DLE 020			/* ^P */
# define CTRLQ 021			/* CTRL/Q, XON */
# define CH_DC2 022			/* ^Q */
# define CTRLS 023			/* CTRL/S, XOFF */
# define CH_DC4 024			/* ^T */
# define CH_NAK 025			/* ^U */
# define CH_SYN 026			/* ^V */
# define CH_ETB 027			/* ^W */
# define CH_CAN 030			/* ^X */
# define CH_EM 031			/* ^Y */
# define CH_SUB 032			/* ^Z */
# define CH_ESC 033			/* ^[ */
# define CH_FS 034
# define CH_GS 035
# define CH_RS 036
# define CH_US 037
# define CH_BREAKPOINT 037	/* the breakpoint character */
# define CH_SPACE 040			/* Space */
# define CH_DEL 0177
# define CH_UNDEFINED -1	/* character not being used */

/*
 * Special editing characters.  There are characters that can be set by
 * "line mode" telnet, and the values are the same values used in the
 * Telnet "Set Local Character" command.  Connection blocks have a
 * parrallel set of flags further describing the actions
 * that occur when a particular character is typed.
 */

#define NSPECIAL 25		/* chars with special network significance */
# define NVT_EC2	  0	/* Secondary Erase char (cisco specific) */
# define NVT_SYNCH        1	/* Synch Procedure for telnet */
# define NVT_BRK          2	/* BREAK character */
# define NVT_IP           3	/* Interrupt Process */
# define NVT_AO           4	/* abort output */
# define NVT_AYT          5	/* Are You There */
# define NVT_EOR          6	/* End of Record */
# define NVT_ABORT        7	/* Abort Process */
# define NVT_EOF          8	/* EOF Signal */
# define NVT_SUSP         9	/* Suspend Process */
# define NVT_EC          10	/* Erase Character */
# define NVT_EL          11	/* Erase Line */
# define NVT_EW          12	/* erase word */
# define NVT_RP          13	/* reprint line */
# define NVT_LNEXT       14	/* Literal next - quote */
# define NVT_XON         15	/* start output */
# define NVT_XOFF        16	/* Stop output */
# define NVT_FORW1       17	/* Extra forwarding character 1 */
# define NVT_FORW2       18	/* Extra forwarding character 2 */	 
    /* Room for a couple more characters */
# define NVT_MAX 18		/* maximum character defined by telnet */
# define NVT_EL2	 24	/* Secondary erase line (cisco specific) */



# define BREAK_CHAR 0x100	/* the break character -- 9th bit set! */
# define ESCAPED_FLAG 0x200	/* escaped chars, 10th bit set... */

# define PARITY_NO	0	/* no parity on serial lines */
# define PARITY_ODD	1	/* set odd parity */
# define PARITY_EVEN	2	/* set even parity */
# define PARITY_SPACE	3	/* set space parity */
# define PARITY_MARK	4	/* set mark parity */

# define STOPBITS1	1	/* 1 stop bit */
# define STOPBITS2	2	/* 2 stop bits */
# define STOPBITS15	3	/* 1.5 stop bits */


/*
 * Terminal status bits number 1
 */

# define DIALOUT     0x1L	/* running dial-out process */
# define FIRSTESC    0x2L	/* first char in escape seq was received */
# define ESCAPED     0x4L	/* full escape sequence was typed */
# define PSIENA	     0x8L	/* pseudo interrupts enabled */
# define RINGIN	     0x10L	/* Ring Indicate transition received */
# define IDLE	     0x20L	/* the modem is idle */
# define READY	     0x40L	/* the modem is ready */
# define HANGUP	     0x80L	/* user hung up modem */
# define CONNECTED   0x100L	/* User currently running a connection */
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
# define CALLBACK_IN_PROGRESS  0x200000L /* Callback on this line */
# define LASTWASSPEC  0x400000L /* last character was preceded by escape */
# define SENDXOFF     0x800000L /* Send an XOFF as soon as possible */
# define CARDCONSOLE  0x1000000L /* interface console/debug */
# define STOPPEDINPUT 0x2000000L /* We have set RTS low to stop input */
# define SENDBREAK   0x4000000L	 /* Send break as soon as possible */
# define CONTROLC    0x8000000L  /* a control-c was hit, used for setup */	
# define CTRLC_ENBL  0x10000000L /* allow ctrl-c checking */
# define PPPMODE     0x20000000L /* HW PPP support active */
# define VTYASYNC_LINE_STATE 0x40000000L /* vtyasync line proto is UP/DOWN */
# define MODEM_DETECTED 0x80000000L /* We know that a modem is attached */

/*
 * Terminal status bits number 2
 */
# define CTS_MODEM_DETECT_2 0x1L  /* Last state of CTS on an IDLE line */
# define MODEM_CONFIGURED_2 0x2L  /* The modem has been configured */
# define MODEM_AUTOCONFING_2 0x4L  /* The modem is being configured */
# define MODEM_SPEED_LOCKED_2 0x8L /* line speed/modem DTE has been locked */
# define BUSYOUT_2          0x10L /* modem is currently busyout */

/*
 *  Macros for reserving lines for use.
 */
#define  CLAIM_TTY(tty) (tty)->statbits |= HASPROC
#define  RELEASE_TTY(tty) (tty)->statbits &= ~HASPROC
#define  TTY_IN_USE(tty) ((tty)->statbits & HASPROC)

/*
 *  Macro for getting platform default capabilities.
 */
#define TTY_PLATFORM_DEF_CAPABILITIES(tty) (platformdef_capabilities[(tty)->type])

/*
 * The following is a set of bits in the status word that are dangerous
 * to clear (when a line config is reset, for example), since they affect
 * how we are going to treat the outside world.  Therefore, they should never
 * be cleared gratuitously.
 */

# define STATBITS_PERM (SENTXOFF|WANTXON|HASPROC|AUTOBAUDING|STOPPEDINPUT)

#define MAX_FILE_SYS_DEVICE_ID_LEN 32

/*
 * define canonical modem control signal bits
 */

typedef unsigned int modem_signal;

typedef struct {
    modem_signal cts : 1;	/* Always the CTS signal */
    modem_signal ring : 1;	/* Ring or DSR or CD */
    modem_signal dtr : 1;	/* Always the DTR signal */
    modem_signal rts : 1;	/* Always the RTS signal */
    /*    modem_signal dsr : 1;	 Unused at present */
    /*    modem_signal cd : 1;	 Unused at present */
} modem_status;

# define PSIFLAGS (FIRSTESC+ESCAPED+PSIENA+LASTWASCR+HOLDING+WANTXON+SENTXOFF+IDLETIMEOUT+LASTWASBREAK)
# define LINESTATE (IDLE+READY+HANGUP+CARDROP+AUTOBAUDING)
# define LINESTATE_2 (MODEM_CONFIGURED_2+BUSYOUT_2)

/*
 * Support for the old (pre-AAA) authentication routines is still available.
 */

/* OLD_AUTH -- remove next two blocks when old authentication goes away */
/*
 * Access control types
 */
#define	AC_NONE		0
#define	AC_PASSWORD	1
#define	AC_TACACS	2
#define AC_LOCAL	3

/*
 * ARAP access control types (bitmask)
 */
#define AAC_LOCAL	0
#define AAC_TACACS	1
#define AAC_SINGLELINE	2

/*
 * Line Rotary selection 
 */
#define ROTARY_ASCENDING  0            /* Select rotary in ascending order */
#define ROTARY_DESCENDING 1            /* Select rotary in descending order */

/*
 * Access control flags
 */
#define ACF_RELOGIN 0x1L		/* go back to login, don't hangup */
#define ACF_CALLBACK_NOVERIFY 0x2L	/* Don't login on callback */
#define ACF_CALLBACK_ROTARY 0x4L	/* Callback rotary access */
#define ACF_CALLBACK_EXEC 0x8L		/* Callback exec */
#define ACF_CALLBACK_PPP 0x10L		/* Callback PPP  */
#define ACF_CALLBACK_ARAP 0x20L		/* Callback ARAP */
#define ACF_CALLBACK_MASK (ACF_CALLBACK_EXEC|ACF_CALLBACK_PPP|ACF_CALLBACK_ARAP)

/*
 * Terminal capability flags for first capabilities mask
 */

# define NO_FLUSH_ACTIVATE  0x1L/* Don't clear input on activation */
# define REMOTE_MOD  0x2L       /* Remote Modification allowed */
# define NOTIFY	     0x4L	/* notify on new data arrival */
# define NOEXEC	     0x8L	/* never create an EXEC on this line */
# define PRIVATE_TTY 0x10L  	/* terminal settings remain over sessions */
# define TRANSPARENT 0x20L	/* enforce 8-bit binary path */
# define SFLOWIN     0x40L	/* Honor XON/XOFF in input data stream */
# define SFLOWOUT    0x80L	/* Send XON/XOFF in output stream */
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
/*
 ******* AVAILABLE BIT ********
# define ttycap_unused	    0x40000L
*/
# define AUTOHANGUP 0x80000L	/* hang up on close */
# define REFUSEECHO 0x100000L   /* refuse remote echo */
# define REFUSESGA  0x200000L   /* refuse request to SGA */
# define REFUSEALL  (REFUSEECHO|REFUSESGA)
# define BREAKSYNC  0x400000L   /* send SYNC on break */
# define MODEMDTR   0x800000L	/* raise DTR when a connection exists */
# define LOCKABLE  0x1000000L	/* terminal may issue LOCK command */
# define BREAKONIP 0x2000000L	/* convert an IP to a BREAK */
# define OUTIDLE   0x4000000L	/* count output as making line non-idle */
# define NOEXECBANNER 0x8000000L /* No login banner */
# define INSECURE  0x10000000L	/* Terminal is insecure */
# define AUTOHIGH  0x20000000L	/* Support reliable 19.2K autobaud */
# define MAYARAP   0x40000000L  /* tty line can run arap */
# define MUSTARAP  0x80000000L  /* tty line always runs arap */

/*
 * Terminal capability flags for capabilities2
 *
 *   These _must_ retain the "_2" suffix.
 */
# define IPONBREAK_2    0x1L    /*convert a BREAK to a telnet IP */
# define MA_DISCOVERY_2	0x2L	/* Modem autoconfigure, in discovery mode */
# define MA_FIXED_2	0x4L    /* Modem autoconfigure, with a fixed type */
# define NO_MODEM_ERRCNTRL_2	0x8L  /* don't set modem for error control */
# define NO_MODEM_COMPRESS_2	0x10  /* don't set modem to compress */
# define MODEM_BUSYOUT_2	0x20L /* modem is set to busyout */
# define BUSYOUT_OVERRIDE_2	0x40L /* modem uses alternative busyout */
# define FLOWPERMANENT_2        0x80L /* SW flow control locked */
# define NTP_CLOCK_2		0x100L /* Run an NTP reference clock */
# define PPS_CLOCK_2		0x200L /* NTP clock has PPS on dataset line */
# define RCV_SUPPR_CLOCK_2	0x400L	/* NTP clock suppressing receive */
# define NTP_BITS_2 (NTP_CLOCK_2 | PPS_CLOCK_2 | RCV_SUPPR_CLOCK_2)
# define NOMOTDBANNER_2	0x800L		/* No motd banner */
# define UNUSED13_2	0x1000L
# define UNUSED14_2	0x2000L
# define UNUSED15_2	0x4000L
# define UNUSED16_2     0x8000L
# define UNUSED17_2	0x10000L
# define UNUSED18_2	0x20000L
# define UNUSED19_2	0x40000L
# define UNUSED20_2	0x80000L
# define UNUSED21_2	0x100000L
# define UNUSED22_2	0x200000L
# define UNUSED23_2	0x400000L
# define UNUSED24_2	0x800000L
# define UNUSED25_2	0x1000000L
# define UNUSED26_2	0x2000000L
# define UNUSED27_2	0x4000000L
# define UNUSED28_2	0x8000000L
# define UNUSED29_2	0x10000000L
# define UNUSED30_2	0x20000000L
# define UNUSED31_2	0x40000000L
# define UNUSED32_2	0x80000000L

/*
 * Index into Terminal Capabilities array
 */
#define FIRST_CAP       1       /* First Capabilities field  */
#define SECOND_CAP      2       /* Second Capabilities field */


	/*
	 * When adding new capability bits,
	 * please update displays in ttysrv.c
	 */

# define MODEMMASK  (MODEMIN|MODEMOUT|MODEMCTS|MODEMRI)

/*
 * Definitions for new form of modem control
 */

/*
 * Modem types (in tty->modem_type)
 */
#define MODEMTYPE_NONE	   0	/* Line does not implement modem control */
#define MODEMTYPE_LOCAL    1	/* Modem control is not enabled */

/*
 * The following modem types are not yet defined
 */

#define MODEMTYPE_DIALIN   2	/* like old modem ri-is-cd, sort of. */
#define MODEMTYPE_DIALOUT  3	/* no previous equivilent */
#define MODEMTYPE_DIALBOTH 4	/* like modem inout */
#define MODEMTYPE_HOST     5	/* like old modem callout */
#define MODEMTYPE_OLD      6	/* like old modem callin */
#define MODEMTYPE_LIGHTS   7	/* like old modem dtr-active */
#define MODEMTYPE_CTS      8	/* like old modem cts-required */
#define MODEMTYPE_PRINTER  9	/* like modem-cts, but with DSR */
/*
 * Modem states (in tty->modem_state)
 */
#define MODEMSTATE_IDLE    0	/* idle, waiting for activity */
#define MODEMSTATE_READY   1	/* ready */
#define MODEMSTATE_HANGUP  2	/* hanging-up (dropping DTR, closing conns) */
#define MODEMSTATE_CARDROP 3	/* carrier drop (by modem) */
#define MODEMSTATE_CONDROP 5	/* connection drop (by conntop) */
#define MODEMSTATE_DIALING 6	/* dialing (no carrier yet) */
#define MODEMSTATE_WAITING 7	/* mandatory idle time after use. */
#define MODEMSTATE_RINGING 8	/* RING seen on old-style modem */
#define MODEMSTATE_ACTIVEI 9	/* Active/ready incoming connection */
#define MODEMSTATE_ACTIVEU 10	/* Active/ready user (exec) */
#define MODEMSTATE_ACTIVEC 11	/* Active/ready user with connections */

/*
 * Modem status bits (in tty->modem_bits)
 * Note that rts/cts are not treated as modem control signals when they
 * are used for hardware flow control.  Also note that not all platforms
 * implement all signals. (eg csc-16 doesn't support RTS.)
 */
#define MODEMBIT_CD	0x01	/* Carrier input */
#define MODEMBIT_CTS	0x02	/* CTS input */
#define MODEMBIT_DTR	0x04	/* DTR output */
#define MODEMBIT_RTS	0x08	/* RTS output */

#define MODEMBIT_CDX	0x10	/* Carrier transition */
#define MODEMBIT_CTSX	0x20	/* CTS Transition */

/*
 * terminal line and printer types
 */

# define BAD_LINE 0 		/* unknown */
# define CTY_LINE 1		/* console line */
# define TTY_LINE 2		/* normal async tty line */
# define VTY_LINE 3		/* virtual terminal */
# define STR_LINE 4		/* really a string pointer */
# define AUX_LINE 5		/* cpu card 2nd port */


/*
 * tt_soc terminal data structure
 */

#define MAXASCII 128		/* size of padding array */

/*
 * tty_editdata contains data used by the user interrface, as oppsed
 * to the actual tty driver.  The structure also gets used in connection
 * blocks and the like.  Note that characters in the "special" array
 * should also be set in the dispatch array.  This is an efficiency hack. 
 * A possible furture improvement is to encode the meaning of each character
 * into the dispatch array.
 */

struct tty_editdata_ {
	boolean noecho;			/* echo is off if set */
	boolean blockflag;		/* Block or return when no input ? */
	boolean editflag;		/* do local line mode w /editing */
	int special[NSPECIAL];		/* special characters */
	uchar dispatch[MAXASCII];	/* dispatch characters */
};

/*
 * Meaning of bits in the dispatch array.
 */
#define TTY_DISP_DISP 0x80		/* Simple dispatch character */
#define TTY_DISP_EDIT 0x3F		/* Some special editing character */


/*
 * parameter changeable by the user are kept in a separate data structure
 * This way, you can save storage of the user doesn't actually change them.
 * Note that there are no strings in here.  This is because duplicating
 * a string would be messy.  The only one that belongs here is ttytype.
 */
typedef struct tty_userdata_ {
	int u_txspeed;			/* transmit speed */
	int u_rxspeed;			/* receive speed */
	int u_parity;			/* parity setting */
	int u_databits;			/* count of data bits */
	int u_stopbits;			/* count of stop bits */
	int u_stopchar;			/* stop output character (XOFF) */
	int u_startchar;		/* start output character (XON) */
	int u_width;			/* screen width */
	int u_length;			/* screen length */
	int u_escapechar;		/* escape character */ 
	int u_holdchar; 		/* local hold character */
	ulong u_dispatchtimeout;	/* dispatch timeout in milliseconds */
	tty_editdata u_editinfo;	/* editing character info */
	uchar *padding_ptr;		/* padding character support */
	ulong u_transport_pref;		/* Preferred transport protocol */
	uchar u_lat_groupmask[33];	/* LAT group code mask */
	uchar u_datamask;		/* mask for connection data xmit */
	uchar u_specialmask;		/* mask for special character checks */
        uchar u_execmask;               /* mask for typein at exec level */
	boolean u_dns;                  /* True if name resolution on */
        boolean u_historyon;		/* TRUE if saving history */
	uint u_historysize;		/* size of history buffer */
	boolean u_editingon;		/* TRUE if doing line editing */
	boolean u_help;			/* TRUE if doing PRIV_USER_HELP */
/* OLD_AUTH -- remove next two lines when old authentication goes away */
	ipaddrtype u_user_server;       /* tacacs server for user authen. */
	ipaddrtype u_slip_server;       /*  "  " for SLIP addr authen. */
	int u_ip_netmask_format;	/* how to display masks */
	int u_tcp_coalesce_size;	/* number of TCP packets to coalesce */
	int u_passspeed;		/* default speed for negotiation */
	int u_maxpassspeed;		/* max speed for negotiation */
	/* The following are values that the user can't set themselves. */
	uint u_absolutetimeout;		/* absolute timeout interval */
	uint u_sessiontimeout;		/* idle session timeout interval */
	uint u_exectimeout;		/* idle exec timeout interval */
} tty_userdata;

#define tty_txspeed current.u_txspeed
#define tty_rxspeed current.u_rxspeed
#define tty_parity current.u_parity
#define tty_databits current.u_databits
#define tty_stopbits current.u_stopbits
#define tty_stopchar current.u_stopchar
#define tty_startchar current.u_startchar
#define tty_width current.u_width
#define tty_length current.u_length
#define tty_escapechar current.u_escapechar
#define tty_holdchar current.u_holdchar
#define tty_dispatchtimeout current.u_dispatchtimeout
#define tty_editinfo current.u_editinfo
#define tty_transport_pref current.u_transport_pref
#define tty_lat_groupmask current.u_lat_groupmask
#define tty_datamask current.u_datamask
#define tty_specialmask current.u_specialmask
#define tty_dns current.u_dns
#define tty_execmask current.u_execmask
#define	tty_historyon current.u_historyon
#define	tty_historysize current.u_historysize
#define	tty_editingon current.u_editingon
/* OLD_AUTH -- remove next two lines when old authentication goes away */
#define tty_user_server current.u_user_server
#define tty_slip_server current.u_slip_server
#define	tty_help current.u_help
#define tty_ip_netmask_format current.u_ip_netmask_format
#define tty_tcp_coalesce_size current.u_tcp_coalesce_size
#define tty_passspeed current.u_passspeed
#define tty_maxpassspeed current.u_maxpassspeed
#define tty_absolutetimeout current.u_absolutetimeout
#define tty_sessiontimeout current.u_sessiontimeout
#define tty_exectimeout current.u_exectimeout
#define conncount usagecount

typedef struct texti_history_struct_ {
    char **buffer;
    int  current;
    int  length;
    int  counter;
    int  size;
    int  used;
    char historyon;
} texti_history_struct;

typedef struct texti_struct_ texti_struct;
typedef int (*texti_function)(texti_struct *);
typedef void (*rxserv_t)(tt_soc *, uchar *, int);
typedef uint (*txserv_t)(tt_soc *, uchar *, int);

struct	texti_struct_ {
    char   flags;		/* Editing flag keeping track of killing
				   and yanking and escaping */
    char   word_sep[10];	/* Word delimiters for M-f, M-b, etc */

    char   old_buffer[257];	/* old display line  */
    int    old_cursor;		/* old cursor position */
    char   new_buffer[257];	/* new display line */

    char   buffer[257];		/* buffer that is being edited          */
    char   *input_buffer;	/* where to put buffer before returning */

    char   *start;		/* points to char after last char in    */
                                /*    head buffer, always == '\0'     */
    char   *end;		/* points to first char in tail buffer  */
    const char *prompt_fmt;	/* pointer to the prompt format */
    va_list prompt_args;	/* pointer to the prompt args */
    uint   prompt_len;		/* length of printed prompt */

    int    input_char;		/* Current input character */
    uint   length;		/* Current length of input line */
    int    max_length;		/* Maximum length of input line */
    int    where;		/* Current cursor position */
    char   *left_edge;		/* First visible character on screen */
    int    width;		/* Width of screen */
    int    scroll_jump;		/* Characters to move when scrolling */

    char   kill_buffer[257];	/* Current kill buffer */
    char   *kill_pointer;	/* Pointer into insert point in kill buffer */
    char   **kill_ring;		/* Array of kill buffers */
    int    kill_current;	/* Index into kill ring rotate yank */
    int    kill_length;		/* Length of kill ring */
    int    kill_max;		/* Maximum length of kill ring */

    texti_history_struct exec_history;	/* History during exec mode */
    texti_history_struct config_history;/* History during config mode */
    texti_history_struct *history;	/* Pointer to current history
					   buffer (exec or config) */

    const texti_function *main_functions;	/* Pointer to regular keymappings */
    const texti_function *escape_functions;	/* Pointer to escaped keymappings */
};

typedef struct tty_priv_ {
    uint cur_priv : 4;		/* current tty privilege level */
    uint def_priv : 4;		/* default tty privilege level */
    uint PACKED(need_to_set:1);	/* does current privilege level need
				   to be set? cleared when is set */
    uint PACKED(pad:7);		/* free bits */
} tty_priv;

/*
 * Access to tty privilege level
 */
#define SET_PRIV(priv,level)\
    (priv).cur_priv = level;\
    (priv).need_to_set = FALSE
#define SET_DEFPRIV(priv,level) (priv).def_priv = level
#define CUR_PRIV(priv)		(priv).cur_priv
#define CUR_DEFPRIV(priv)	(priv).def_priv
#define NEED_TO_SET_PRIV(priv)	(priv).need_to_set
#define SET_NEED_TO_SET_PRIV(priv) (priv).need_to_set = TRUE


struct tt_soc_ {
	/*
	 * General variables.  Beware: "type" must be first field in struct.
	 */
	int type;			/* terminal type code */
	int ttynum;			/* terminal number */
/* OLD_AUTH -- remove next two lines when old authentication goes away */
	char *user;			/* remember username */
	int uuid;			/* user id number */
	userstruct *ustruct;		/* NEW user structure */
	struct vectortype_ *ivector;	/* input dispatch vector */
	struct vectortype_ *ovector;	/* output dispatch vector */
	struct vectortype_ *nvector;	/* "native" dispatch vector */
        sys_timestamp dallytimer;	/* useful timer */
        sys_timestamp callbacktimer;	/* callback timer */
        sys_timestamp modem_checked;	/* last time we looked for a modem */
        ulong callback_forcewaittime;	/* callback force wait time */
        ulong callback_wait_nodsr;      /* wait time for DSR to go down */
    	int answercountdown;		/* current modem answer count down */
	int isavedchar;			/* saved input char, or -1 if none */
        int tiptop;			/* process number of tiptop */
	conntype *conntop;		/* pointer to VTY connection */
	queuetype connhead;		/* connection list queue header */
	int nconns;			/* current connection count */
	int usagecount;			/* usage (conn, ppp, etc) count */
	const char *automore_header;	/* Pointer to automore header */
	int automore_count;		/* Current line number on page */
	int automore_supress;		/* Recursion supression count */
	boolean automore_enabled;	/* TRUE if automore enabled */
	boolean automore_flush;		/* TRUE if flushing output */
	queuetype messageQ;		/* Queue of messages logging to show */
	int message_offset;		/* Byte offset into current message */
	queuetype asyncmsgQ;		/* Queue for asynchronous messages */
	int asyncmsg_offset;	        /* Byte offset into asyncmsgQ */
	conntype *currentcon;		/* Current connection */
	parseinfo *csb;			/* csb ptr for exec session */
	sys_timestamp time_activated;   /* last time start_process was run */
	boolean notified;               /* been notified of disconnect? */
	uint snmp_if_index;		/* snmp i/f index if registered or 0 */
	uint autoselect_sample;		/* buffer for the last few bytes of
						input on tty */

	/*
	 * Variables changed only by privileged configuration.
	 */
	int autoselectmask;		/* autoselect information */
	int rotary;			/* if ROTARY, our group number */
	int accessin;			/* access class for incoming conns */
	int accessout;			/* access class for outgoing conns */
	int accessout_default;		/* output access class default */
/* OLD_AUTH -- remove the next 3 lines when old authentication goes away */
	int actype;			/* access control type */
	uint acflags;			/* access control flags */
	int arap_actype;		/* ARAP access control type */
	char login_acc_list_name[32];	/* NEW access control methods */
	char arap_acc_list_name[32];	/* NEW access control methods */
	char nasi_acc_list_name[32];	/* NEW access control methods */
	uint answertimeout;		/* initial modem answer timeout */
	uint currenttimeout;		/* time in which we'll time out */
	uint logoutwarning;             /* warning countdown for absolute TO */
	uint sessiontimeout_warning;	/* session timeout warning interval */
	int logoutchar;			/* session disconnect character */
	int activatechar;		/* session activation character */
	char *location;			/* pointer to location string */
	char *sessiontimeout_warning_msg; /* ptr to optional session */
					/* timeout warning*/
	password_struct *password;	/* pointer to password structure */
	boolean vacantsuppressed;	/* TRUE if no banner desired */
	char *vacantstring; 		/* pointer to vacant banner string*/
	char *tty_def_autocommand;	/* Default autocommand for line */
	char *tty_autocommand;		/* Dynamic autocommand for line */
	char *tty_busymsg;		/* msg to send to net if line busy */
	ulong transport_allowin;	/* incoming Transports allowed */
	ulong transport_allowout;	/* outgoing transports allowed */
	int sessionlimit;		/* Maximum sessions; 0 = unlimited */
	ulong statbits;			/* Line state flags */
	ulong statbits2;		/* Line state flags */
	char *modem_regexp_string;	/* Regular expression for modem */
	regexp *modem_regexp_comp;	/* Compiled regexp for modem */

	/*
	 * Current line parameters.  Settable by unprivileged configuration.
	 */
	ulong capabilities;
	ulong capabilities2;
	char *termtype;
        char *keymaptype;
	tty_userdata current;
	texti_struct texti_info;

	/*
	 * Default line parameters.  Set by privileged configuration.
	 */
	ulong def_capabilities;
	ulong def_capabilities2;
	char *def_termtype;
        char *def_keymaptype;
	int dispatch_state;
	tty_statemtype *dispatch_machine;
	tty_userdata *defaults;		/* Saved defaults */
	tty_priv privilege;		/* Users current enable level */
	boolean pushdata;

	/*
	 * Modem control is hearby divorced from "statbit" and "capabilities"
	 * This should make the modem code much cleaner.
	 */
	uchar modem_type;
	uchar modem_state;
	uchar modem_bits;		/* Save output signal states here */
	uchar modem_flags;

	uchar *modem_name;		/* Name of modem type on this line */

	/*
	 * Variables for UART based lines
	 */
	volatile void *uartdata;	/* pointer to uart data register */
	volatile void *uartstatus;	/* pointer to uart status register */
	volatile ushort	*board_txstatus; /* pointer to board TX status */
	volatile ushort	*board_rxstatus; /* pointer to board RX status */
	v120_info_type *v120_info;	/* pointer to v120 information */
	idbtype *(*cfgidb)(tt_soc *);	/* get the configuration idb */
	uint cache_rtpr;		/* Calculated Copan RTPR value */
	boolean short_rtpr;		/*  ... RTPR set to 1 */
	rxserv_t rxservice;		/* receive interrupt service */
	txserv_t txservice;		/* transmit interrupt service */
	void (*txfinish)(tt_soc *);	/* dispose of xmit buffer */
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
	paktype *outpak;		/* Current output packet */
	paktype *inpak[2],		/*  ...input packets (A/B) */
	    *inheadpak;			/* Packet being consumed via inhead */
	queuetype			/* Packets in line for input, output */
	    rxpakq, txpakq,
	    rxdataq;			/* RX data waiting to be used */
	int insize;			/* input buffer size in bytes */
	int outsize;			/* output buffer size in bytes */
	int totalin;			/* total characters input */
	int totalout;			/* total characters output */
	int linenoise;			/* count of bad chars when inactive */
	int overrun;			/* Count of uart overruns detected */
	int overflow;			/* Count of buffer overflows */
	int helper;			/* Process number of input helper */
	boolean helper_outready;	/* True if output ready for helper */
	modem_status modem_changes;	/* Modem signal change flags */
	sys_timestamp break_timer;	/* Time when current break ends */

	/*
	 * SLIP support
	 */
	hwidbtype *tty_idb;		/* Pointer to associated interface */
	paktype *slip_inpak;		/* current SLIP input packet */	
	paktype *slip_outpak;		/* Current SLIP output packet */

        araptype *arapptr;              /* pointer to arap state variables */
        mnp4type *mnp4ptr;
	int arap_version;
/* OLD_AUTH -- remove next line when old authentication goes away */
        char saved_passwd[PASSWORDLEN+1];/* buffer to keep the login password*/

        long arap_timelimit;            /* maximum connection time */
        long arap_warningtime;          /* disconnect warning time */
	long arap_mask;			/* ARAP bits from arap_public.h */
        int     arap_netlist;           /* net access list */
        int     arap_zonelist;          /* zone access list */
	int slip_istate;		/* SLIP input state */
	int slip_ostate;		/* SLIP output state */
	ulong slip_flags;		/* SLIP flags */
        ipaddrtype async_peeraddr;      /* Current ASYNC peer address */
	/* Xremote support */
	xrcontext *xrcb;		/* Pointer to Xremote context block */

	/* Process debugging support */
	debug_context *debugging;	/* Pointer to debugging context */

	/* NTP reference clock support */
	ntp_refclock *tty_refclock;	/* NTP reference clock context */
	int ts_char_1;			/* Timestamp trigger character 1 */
	int ts_char_2;			/* Timestamp trigger character 2 */
	ushort ntp_status_mask;		/* Status mask for testing PPS */
	ushort ntp_status_value;	/* Expected value after mask */

	/* Async PPP support */

	pppContext *appp_context;	/* PPP context */

	/* Logging synchronous feature */
	boolean log_sync;	        /* Is this feature on */
	int log_sync_lock;	        /* Indicate if message is printable */
	int log_sync_level;	        /* Severity level printout async */
	int log_sync_msgsize;           /* Size of sync message queue */
	char log_sync_reprint_msg[LOG_SYNC_RPRNT_BUFSIZE];  
	                                /* prompt to be reprinted */
	int log_sync_reprint_type;      /* reprint type */

	/* async support */
	ulong dtr_pulseinterval;        /* Seconds to pulse DTR */

	/* Chat scripts support */      /* Run chat script on line at events */

        /* Note: all chat script pointers are regular expressions */

	char *scr_startup;              /* System startup script for line*/
	char *scr_connection;           /* Incoming connection script */
	char *scr_dialer;               /* Dialer script */
	char *scr_reset;                /* Reset device on line */
	char *scr_activation;           /* Line activation script */
	char *scr_line_change;          /* Line config changed script */
        char *scr_callback;             /* Line callback script */
        char *scr_arapcallback;         /* Line arap callback script */
        char *scr_offhook;              /* Line off-hook script */
	char *dial_string;              /* Chat dial string */

	/* Compiled regular exrpressions for script names */

	regexp *scr_startup_comp;
	regexp *scr_connection_comp;
	regexp *scr_dialer_comp;
	regexp *scr_reset_comp;
	regexp *scr_activation_comp;
	regexp *scr_line_change_comp;
        regexp *scr_callback_comp;
        regexp *scr_arapcallback_comp; 
        regexp *scr_offhook_comp;

	int parse_cli_type;		/* Determines type of parser used */

	/* RSH support */
	rshcontext *rshcontext;

	/* ISPY support */
	void (*ispy)(tt_soc *, void *, int);
	void *ispyctx;

	/* Remote redirect via IPC support (HSA, dual RSP) */
	ipc_port_id RTTY_server_port;		/* master's port */
	int server_ttynum;			/* tty on master */
	ipc_message *RTTY_req_msg;

	/* Line NVGEN optimization */
	ulong cfg_counter;
};

/* 
 * input/output selector for calls to retrieve modem information from
 * modemsupport code.  Tells called routine whether we want input
 * signal or output signal information.
 */
#define SNMP_MODEM_INPUT_SIGNAL      1   /* Return info on input signal(s) */
#define SNMP_MODEM_OUTPUT_SIGNAL     2   /* Return info on output signal(s) */

struct snmp_signal_info_ {
    long rs232_signal_name;		 /* see snmp/sr_rs232mibdefs.h */
    long rs232_signal_state;		 /* see snmp/sr_rs232mibdefs.h */
};

/*
 * Possible processes owning a tty
 * useful for determining which chat script to run on the line
 */
enum TTY_PROCESS {
    TTYP_EXEC,                  /* exec for ui */
    TTYP_NW_DAEMON,             /* tty_daemon for network connections */
    TTYP_TELNET,                /* Forward telnet or rlogin */
    TTYP_CHAT,                  /* chat dialer */
    TTYP_ARAP,                  /* arap process */
    TTYP_XREMOTE,               /* xremote process */
    TTYP_UNKNOWN
  };

/*
 * Global tt_soc variables
 */
extern tt_soc *MODEMS[];	/* the array of terminal data blocks */
extern tt_soc *stdio;		/* pointer to current terminal */
extern tt_soc *console;		/* pointer to console terminal */
extern uchar execmask_default, datamask_default, specialmask_default;

/* platform line defaults */
extern int platformdef_asyncspeed[];
extern int platformdef_stopbits[];
extern ulong platformdef_capabilities[];

/* Hooks to optimize line NVGEN */
extern ulong tty_cfg_rotor;

/*
 * Used for
 * 3270 keymap definitions
 * termcap definitions
 */
#define MAX3270DEFNAME 32
struct tty_3270def_ {
        struct tty_3270def_ *next_def; /* pointer to next def in list    */
        char def_name[MAX3270DEFNAME + 1];
        char def[1];                   /* when this is dynamically allocated */
                                       /* it will be larger than 1 char      */
};

/*
 * Fake TTY for printf/putc/puts
 */
#define NULLTTY ((tt_soc *) -1)
#define CONTTY ((tt_soc *) -2)

/*
 * Enum for NTP setup vector
 */
typedef enum ntp_setuptype_ {
    NTP_SETUP_PROBE,			/* Does this port support NTP? */
    NTP_SETUP_PPSTEST,			/* Does it support these PPS bits? */
    NTP_SETUP_DISABLE,			/* Turn off support */
    NTP_SETUP_ENABLETIME,		/* Enable timecode support */
    NTP_SETUP_ENABLEPPS,		/* Enable PPS support */
    NTP_SETUP_ENABLETIMEPPS		/* Enable timecode and PPS support */
} ntp_setuptype;


/*
 * NTP PPS signal types.  Note that the order must match the code in
 * ntp_refclock_config.c.
 */
enum {
    NTP_PPS_DCD_BITNUM,
    NTP_PPS_RTS_BITNUM,
    NTP_PPS_CTS_BITNUM,
    NTP_PPS_DTR_BITNUM,
    NTP_PPS_DSR_BITNUM,
    NTP_PPS_RI_BITNUM,
    NTP_PPS_BITNUM_COUNT		/* Number of bits */
};

#define NTP_PPS_NONE	0
#define NTP_PPS_DCD	(1 << NTP_PPS_DCD_BITNUM)
#define NTP_PPS_RTS	(1 << NTP_PPS_RTS_BITNUM)
#define NTP_PPS_CTS	(1 << NTP_PPS_CTS_BITNUM)
#define NTP_PPS_DTR	(1 << NTP_PPS_DTR_BITNUM)
#define NTP_PPS_DSR	(1 << NTP_PPS_DSR_BITNUM)
#define NTP_PPS_RI	(1 << NTP_PPS_RI_BITNUM)
#define NTP_PPS_INVERTED 0x80000000	/* Set if pulse sense is inverted */



/*
 * Character device vector definitions.
 */

typedef struct vectortype_ {
	void (*vsetup)(tt_soc *);	/* set up input and output buffers */

	int (*vgetc)(tt_soc *);		/* character input, no wait */
	void (*vstartinput)(tt_soc *);	/* start character input */
	void (*vclearinput)(tt_soc *);	/* clear buffered input */
	int (*vinputpending)(tt_soc *);	/* number of bytes buffered for input */

	void (*vputc)(tt_soc *, int);	/* character output */
	boolean (*vstartoutput)(tt_soc *); /* start character output */
	void (*vclearoutput)(tt_soc *);	/* clear buffered output */
	int (*voutputpending)(tt_soc *);/* number of bytes buffered for output */
	boolean (*voutputblock)(tt_soc *); /* Would an output call block? */
	boolean (*voutputxoffed)(tt_soc *); /* Is the output Xoffed ? */
	int (*vputpkt)(tt_soc *, paktype *, uchar *, int);	/* Output data from a packet */

	boolean (*vsetspeed)(tt_soc *, int, int);	/* set device speed */
	void (*vautosetspeed)(tt_soc *);	/* set speed for autobauding */
	void (*vauto_baud)(tt_soc *);	        /* set autobaud function */
	void (*vstopbits)(tt_soc *, int);	/* set stop bits */
	void (*vparitybits)(tt_soc *, int);	/* set parity handling */
	void (*vdatabits)(tt_soc *, int);	/* set number of data bits */
	void (*vsendbreak)(tt_soc *);	/* send a break "character" */
	void (*vdtrstate)(tt_soc *, boolean);	/* set DTR state */
	void (*vsetflow)(tt_soc *);	/* set hardware flow control */
	void (*vsetsflow)(tt_soc *);	/* set software flow control */
	/* set txservice/rxservice hook */
	boolean (*vservicehook)(tt_soc *, txserv_t, rxserv_t);
	void (*vnoservicehook)(tt_soc *); /* disable txservice/rxservice hook */
 	void (*vportenable)(tt_soc *);	/* enable a port transmitter and receiver */
 	void (*vportdisable)(tt_soc *);	/* enable a port transmitter and receiver */
 	modem_status (*vmodemsignal)(tt_soc *); /* read current modem signals */
 	modem_status (*vmodemchange)(tt_soc *);	/* read modem change flags */
	void (*vstopoutput)(tt_soc *);	/* Stop current output */
	void (*vpppmode)(tt_soc *, int);	/* enter/leave PPP mode */
	void (*vpppmap)(tt_soc *, ulong, ulong); /* set PPP ctl char map */
	boolean (*vntpsetup)(tt_soc *, ntp_setuptype, ulong); /* Setup for NTP */
} vectortype;

#define NVECTORS (sizeof(vectortype) >> 2)   /* number of dispatch vectors */

#define setup(tty,in,out) ((*(tty->ovector->vsetup)) (tty,in,out))

#define getc(tty) ((*(tty->ivector->vgetc)) (tty))
#define clearinput(tty) ((*(tty->ivector->vclearinput)) (tty))
#define inputpending(tty) ((*(tty->ivector->vinputpending)) (tty))

/*
 * Note that vstartinput is supposed to enable uart reecivers and such,
 * even if the "input" io vector is a TCP connection, so it always uses
 * ovector->vstartinput instead of ivector->vstartinput (which changes)
 */
#define startinput(tty) ((*(tty->ovector->vstartinput)) (tty))
#define putc(tty,ch) ((*(tty->ovector->vputc)) (tty,ch))
#define clearoutput(tty) ((*(tty->ovector->vclearoutput)) (tty))
#define startoutput(tty) ((*(tty->ovector->vstartoutput)) (tty))
#define outputpending(tty) ((*(tty->ovector->voutputpending)) (tty))
#define outputblock(tty) ((*(tty->ovector->voutputblock)) (tty))
#define outputxoffed(tty) ((*(tty->ovector->voutputxoffed)) (tty))
#define putpkt(tty, pak, datastart, len) ((*(tty->ovector->vputpkt)) (tty, pak, datastart, len))

#define setspeed(tty,tx,rx) ((*(tty->ovector->vsetspeed)) (tty,tx,rx))
#define auto_setspeed(tty) ((*(tty->ovector->vautosetspeed)) (tty))
#define auto_baud(tty) (*(tty->ovector->vauto_baud))
#define setstopbits(tty,arg) ((*(tty->ovector->vstopbits)) (tty,arg))
#define setparity(tty,arg) ((*(tty->ovector->vparitybits)) (tty,arg))
#define setdatabits(tty,arg) ((*(tty->ovector->vdatabits)) (tty,arg))
#define sendbreak(tty) ((*(tty->ovector->vsendbreak)) (tty))
#define dtrstate(tty,arg) ((*(tty->ovector->vdtrstate)) (tty,arg))
#define setflow(tty) ((*(tty->ovector->vsetflow)) (tty))
#define setsflow(tty) ((*(tty->ovector->vsetsflow)) (tty))

/*
 * Eventually, all of the entry points should be changed to use the
 * "native" vectors and the input and output vectors eliminated in favor
 * of standard I/O to network connections. This will simplify a lot
 * of the system.
 */

#define tty_portenable(tty) ((*(tty->nvector->vportenable)) (tty))
#define tty_portdisable(tty) ((*(tty->nvector->vportdisable)) (tty))

#define tty_dtrstate(tty,arg) ((*(tty->nvector->vdtrstate)) (tty,arg))
#define tty_modemsignal(tty) ((*(tty->nvector->vmodemsignal)) (tty))
#define tty_modemchange(tty) ((*(tty->nvector->vmodemchange)) (tty))
#define tty_servicehook(tty, tx, rx) ((*(tty->nvector->vservicehook)) (tty, tx, rx))
#define tty_noservicehook(tty) ((*(tty->nvector->vnoservicehook)) (tty))
#define tty_stopoutput(tty) ((*(tty->nvector->vstopoutput)) (tty))
#define tty_pppmode(tty, on) ((*(tty->nvector->vpppmode)) (tty, on))
#define tty_pppmap(tty, txbits, rxbits) \
    ((*(tty->nvector->vpppmap)) (tty, txbits, rxbits))

/*
 * Character I/O routines
 */

extern void serial_clearinput(tt_soc *);
extern void serial_putc(tt_soc *, int);
extern boolean serial_outputblock(tt_soc *);
extern boolean serial_outputxoffed(tt_soc *);
extern int generic_putpkt(tt_soc *, paktype *, uchar *, int);

extern void serial_clearoutput(tt_soc *);
extern int serial_outputpending(tt_soc *);

extern boolean generic_servicehook(tt_soc *, txserv_t, rxserv_t);
extern void generic_noservicehook(tt_soc *);
extern boolean helper_servicehook(tt_soc *, txserv_t, rxserv_t);
extern void helper_noservicehook(tt_soc *);
extern void rx_tx_handler(tt_soc *);
extern void console_putc(uchar);
extern int console_getc(void);

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * Prototypes for os/null_tty.c
 */
extern void null_io_init(void);
extern vectortype null_io_vector;

/*
 * Prototypes for os/sched.c
 */
extern tt_soc *disable_io (void);
extern void restore_io (tt_soc *);

/*
 * Prototypes for os/ttycon.c
 */
extern boolean rdtty(char *, int);
extern boolean texti(char *, int, char *, char *);
extern boolean ttyioBLOCK(tt_soc *);
extern int getcwait(tt_soc *);
extern int serial_getc(tt_soc *);
extern int serial_inputpending(tt_soc *);
extern int ttyin(tt_soc *);
extern tt_soc *tty_init(int, ulong, int, vectortype *);
extern void erase_char(uchar);
extern void tty_init_data(tt_soc *, int, ulong, int, vectortype *);
extern void vector_init(vectortype *);
extern void reprint_prompt(tt_soc *);
extern boolean tty_signal_helper(tt_soc *);
extern int AuxBase;

/*
 * Prototypes for os/ttysrv.c
 */
extern int maxvtylines;
extern int defvtylines;
extern queuetype *translateQ;
extern boolean modemcheck(tt_soc *);
extern char *stopstring(int);
extern int special_chars(tt_soc *, int);
extern int tt2number(tt_soc *);
extern tt_soc *tty_allocate(int, int);
extern void tty_create_vty_exec(tt_soc *);
extern int wall(char *);
extern void printbits(char *, long, char *const array[], long, long *);
extern void restoreflow(tt_soc *, ulong);
extern void tty_clearbuffers(tt_soc *);
extern void tty_deallocate(tt_soc *);
extern void tty_savedefaults(tt_soc *);
extern void ttybug(tt_soc *, char *);
extern void ungetc(tt_soc *, int);
extern void vty_init(void);
extern void tty_if_reset(tt_soc *);
extern boolean tty_xon_default(tt_soc *);
extern void create_vtylines(int);
extern void destroy_vtylines(int);
extern int get_vtyline(ulong, connvector *);
extern int select_line(int, connvector *);
extern int select_rotary(int, connvector *, tt_soc *, int);
extern char *get_tty_location(tt_soc *);
extern void tty_setedit(tty_editdata *, int, int);

/*
 * Prototypes for if/if_async.c
 */
extern void download_command(parseinfo *);

/*
 * modemsupport.c
 */
extern int try_19200(tt_soc *);
extern int auto_verify(tt_soc *);
extern void tty_startexec(tt_soc *);
extern void async_autosetspeed(tt_soc *);
extern void async_autobaud(tt_soc *);
extern void modemline(tt_soc *, boolean);
extern char *modem_statestring(tt_soc *);
extern void modem_command(parseinfo *);
extern int snmp_num_modem_signals(tt_soc *tty, uchar input_or_output);
extern boolean snmp_get_modem_signal(tt_soc *tty, snmp_signal_info
				     *signal_ptr, long signal_num,
				     boolean next, uchar input_or_output);  
extern char* print_modemtype(tt_soc *);
extern const int autobaud_retry;
extern int modem_autoconfigure(tt_soc *);

/*
 * Other prototypes
 */
extern tt_soc *auxline_init(void);
extern int analyzeAUXports(void);
extern tt_soc *console_init(void);

/*
 * async_chain.c
 */
extern void async_parser_init(void);

#endif		/* __TTYSRV_H__ */
