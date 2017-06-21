/* $Id: logger.h,v 3.4.50.6 1996/09/13 23:40:02 hampton Exp $
 * $Source: /release/112/cvs/Xsys/h/logger.h,v $
 *------------------------------------------------------------------
 * logger.h -- system logger definitions
 * 
 * January 1987, Greg Satz
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: logger.h,v $
 * Revision 3.4.50.6  1996/09/13  23:40:02  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.4.50.5  1996/08/29  23:30:53  snyder
 * CSCdi67619:  facility obsoleted by CSCdi67083, dump it, save memory
 *              saves 404 bytes from image, 360 from data section
 *              also pre-commit hamptons error message doc macros, so
 *              between the 2 of us we only touch logger.h one time
 * Branch: California_branch
 *
 * Revision 3.4.50.4  1996/08/28  12:42:56  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.50.3  1996/06/19  04:43:17  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.4.50.2  1996/05/21  06:27:47  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.4.50.1  1996/03/18  19:37:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.18.2  1996/03/09  05:05:00  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.18.1  1996/02/20  14:17:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/11/30  00:02:26  hampton
 * Print a stack traceback before attempting to dump core.  [CSCdi44670]
 *
 * Revision 3.3  1995/11/17  09:12:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:24:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:35:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/23  23:27:52  ronnie
 * CSCdi42665:  Traceback should show more frames
 *
 * Revision 2.2  1995/07/18  06:45:53  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 20:35:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#define CHARS_HEX32	8
#define LOG_TRACE_MAX	16	/* Max depth for PC traceback - making this
				   larger increases the run-time stack usage
				   in some error handling routines */
#define traceback_prefix_string "\n-Traceback= "
#define TRACEBACK_SIZE (sizeof(traceback_prefix_string) + \
			LOG_TRACE_MAX * (CHARS_HEX32 + 1))

/*
 * Log level values
 */

/*
 * These are "special" values
 */

#define LOG_DEFAULT    -2	/* Logging is at default level */
#define LOG_DISABLED   -1	/* Logging is disabled */

/*
 *  Priorities (these are ordered)
 */

#define LOG_EMERG	0	/* system is unusable */
#define LOG_ALERT	1	/* action must be taken immediately */
#define LOG_CRIT	2	/* critical conditions */
#define LOG_ERR		3	/* error conditions */
#define LOG_WARNING	4	/* warning conditions */
#define LOG_NOTICE	5	/* normal but signification condition */
#define LOG_INFO	6	/* informational */
#define LOG_DEBUG	7	/* debug-level messages */

/*
 *  interface logging flags, if TRUE then DISABLE the msg output
 */
#define LOG_INTF_FLAGS_LINK_STATUS	0x00000001    /* UPDOWN/CHANGE msg's*/

#define LOG_INTF_IS_LOGGING_EVENTS_LINK_STATUS(hwidb)    \
    (!((hwidb)->logging_events & LOG_INTF_FLAGS_LINK_STATUS))
    


/*
 * Message facility definitions
 *
 */

/*
 * There is one facility structure per facility. The facdef macro is
 * used to create a facility.
 *
 * The facdef() macro takes one argument; the name of the facility.
 *
 * For instance, to create a facility called DEAD, you would do:
 *
 *	facdef(DEAD);
 *
 * The facility name is used to group related messages together, and should
 * indicate what part of the system the message came from. The debugging
 * boolean was ripped out because it was wasting space and not being
 * used. 
 *
 * If DEFINE_MESSAGES is defined (see "A little bit of magic..." below
 * then the facdef() call declares the facility.  If DEFINE_MESSAGES
 * is not set, then the facdef is a no-op. 
 */

#ifdef DEFINE_MESSAGES
#define facdef(name) \
const char msgfac_##name[] = { #name }
#else
#define facdef(name)
#endif

/*
 * A message is described using the message structure. A message is defined
 * using the msgdef macro, and the symbol for a message is constructed using
 * the msgsym macro.
 *
 * The msgdef() macro takes 5 parameters:
 *	name - The name of the message
 *	facility - The name of the facility
 *	severity - the LOG_xxx severity level of the message
 *	flags - Various flags associated with this message
 *	format - A printf() style format string for the message
 *
 * The name of the message should be something easy to see and remember; the
 * idea being that a customer may be asked for it over the phone and may not
 * have the console output handy.
 * 
 * The facility comes from a facility defined by the facdef() macro. A
 * combination of the message name and the facility name is used to create
 * the global symbol for the message.
 *
 * The severity is used to indicate how severe the message is. Configuration
 * commands can control what level of messages actually get displayed.
 *
 * The flags are a bitmap of special operations to perform. They are defined
 * as:
 *
 *	MSG_TRACEBACK - Include a PC trace with the message, containing the
 *			saved PC values for up to eight routines saved on
 *			the stack.
 *
 *	MSG_PROCESS -   Include information about the current running process
 *			with the message. This includes the process name,
 *			processor priority level, and the process ID. This
 *			flag is safe to use on messages which do not run
 *			as part of a process context, such as interrupt
 *			service routines, but is most useful for messages
 *			which are called from process context.
 *
 * The format is a standard printf() format specification for printing out
 * the actual message text.
 *
 * The "msgdef_limit" macro has an additional parameter--the rate
 * limiter.  This defines the minimum interval in milliseconds between 
 * emissions of the message.
 *
 * Here is an example message SCARLET in the DEAD facility:
 *
 *	msgdef(SCARLET, DEAD, LOG_WARNING, MSG_TRACEBACK,
 *	       "Interface %s: I knew without asking she was into the blues");
 *
 * This will create a warning message which includes a PC traceback. The
 * message itself takes a single string argument.
 */

typedef struct message_limiter_ {
    ulong rate_limit;			/* min interval, in msec, or 0 */
    sys_timestamp last_time;		/* Last time we emitted this one */
} message_limiter;

struct message_ {
    const char *fac;
    const char *name;
    const char *format;
    message_limiter *limiter;		/* Pointer to limiter or NULL */
    ushort flags;
    uchar severity;
};

/*
 * A little bit of magic here...  
 * If DEFINE_MESSAGES is defined, then the msgdef() and msgdef_limit() 
 * macros will actually define the messages.  This is used in the
 * msg_<whatever>.c files to generated the definitions, by #defining
 * DEFINE_MESSAGES before including this file.  
 * If DEFINE_MESSAGES is not defined, then the macros will generate
 * appropriate extern declarations.  This is used in files which need
 * extern declarations by #including logger.h, then #including the
 * msg_<whatever>.c which defines the messages.  Because logger.h is
 * protected against being included twice, the include of logger.h in
 * the msg_<whatever>.c after #defining DEFINE_MESSAGES will have no
 * effect. 
 */
#ifdef DEFINE_MESSAGES
/*
 * Actually generate the message declarations.
 */
#define msgdef(name, fac, severity, flags, fmt) \
const message msg_ ## fac ## _ ## name = \
	{ msgfac_ ## fac, #name, fmt,  NULL, flags, severity }

#define msgdef_limit(name, fac, severity, flags, fmt, limit) \
static message_limiter msg_ ## fac ## _ ## name ## _limit = \
	{ limit, timer_no_time }; \
const message msg_ ## fac ## _ ## name = \
	{ msgfac_ ## fac, #name, fmt, \
	      &msg_ ## fac ## _ ## name ## _limit, flags, severity }
#else	
/*
 * DEFINE_MESSAGES not defined, just generate the externs
 */
#define msgdef(name, fac, severity, flags, fmt) \
extern const message msg_ ## fac ## _ ## name;

#define msgdef_limit(name, fac, severity, flags, fmt, limit) \
extern const message msg_ ## fac ## _ ## name;

#endif

/*
 * Error Message Documentation Macros.  These are NOPs for all code
 * compilation.  The documentation group uses them to create the Error
 * Messages Manual.
 */

/*
 * A textual description of the error messages in this section of a file.
 * This macro should always follow immediately after every usage of the
 * facdef() macro.
 *
 * This text will be placed verbatim into the Error Messages manual.
 */
#define msgdef_section(text)

/*
 * Text explaining why an error message occurred.
 *
 * This text will be placed verbatim into the Error Messages manual.
 */
#define msgdef_explanation(text)

/*
 * Text explaining the recommended customer action then this error
 * occurs.
 *
 * This text will be placed verbatim into the Error Messages manual.
 */
#define msgdef_recommended_action(text)

/*
 * This macro tells the documentation group/tools not to include this
 * message in the Error Messages manual.
 */
#define msgdef_do_not_document(text)

/*
 * The name of the DDTS component for this section of code.  Customer
 * Support will use this field when filing a DDTS report on the
 * problem.  DDTS components are used instead of individual engineer's
 * names because engineers migrate between positions over time.
 *
 * This text is for cisco internal use only.
 */
#define msgdef_ddts_component(text)

/*
 * Details to further explain the problem and what actions CE should
 * take.  This field should be used to educate CE  on why a problem
 * occurs, and how to prevent it if possible.
 *
 * This text is for cisco internal use only.
 */
#define msgdef_tac_details(text)

/*
 * What information the customer must gather and provide to CE on the
 * initial support call.  Having all necessary information up front
 * will cut down on the number of phone call and the number of
 * frustrated customers.
 *
 * This text is for cisco internal use only.
 */
#define msgdef_required_info(text)


/*
 * The msgsym() macro is used to create a global symbol for a message. This
 * is used so that the naming convention can be changed without changing
 * any of the references.
 *
 * The msgsym() macro is used in calling the message facility. For
 * example: 
 *
 *	errmsg(&msgsym(SCARLET, DEAD), idb->namestring);
 *
 * Note the use of the & operator in the use of msgsym(). This is
 * required since errmsg() expects a *pointer* to the message
 * structure. 
 *
 * NB:  The msgsym() macro is no longer used to create extern
 * declarations, as this is handled automatically by the msgdef()
 * macro when DEFINE_MESSAGES is not defined.
 */

#define msgsym(msg, fac) msg_ ## fac ## _ ## msg

/*
 * msg_setlimit
 *
 * Sets the rate limit for a message at runtime.
 */
#define msg_setlimit(msg, fac, limit) { \
    if (msgsym(msg, fac).limiter != NULL) \
	msgsym(msg, fac).limiter->rate_limit = (limit);}

/*
 * Useful default times for rate limited messages
 */
#define MSGDEF_LIMIT_GLACIAL (ONEMIN)
#define MSGDEF_LIMIT_SLOW    (ONESEC*30)
#define MSGDEF_LIMIT_MEDIUM  (ONESEC*5)
#define MSGDEF_LIMIT_FAST    (ONESEC)

/*
 * Define message flags
 */

#define MSG_TRACEBACK	0x0001		/* Include PC traceback with message */
#define MSG_PROCESS	0x0002		/* Include process info with message */

/*
 * Declare external routines for message system
 */

/*
 * Prototypes
 */
extern char *traceback_prefix;
extern ulong service_timestamps[];

extern boolean ippkt_check(paktype *, boolean, uint);
extern void logger_ttydel(tt_soc *);
extern void bugpak(char *, paktype *);
extern void bugtrace(void);
extern void errmsg(const message *, ...);
extern void errvarmsg(const message *, char *, ...);
extern char *errfmt(const message *, char *, int, ...);
extern void logger_init(void);
extern void logger_start(void);
extern void logger_flush(void);
extern long logger_drops(void);
extern void logger_clear(void);
extern char *logger_level(int level);
extern void monitor_command(parseinfo *);
extern boolean logger_vtyaddress(ipaddrtype);

typedef void (*kernel_buginf_t)(const char *, ...);
extern kernel_buginf_t kernel_buginf;
extern void kernel_set_buginf_hook(kernel_buginf_t new_hook);

typedef void (*kernel_errmsg_t)(const message *, ...);
extern kernel_errmsg_t kernel_errmsg;
extern void kernel_set_errmsg_hook(kernel_errmsg_t new_hook);

/*
 * logger_chain.c
 */
extern void logger_parser_init(void);

#endif		/* __LOGGER_H__ */
