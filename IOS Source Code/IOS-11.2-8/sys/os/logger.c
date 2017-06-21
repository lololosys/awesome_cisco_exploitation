/* $Id: logger.c,v 3.8.12.7 1996/08/29 23:31:41 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/logger.c,v $
 *------------------------------------------------------------------
 * logger.c -- system logger and error message process
 * 
 * January 1987, Greg Satz
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: logger.c,v $
 * Revision 3.8.12.7  1996/08/29  23:31:41  snyder
 * CSCdi67619:  facility obsoleted by CSCdi67083, dump it, save memory
 *              saves 404 bytes from image, 360 from data section
 *              also pre-commit hamptons error message doc macros, so
 *              between the 2 of us we only touch logger.h one time
 * Branch: California_branch
 *
 * Revision 3.8.12.6  1996/08/28  13:03:25  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.12.5  1996/07/23  13:27:05  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.8.12.4  1996/06/19  04:43:59  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.8.12.3  1996/05/21  06:36:33  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.8.12.2  1996/05/09  14:39:52  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.8.12.1.18.2  1996/04/30  05:24:05  cakyol
 * forgot to convert %#C to a %#CC.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.8.12.1.18.1  1996/04/27  07:17:04  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.8.12.1.8.1  1996/04/11  23:12:59  cakyol
 * missed converting this to "%CC" the first time round.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.8.12.1  1996/03/18  21:30:11  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.4  1996/03/16  07:20:44  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.20.3  1996/03/09  05:09:51  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.20.2  1996/03/07  10:18:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.20.1  1996/02/20  16:40:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/16  21:17:32  hampton
 * Fix false indications of buffer overrun, and a small buffers leak in
 * this situation.  [CSCdi49224]
 *
 * Revision 3.7  1996/02/09  18:25:15  hampton
 * CSCdi48543:  errmsg_guts() can loop if memory is already corrupt
 *
 * Revision 3.6  1996/02/07  16:14:26  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.5  1996/02/01  06:07:45  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/11/30  00:02:47  hampton
 * Print a stack traceback before attempting to dump core.  [CSCdi44670]
 *
 * Revision 3.3  1995/11/17  18:48:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:46:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:18:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/08/25  11:44:46  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.5  1995/07/18 06:48:19  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.4  1995/07/07 05:26:43  hampton
 * Hide the definitions of watched data structures.  These data structures
 * should never be referenced directly.  [CSCdi36799]
 *
 * Revision 2.3  1995/06/19 06:48:47  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/16  04:29:22  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  21:56:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "sched_private.h"
#include "connect.h"
#include "logger.h"
#include "logger_private.h"
#include "mgd_timers.h"
#include "../os/clock.h"
#include "../parser/parser_defs_service.h"
#include "../ui/service.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_logger.h"
#include "../os/async.h"
#include "../os/boot.h"

/*
 * stack trace declarations
 */
static void traceback(char*, int);


/*
 * Storage
 */
int log_facility;
boolean log_init;
static paktype* lastlogpak;
static mgd_timer lastlogclock;
queuetype logttyQ;
static watched_queue *lQ;
int log_console;
int log_buffer;
int log_monitor;
int log_traps;
ulong log_console_count;
ulong log_buffer_count;
ulong log_monitor_count;
ulong log_traps_count;
ulong log_drop_count;
ulong log_overrun_count;
ulong log_flush_count;
uchar *log_buffer_addr;       /* base address of buffered logging ring buffer */
uint log_buffer_size;         /* size of buffered logging ring buffer         */
static uint log_buffer_begin; /* offset to first character within buffered    *
                               *   logging ring buffer                        */
static uint log_buffer_chars; /* number of characters in buffered logging     *
                               *   ring buffer                                */
static pid_t logger_pid;
ulong service_timestamps[NUM_SERVICE_TIMESTAMPS];

boolean some_errmsgs_have_printed = FALSE; /* set TRUE if err   *
                                                          * gets logged before*
                                                          * bootload is done  */

/*
 * Get the current date and time and return the length of the string.
 * Interpret the various "service timestamp" flags for options.
 */

static int sysclock_timestamp (char *timestring, ulong flags)
{
    clock_epoch epoch;
    timeval tv;
    char *tz_name;
    long tz_offset;
    char format_string[20];
    
    /* Set up the base formatting string. */

    strcpy(format_string, "%v%tn %bd %T");

    /* If milliseconds are desired, add that to the format. */

    if (flags & SERVICE_TIMESTAMP_MSEC) {
	strcat(format_string, ".%k");
    }

    /* If timezone is desired, add that as well. */

    if (flags & SERVICE_TIMESTAMP_SHOWTZ) {
	strcat(format_string, " %tZ");
    }

    /* Get the current time. */
    
    clock_get_time(&epoch);

    /* Adjust for local or UTC time as requested. */

    if (flags & SERVICE_TIMESTAMP_LOCAL) {
	clock_get_timezone_info(&epoch, &tz_name, &tz_offset); /* Local */
    } else {
	tz_offset = DEFAULT_TZ_OFFSET;			/* UTC */
	tz_name = DEFAULT_TZ_NAME;
    }
    /* Convert to a timeval. */

    clock_epoch_to_timeval(&epoch, &tv, tz_offset);

    /* Format the string. */

    return(format_time(timestring, 0, format_string, &tv, tz_name));
}


/*
 * add_timestamp
 * Add the appropriate timestamp
 */
static int add_timestamp(int type, char *cp)
{
    ulong *svc_ptr = &service_timestamps[type];
    
    if (*svc_ptr & SERVICE_TIMESTAMP_DATETIME) {
	return(sysclock_timestamp(cp, *svc_ptr));
    } else {
	return(sprintf(cp, "%TN"));
    }
}

/*
 * logger_get_default_level
 *
 * Return the default level for this logging device
 */

int logger_get_default_level (int device)
{
    switch (device) {
      case P_LOG_CONSOLE:
      case P_LOG_MONITOR:
      case P_LOG_BUFFERED:
	return(LOG_DEBUG);

      case P_LOG_TRAP:
	return(LOG_INFO);

      default:
	return(LOG_DISABLED);
    }
}


/*
 * logger_get_default_buffer_size
 *
 * Return the default buffer size for this platform
 */

uint logger_get_default_buffer_size (boolean init)
{
    uint size;

    /*
     * The default size is determined in the following steps
     *
     * 1) If the platform declares a default size, use that
     * 2) Use the LOG_BUFFERSIZE default
     *
     * However, if the init boolean is TRUE and the platform
     * returns a size of zero, no logging buffer is created
     * until configured by the user
     */
    size = platform_get_value(PLATFORM_VALUE_LOG_BUFFER_SIZE);

    if ((!size) && (!init))
	size = LOG_BUFFERSIZE;

    return(size);
}


/*
 * logger_buffer_init
 *
 * Initialize and manage buffer logging pointers
 */

void logger_buffer_init (uint size, boolean enabled, boolean init)
{
    uchar *start;

    /*
     * (Re)Initialize the non-address buffer variables
     */
    log_buffer_begin = 0;
    log_buffer_chars = 0;
    /*
     * If there's no change, there's no need to do memory reallocation
     */
    if (enabled && (size != 0) && (size == log_buffer_size))
	return;
    
    /*
     * If we have a buffer, free it
     */
    if (log_buffer_addr) {
	/*
	 * We always set the start pointer to NULL before we actually
	 * free the buffer structures
	 */
	start = log_buffer_addr;
	log_buffer_addr  = NULL;
	log_buffer_size = 0;
	free(start);
    }
    /*
     * If the buffer is still enabled, create one of the given size
     */
    if (enabled) {
        /*
	 * If no size paramter was supplied, find a default
	 */
	if (!size)
	    size = logger_get_default_buffer_size(init);
	
	/*
	 * If we get here without a valid size, return
	 */
	if (!size)
	    return;

	/*
	 * Remember what the size configured was
	 */
	log_buffer_size = size;

	/*
	 * If after all that, we still have size, create a new buffer
	 */
	start = malloc(log_buffer_size);
	if (!start) {
	    /*
	     * If we don't call this at init time, we assume that it's
	     * being manipulated by a user so we print a message out
	     */
	    if (!init)
		printf(nomemory);
	    return;
	}

	/*
	 * The last thing we do is set the start pointer to re-enable
	 * buffer logging again
	 */
	log_buffer_addr = start;
    }
}


/*
 * logger_init
 * Initialize logging structures
 */

void logger_init (void)
{
    int i;

    lQ = create_watched_queue("Log Messages", LOG_MAXMESSAGES, 0);
    queue_init(&logttyQ, 0);
    log_facility = P_LOG_FAC_LOCAL7;
    lastlogpak = NULL;
    mgd_timer_init_leaf(&lastlogclock, NULL, 0, NULL, TRUE);

    /*
     * Set the default logging levels
     */
    log_console = logger_get_default_level(P_LOG_CONSOLE);
    log_buffer  = logger_get_default_level(P_LOG_BUFFERED);
    log_monitor = logger_get_default_level(P_LOG_MONITOR);
    log_traps   = logger_get_default_level(P_LOG_TRAP);

    /*
     * Configure the logging buffer
     */
    logger_buffer_init(0, TRUE, TRUE);

    for (i=0; i<NUM_SERVICE_TIMESTAMPS; i++)
	service_timestamps[i] = 0;
    reg_add_flush_logging(logger_flush, "logger_flush");

    if (system_loading) {
	log_init = FALSE;
	return;
    }

    log_init = TRUE;

    /*
     * Initialize kernel hooks
     */
    kernel_set_buginf_hook(buginf);
    kernel_set_errmsg_hook(errmsg);

    /*
     * Initialize logger parser support
     */
    logger_parser_init();
}


/*
 * log_dequeue
 * Dequeue a message for logging queue and keep track
 * of the total number of pending messages
 */

static inline paktype *log_dequeue (void)
{
    return(process_dequeue(lQ));
}

/*
 * logger_flush_console
 * If the console's message queue has filled up, we will empty it using
 * ROM monitor I/O. This has the effect of "freezing" the system until the
 * messages can get out.
 */

static void logger_flush_console (void)
{
    paktype *pak;

    /*
     * Dump the pending message queue for the console
     */
    ++log_flush_count;
    if (console == NULL)
	return;
    while ((pak = pak_dequeue(&console->messageQ))) {
	while (console->message_offset < pak->datagramsize) {
	    console_putc(pak->datagramstart[console->message_offset++]);
	}
	console->message_offset = 0;
	log_console_count += pak->unspecified[1];
	datagram_done(pak);
    }
}

/*
 * logger_print_buffer
 * 
 * Dump contents of loggering buffer to the terminal
 */

void logger_print_buffer (tt_soc* tty, boolean domore)
{
    uchar c;
    uint  count;
    uint  index;

    index = log_buffer_begin;
    count = log_buffer_chars;
        
    if ((log_buffer_addr == NULL) || (count == 0))
	return;

    ttyprintc(tty, '\n');
    do {
	c = log_buffer_addr[index++];
	if (index >= log_buffer_size)
	    index = 0;
	if (c == '\n') {
	    if (domore && automore_quit())
		break;
	}
	ttyprintc(tty, c);
    }
    while (--count != 0);
}

/*
 * logger_flush
 * We're dying, flush all pending messages to console
 */

void logger_flush (void)
{
    paktype* pak;

    if (log_buffer_addr) {
	ttyprintf(CONTTY, "\nBuffered messages:");
	logger_print_buffer(CONTTY, FALSE);
    }
    ttyprintf(CONTTY, "\nQueued messages:");
    if (lastlogpak) {
	leveltype SR;

	SR = raise_interrupt_level(ALL_DISABLE);
	process_enqueue_pak(lQ, lastlogpak);
	lastlogpak = NULL;
	reset_interrupt_level(SR);
	mgd_timer_stop(&lastlogclock);
    }
    while ((pak = log_dequeue())) {
	tty_send_logmessage(console, pak, pak->unspecified[0], 
			    pak->unspecified[1]);
    }
    logger_flush_console();
    return;
}

/*
 * log_enqueue
 * Add a message to the logger queue and keep track
 * of the total number of pending messages
 *
 * use pak->unspecified[0] for level of messages in this buffer
 *     pak->unspecified[1] for number of messages in this buffer
 */

static void log_enqueue (paktype *pak)
{
    leveltype status;
    paktype *tmp;

    status = raise_interrupt_level(ALL_DISABLE);
    if (lastlogpak) {
	if ((pak->unspecified[0] == lastlogpak->unspecified[0]) &&
	    (pak->datagramsize + lastlogpak->datagramsize < (LOGDATA +
							     ENCAPBYTES))) {
	    bcopy(pak->datagramstart,
		  &lastlogpak->datagramstart[lastlogpak->datagramsize],
		  pak->datagramsize);
	    lastlogpak->datagramsize += pak->datagramsize;
	    ++lastlogpak->unspecified[1];
	    reset_interrupt_level(status);
	    retbuffer(pak);
	    return;
	}
	/*
	 * Must clear lastlogpak before the call to enqueue.  Otherwise
	 * If the enqueue screws up somehow, god forbid, the router would
	 * end up right back here with the same value of lastlogpak and
	 * the recurion would continue ad-infinitum.
	 */
	tmp = lastlogpak;
	lastlogpak = NULL;
	process_enqueue_pak(lQ, tmp);
    }
    pak->unspecified[1] = 1;
    lastlogpak = pak;
    mgd_timer_start(&lastlogclock, ONESEC);
    reset_interrupt_level(status);
    return;
}

/*
 * log_overrun_notify
 */

static void log_overrun_notify (const char* fmt, ulong caller_pc)
{
    ++log_overrun_count;
    ttyprintf(CONTTY, "\n"
	      "%%Log packet overrun, potential memory corruption, "
	      "PC %#x, format:\n%s\n",
	      caller_pc, fmt);
    return;
}

/*
 * doerr - Format a system error message
 *
 * Format the error and store the string to the pointer "cp". Length
 * overrun will be detected here. This function returns a character
 * pointer to the end of the formatted string.
 * 
 */

static char *doerr (const message *msg, char *cp, int len, ulong call_pc,
		    ulong frame_skip_count, va_list args)
{
    char *buf = cp;
    const char *proc_name;
    int cp_len;
    pid_t pid;

    *cp++ = '\n';
    if (service_timestamps[SERVICE_TIMESTAMPS_LOG]) {
	cp += add_timestamp(SERVICE_TIMESTAMPS_LOG, cp);
	*cp++ = ':';
	*cp++ = ' ';
    }
    cp += sprintf(cp, "%%%s-%d-%s: ",
		  msg->fac, msg->severity, msg->name);
    cp += _doprnt(cp, NULL, msg->format, args, FALSE);
    if (msg->flags & MSG_PROCESS) {
	process_get_name(THIS_PROCESS, &proc_name);
	cp += sprintf(cp, "\n-Process= \"%s\", ipl= %d",
		      (onintstack() != 0) ? "<interrupt level>" : proc_name,
		      get_interrupt_level());
	if (system_running && !schedflag) {
	    process_get_pid(&pid);
	    cp += sprintf(cp, ", pid= %d", pid);
	}
    }
    if (msg->flags & MSG_TRACEBACK || log_backtrace) {
	traceback(cp, frame_skip_count + 1); /* Skip caller frames plus ours */
	cp += strlen(cp);
    }

    cp_len = (int)(cp - buf);
    if (cp_len > len) {
	log_overrun_notify(msg->format, call_pc);
	return((char*)NULL);
    }
    return(cp);
}

/*
 * errfmt - Format a system error message to buffer.
 *
 * This function acts like errmsg() except it stores the output to "buf".
 * The "len" specifies the size of "buf".
 *
 */

char *errfmt (const message *msg, char *buf, int len, ...)
{
    va_list args;
    char *p;

    SAVE_CALLER();

    va_start(args, len);
    p = doerr(msg, buf, len, caller(), 1, args); /* Skip this frame */
    va_end(args);
    return(p);
}

/*
 * incr_log_lostcount
 *
 * Increment the message lost count by one for all tty except console.
 *
 */

static void incr_log_lostcount (int severity)
{
    ttylisttype *tp;

    if (severity <= log_monitor) {
	for (tp = (ttylisttype *)logttyQ.qhead; tp; tp = tp->next) {
	    if (tp->tty->log_sync)
		tty_log_lostcount(tp->tty, 1);
	}
    }
}

/*
 * errmsg is the primary mechanism for logging messages. errmsg takes
 * a pointer to a message descriptor, and an optional list of arguments.
 */

static boolean errmsg_guts_entered;	/* TRUE if errmsg_guts is running */
static
void errmsg_guts (const message *msg, ulong caller_pc, va_list args)
{
    paktype *pak;
    char* cp;
    const char *proc_name;
    message_limiter *limit;
    pid_t pid;

    /*
     * Bail if we are recursing.  Note that this will also block the
     * case where we happen to call errmsg from an interrupt routine
     * that interrupts errmsg.  Life is like that.  This code isn't
     * bulletproof in the case of interrupts, but the point is to simply
     * ensure that the stack isn't overrun by calling too deeply.
     */
    if (errmsg_guts_entered)		/* Already here? */
	return;				/* G'wan, git out. */
    errmsg_guts_entered = TRUE;		/* Lock 'em out, more or less. */

    /*
     * Rate limit the message if appropriate.  This will alias every
     * 49 days for a period of the rate limit, but only if a message
     * was issued exactly 49 days after the last time.  This is an
     * acceptable risk until 64 bit timers are available.
     *
     * The nonzero check on limit->last_time is a special case for rate
     * limited messages issued immediately after system restart--we
     * don't want to forget to issue those.
     */

    limit = msg->limiter;
    if (limit != NULL) {
	if (limit->rate_limit) {
	    /*
	     * Don't limit the first msg
	     */
	    if (TIMER_RUNNING(limit->last_time)) {
		if (CLOCK_IN_INTERVAL(limit->last_time, limit->rate_limit))
		    goto byebye;
	    }
	    GET_TIMESTAMP(limit->last_time);
	}
    }

    if (log_init && !process_is_queue_full(lQ) &&
	((msg->severity > log_console) || !QUEUEFULL(&console->messageQ))) {
	pak = getbuffer(LOGDATA);
	if (pak) {
	    pak->datagramstart = pak->data_area;
	    pak->unspecified[0] = msg->severity;
	    pak->flags |= PAK_PRIORITY;
	    cp = (char*) pak->datagramstart;
	    if ((cp = doerr(msg, cp, LOGDATA + ENCAPBYTES, caller_pc, 2,
			    args))) {	/* Skip this frame plus caller's */
                pak->datagramsize = (uchar*) cp - pak->datagramstart;
                log_enqueue(pak);
            }
	    goto byebye;
	}
    }
    if (!log_init || (msg->severity <= log_console)) {
	/*
	 * If logging synchronous is turned on, use its severity level.
         */
	if (!console ||
	    (!console->log_sync || msg->severity <= console->log_sync_level)) {
	    logger_flush_console();
	    ttyprintf(CONTTY, "\n%%%s-%d-%s: ", msg->fac, msg->severity,
		      msg->name);
	    (void) _doprnt (NULL, CONTTY, msg->format, args, FALSE);
	    if (msg->flags & MSG_PROCESS) {
		process_get_name(THIS_PROCESS, &proc_name);
		ttyprintf(CONTTY, "\n-Process= \"%s\", ipl= %d",
			  (onintstack() != 0) ?
			  "<interrupt level>" : proc_name,
			  get_interrupt_level());
		if (system_running && !schedflag) {
		    process_get_pid(&pid);
		    ttyprintf(CONTTY, ", pid= %d", pid);
		}
	    }
	    if (msg->flags & MSG_TRACEBACK) {
		char frames[TRACEBACK_SIZE];

		traceback(frames, 2 /* errmsg_guts + caller */);
		ttyprintf(CONTTY, "%s", frames);
	    }
	    ++log_console_count;
	} else {
	    /*
	     * Logging synchronous: increment message lost by 1 for console
	     */
	    tty_log_lostcount(console, 1);
	}
    } else
	++log_drop_count;
    /*
     * Logging synchronous: increment message lost count to all ttys
     */
    incr_log_lostcount(msg->severity);

    /* Open the flimsy lock. */

  byebye:
    errmsg_guts_entered = FALSE;
}

/*
 * errmsg - Display a system error message
 *
 * Use this routine to display any unsolicited output.
 */
void
errmsg (const message *msg, ...)
{
    va_list args;

    SAVE_CALLER();

    /* 
     * set this boolean so that the system can log a calming
     * message at the end of bootloading to assuage customer fear.
     */ 
    some_errmsgs_have_printed = TRUE;

    va_start(args, msg);
    errmsg_guts(msg, caller(), args);
    reg_invoke_syslog_history_event(msg, args); 
    va_end(args);
    return;
}

/*
 * emit an errmsg() with a modified format string
 */
void
errvarmsg (const message *msg, char *fmt, ...)
{
    va_list args;
    message newmsg;

    SAVE_CALLER();

    /* 
     * set this boolean so that the system can log a calming
     * message at the end of bootloading to assuage customer fear.
     */ 
    some_errmsgs_have_printed = TRUE;

    newmsg = *msg;
    newmsg.format = fmt;
    va_start(args, fmt);
    errmsg_guts(&newmsg, caller(), args);
    va_end(args);
    return;
}

/*
 * buginf
 *
 * Write debugging messages to any terminal that has logging enabled.
 *
 * THIS ROUTINE MAY ONLY BE USED FOR PRINTING MESSAGES THAT ARE
 * PREDICATED ON A DEBUGGING FLAG.  THE PRINTING OF ANY UNSOLICITED
 * MESSAGES WITH THIS ROUTINE IS CONSIDERED A CAPITAL CRIME.  ALL
 * UNSOLOCITED INFORMATION *MUST* BE PRINTED BY THE ERRMSG FACILITY.
 */
void buginf (const char *fmt, ...)
{
    paktype *pak;
    va_list args;

    SAVE_CALLER();

    /* 
     * set this boolean so that the system can log a calming
     * message at the end of bootloading to assuage customer fear.
     */ 
    some_errmsgs_have_printed = TRUE;

    if (console == NULL)
	return;

    va_start(args, fmt);
    if (log_init && !process_is_queue_full(lQ) &&
	((LOG_DEBUG > log_console) || !QUEUEFULL(&console->messageQ))) {

	pak = getbuffer(LOGDATA);
	if (pak) {
	    char* cp;
	    
	    pak->datagramstart = pak->data_area;
	    cp = (char*) pak->datagramstart;
	    if (service_timestamps[SERVICE_TIMESTAMPS_DEBUG] && *fmt == '\n') {
		fmt++;			/* Skip newline in format */
		*cp++ = '\n';
		cp += add_timestamp(SERVICE_TIMESTAMPS_DEBUG, cp);
		*cp++ = ':';
		*cp++ = ' ';
	    }
	    pak->unspecified[0] = LOG_DEBUG;
            /*
             * The code to prevent this memory corruption would
             * probably not be worth the performace degradation,
             * but at least now you'll know what happened.
             */
	    cp += _doprnt(cp, NULL, fmt, args, FALSE);
	    pak->datagramsize = (ushort) ((uchar*) cp - pak->datagramstart);
            if (pak->datagramsize > (pak_get_pool_size(pak) + ENCAPBYTES)) {
		log_overrun_notify(fmt, caller());
		retbuffer(pak);
		return;
            }

	    if (log_backtrace) {
		traceback(cp, 1 /* skip this frame */);
		cp += strlen(cp);
	    	pak->datagramsize = (ushort) ((uchar*) cp -
					      pak->datagramstart);
            	if (pak->datagramsize >
		    (pak_get_pool_size(pak) + ENCAPBYTES)) {
		    log_overrun_notify(fmt, caller());
		    retbuffer(pak);
		    return;
            	}
	    }
	    log_enqueue(pak);
	    return; 
	}
    }

    if (!log_init || (LOG_DEBUG <= log_console)) {
	/*
	 * If logging synchronous is turned on, use its severity level.
         */
	if (!console ||
	    (!console->log_sync || LOG_DEBUG <= console->log_sync_level)) {
	    logger_flush_console();
	    (void) _doprnt(NULL, CONTTY, fmt, args, FALSE);
	    ++log_console_count;
	} else {
	    /*
	     * Logging synchronous: increment message lost by 1 for console
	     */
	    tty_log_lostcount(console, 1);
	}
    } else
	++log_drop_count;

    /*
     * Logging synchronous: increment message lost count to all ttys
     */
    incr_log_lostcount(LOG_DEBUG);
    va_end(args);
}

/*
 * logger_ttydel
 * Remove an entry from the logttyQ.
 */

void logger_ttydel (tt_soc *tty)
{
    ttylisttype *tp;

    for (tp = (ttylisttype *)logttyQ.qhead; tp; tp = tp->next) {
	if (tp->tty == tty) {
	    unqueue(&logttyQ, tp);
	    tty->capabilities &= ~LOGGING;
	    tty->def_capabilities &= ~LOGGING;
	    free(tp);
	    return;
	}
    }
}

/*
 * logger_vtyaddress
 * Does the given IP address belong to one of the vty's to which
 * we are logging?
 */

boolean logger_vtyaddress (ipaddrtype address)
{
    ttylisttype *tp;

    for (tp = (ttylisttype *)logttyQ.qhead; tp; tp = tp->next) {
	if (tp->addr != 0L && tp->addr == address)
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * logger_clear
 *
 * Clear any messages in the logger buffer
 */
void logger_clear (void)
{
    leveltype level;

    level = raise_interrupt_level(ALL_DISABLE);
    log_buffer_begin = 0;
    log_buffer_chars = 0;
    reset_interrupt_level(level);
};


/*
 * logger_process_queue
 *
 * Process any messages on the incoming queue.
 */
static void logger_process_queue (void)
{
    paktype *pak;
    int chars_to_copy, offset;
    uchar err;
    uchar *iptr, *optr;
    ushort count;
    ttylisttype *tp;

    count = PAK_DISPLAY_COUNT;
    while ((pak = log_dequeue())) {
	err = pak->unspecified[0];
	/*
	 * Are we logging to a buffer?
	 */
	if (err <= log_buffer) {
	    log_buffer_count += pak->unspecified[1];
	    if (log_buffer_addr) {
                /*
		 * Figure out size of data to copy;
		 *   do not allow it to overwrap the max buffer size.
                 * Also, determine where we want to begin copying from,
                 *   adjusting it, if necessary, so that we copy no
                 *   more than the max buffer size worth of data.
		 */
		chars_to_copy = pak->datagramsize;
                if (chars_to_copy == 0)
		    return;
		if (chars_to_copy > log_buffer_size) {
		    iptr = pak->datagramstart + chars_to_copy -
			log_buffer_size;
		    chars_to_copy = log_buffer_size;
		} else
		    iptr = pak->datagramstart;
                /* 
                 * Determine the data destination, 
                 *   being mindful of ring buffer wrap-around
                 */
                offset = log_buffer_begin + log_buffer_chars;
                if (offset >= log_buffer_size)
                    offset -= log_buffer_size;
                optr = &log_buffer_addr[offset];
		/*
		 * Perform the copy, adjusted for ring-buffer wraparound
		 */
		if ((optr + chars_to_copy) >
                    (log_buffer_addr + log_buffer_size)) {
		    offset = optr + chars_to_copy - log_buffer_addr -
		        log_buffer_size;
  		    bcopy(iptr, optr, chars_to_copy - offset);
		    bcopy(iptr + chars_to_copy - offset, log_buffer_addr,
		        offset);
	        } else
		    bcopy(iptr, optr, chars_to_copy);
		/*
		 * Recalculate log buffer beginning and char count
		 */
                log_buffer_chars += chars_to_copy;
		if (log_buffer_chars > log_buffer_size) {
		    log_buffer_begin += (log_buffer_chars - log_buffer_size);
                    if (log_buffer_begin >= log_buffer_size)
                      log_buffer_begin -= log_buffer_size;
		    log_buffer_chars = log_buffer_size;
                }
	    }
	}

	/*
	 * Are we logging to the console?
	 */
	if (err <= log_console) {
	    log_console_count += pak->unspecified[1];
	    tty_send_logmessage(console, pak, pak->unspecified[0],
				pak->unspecified[1]);
	}

	/*
	 * Are we logging to TTYs?
	 */
	if (err <= log_monitor) {
	    for (tp = (ttylisttype *)logttyQ.qhead; tp; tp = tp->next) {
		log_monitor_count += pak->unspecified[1];
		tty_send_logmessage(tp->tty, pak, pak->unspecified[0],
				    pak->unspecified[1]);
		tp->count += pak->unspecified[1];
	    }
	}

	if (err <= log_traps) {
	    char* msgp;
	    char* endp;
	    char buffer[20];
	    uint buflen, msglen;

	    msgp = (char*) pak->datagramstart;
	    endp = (char*) &pak->datagramstart[pak->datagramsize];
	    for (;;) {
		if (*msgp == '\n')
		    ++msgp;
		msglen = 0;
		while ((msgp + msglen < endp) && (msgp[msglen] != '\n'))
		    ++msglen;
		buflen = sprintf(buffer, "<%u>%u: ",
				 LOG_FAC(log_facility) | err,
				 ++log_traps_count);
		(void) reg_invoke_log_send(buffer, buflen, msgp, 
					   msglen, err);
		msgp += msglen;
		if (msgp >= endp)
		    break;
	    }
	}
	datagram_done(pak);
	if (--count == 0)
	    break;
    }
}

/*
 * logger
 * System logger process. Looks for messages on incoming
 * queue and outputs them to console or sends a udp datagram
 * to syslog processes on remote systems.
 */

static process logger (void)
{
    ulong major, minor;

    process_wait_on_system_init();
    process_watch_queue(lQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&lastlogclock, ENABLE);

    for (;;) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		logger_process_queue();
		break;

	      case TIMER_EVENT:
		mgd_timer_stop(&lastlogclock);
		if (lastlogpak) {
		    leveltype SR;

		    SR = raise_interrupt_level(ALL_DISABLE);
		    process_enqueue_pak(lQ, lastlogpak);
		    lastlogpak = NULL;
		    reset_interrupt_level(SR);
		}
		break;

	      default:
		/* This would normally contain an errmsg call, but since
		   this code is in the logger, that's a no-no. */
		break;
	    }
	}
    }
}

/*
 * logger_start
 *
 * Actually start up the logger process.
 */
void logger_start (void)
{
    logger_pid = process_create(logger, "Logger", NORMAL_STACK, PRIO_LOW);
}

/*
 * emit traceback as a debugging message
 */
void
bugtrace (void)
{
    char frames[TRACEBACK_SIZE];

    traceback(frames, 1 /* skip this frame */);
    buginf(frames);
    return;
}

/*
 * generate stack trace to buffer
 */
static void
traceback (char *buf, int frames2skip)
{
    int ix, nframes;
    ulong *frame, *pc;

    nframes = LOG_TRACE_MAX + frames2skip;
    buf += sprintf(buf, "%s", traceback_prefix);

    if (schedflag && systeminit_flag && !onintstack()) {
        nframes = LOG_TRACE_MAX;
        frames2skip = 0;
        frame = forkx->savedsp;
        pc = (ulong*) forkx->callee_pc;
    } else {
        nframes = LOG_TRACE_MAX + frames2skip;
        frame = caller_frame();
        pc = (ulong*) traceback;
    }

    for (ix = 0; ix < nframes; ++ix) {
	frame = trace_caller(frame, &pc);
	if (frame == NULL)
	    break;
	if (ix >= frames2skip)
	    buf += sprintf(buf, "%x ", pc);
    }
    *--buf = '\0';		/* dump trailing space */
    return;
}

#if 0			/* for *temporary* development use only! */
/*
 * bugpak
 * Write the first 50 data bytes in a pak (include encapsulation) to the
 * console.  This is very ugly and should never get seen outside of
 * development.   Name is an identifier that is prepended to packet stream.
 */

#define BUGPAK_BYTES 100

void bugpak (char *name, paktype *pak)
{
    paktype *newp;
    uchar *dp, *sp, *last;

    if (log_init && 
	!process_is_queue_full(lQ) &&
	((LOG_DEBUG > log_console) || !QUEUEFULL(&console->messageQ))) {
	newp = getbuffer(LOGDATA);
	if (newp) {
	    newp->datagramstart = newp->data_area;
	    newp->unspecified[0] = LOG_DEBUG;
	    strcpy((char *)&newp->datagramstart[0], name);
	    dp = &newp->datagramstart[strlen(name)];
	    last = pak->datagramstart + min(pak->datagramsize, BUGPAK_BYTES);
	    for (sp = pak->datagramstart; sp < last; sp++) {
		sprintf((char *)dp, "%02x", *sp);
		dp += 2;
	    }
	    newp->datagramsize = dp - newp->datagramstart;
	    log_enqueue(newp);
	}
    } else {
	++log_drop_count;
    }
}
#endif


/*
 * logger_drops
 *
 * Gets the count of syslog messages dropped
 */
long logger_drops (void)
{
    return(log_drop_count);
}
