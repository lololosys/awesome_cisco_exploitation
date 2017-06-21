/* $Id: logger_private.h,v 3.1.2.1 1996/06/19 04:44:02 smackie Exp $
 * $Source: /release/112/cvs/Xsys/os/logger_private.h,v $
 *------------------------------------------------------------------
 * logger_private.h - Private system error message logger definitions
 *
 * June 1996, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: logger_private.h,v $
 * Revision 3.1.2.1  1996/06/19  04:44:02  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.1  1996/06/07  21:34:05  smackie
 * Add placeholders for logger cleanup
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __LOGGER_PRIVATE_H__
#define __LOGGER_PRIVATE_H__

/*
 * Defines
 */

#define LOG_FAC(i)	((i)<<3)

#define	LOG_BUFFERSIZE	4096
#define LOG_MAXMESSAGES 100

/*
 * Define the per-tty data logging structures.
 */
#define LOGDATA		MEDDATA
#define PAK_DISPLAY_COUNT  10

typedef struct ttylisttype_ {
    struct ttylisttype_ *next;		/* ptr to next entry */
    ipaddrtype addr;			/* address if VTY otherwise 0 */
    tt_soc *tty;			/* ptr to tty */
    ulong count;			/* number of messages */
} ttylisttype;

/*
 * Externs
 */

extern int log_facility;
extern queuetype logttyQ;
extern int log_console;
extern int log_buffer;
extern int log_monitor;
extern int log_traps;
extern ulong log_console_count;
extern ulong log_buffer_count;
extern ulong log_monitor_count;
extern ulong log_traps_count;
extern ulong log_drop_count;
extern ulong log_overrun_count;
extern ulong log_flush_count;
extern uchar *log_buffer_addr;
extern uint   log_buffer_size;

/*
 * Prototypes
 */

extern int  logger_get_default_level(int device);
extern uint logger_get_default_buffer_size(boolean init);
extern void logger_buffer_init(uint size, boolean enabled, boolean init);
extern void logger_print_buffer(tt_soc* tty, boolean domore);

#endif

