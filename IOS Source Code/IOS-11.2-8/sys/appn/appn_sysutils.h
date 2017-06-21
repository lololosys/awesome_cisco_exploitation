/* $Id: appn_sysutils.h,v 3.6 1996/02/27 10:51:13 dcalia Exp $
 * $Source: /release/111/cvs/Xsys/appn/appn_sysutils.h,v $
 *------------------------------------------------------------------
 * Sytem Utilities required in APPN
 *
 * May 1995, Leo Pereira
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appn_sysutils.h,v $
 * Revision 3.6  1996/02/27  10:51:13  dcalia
 * APPN/RSRB needs a configurable bridge number (CSCdi42257)
 *
 * Revision 3.5  1996/02/22  14:46:10  mregan
 * CSCdi48499:  sho appn int filter does not work for unqualified LU names
 * corrected logic for filtering on names
 *
 * Revision 3.4  1996/01/17  11:53:57  dcalia
 * APPN isr performance not adequate (CSCdi32469)
 *
 * Revision 3.3  1995/11/21  07:26:04  lpereira
 * CSCdi44255:  APPN build is broken with the PortReady changes
 * Added function appn_get_timestamp32().
 *
 * Revision 3.2  1995/11/17  08:37:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:52:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/08/12  11:47:29  dcalia
 * Rate limit APPN alerts. Provide API to CLSReturnCodeToString and
 * string_getnext (CSCdi28882)
 *
 * Revision 2.7  1995/08/11  02:22:36  dmccowan
 * CSCdi30561:  need to support APING on our router
 *
 * Revision 2.6  1995/08/02  00:36:37  vtseng
 * CSCdi29693:  no appn activate should not stop appn
 *
 * Revision 2.5  1995/07/27  22:07:50  mregan
 * CSCdi36463:  redo appn_get_current_time: APPN images are broken
 * add timezone support for APPN
 *
 * Revision 2.4  1995/07/26  21:52:19  mregan
 * CSCdi33023:  appn show command CPUHOG message
 * fix CPUMOG messages on large show outputs
 *
 * Revision 2.3  1995/06/27  20:38:46  mregan
 * CSCdi36463:  redo appn_get_current_time: APPN images are broken
 * do the appn_get_current_time() right this time.
 *
 * Revision 2.2  1995/06/26  21:51:45  mregan
 * CSCdi33791:  APPN MS TP can starve other processes when activated
 * implement get_current_time in APPN code.
 *
 * Revision 2.1  1995/06/07  20:03:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __APPN_SYSUTILS_H__
#define __APPN_SYSUTILS_H__

#include "types.h"
#include "plural.h"

/*
 * Memory allocation stuff
 */
void *appn_malloc(uint);
void appn_free(void *);

/* 
 * Manipulating ieee 48 bit addresses
 */
void appn_ieee_copy (uchar const *src, uchar *dst);
void appn_ieee_swap (uchar const *src, uchar *dst);

/*
 * Interactive commands
 */
boolean appn_yes_or_no(char *, boolean, boolean);
/*
 * Timestamp stuff
 */
typedef struct appn_timeval_ {
    u_long year;		/* Year, AD (1993, not 93!) */
    u_long month;		/* Month in year (Jan = 1) */
    u_long day;			/* Day in month (1-31) */
    u_long hour;		/* Hour in day (0-23) */
    u_long minute;		/* Minute in hour (0-59) */
    u_long second;		/* Second in minute (0-59) */
} appn_timeval;

sys_timestamp appn_get_timestamp(void);
ulong appn_get_timestamp32(void);
void  appn_get_secs_and_msecs(word *secs, word *msecs);
ulong appn_elapsed_time(sys_timestamp timestamp);
ulong appn_get_seconds(void);
void  appn_get_current_time(appn_timeval *atv);


/*
 * printing and debugging
 */
#define FMTAREA_SIZE 256
int appn_printf(const char *fmt, ...);
void appn_buginf(const char *fmt, ...);

/*
 * Process Management 
 */
boolean appn_breakpoint(void);
void appn_edisms(void *, ulong);
void appn_tdisms(ulong);
void appn_s_killp(int);
int appn_cfork(void *, long, int, char *, int);
void c_disable(void);
void c_enable(void);

/* need to make this one go away later. [sberl] */
int _doprnt (char *cp, tt_soc *tty, const char *fmt, va_list argp,
	     boolean do_precision);
int printf(const char *fmt, ...);
void buginf(const char *fmt, ...);

/*
 * TFTP file I/O
 */
#define TFTP_OK   0
#define TFTP_ERR  1

int appn_tftp_file_write (char *, ipaddrtype, char *, int);
int appn_tftp_file_read (char *, ipaddrtype, char *, ulong);
boolean appn_automore_quit(void);
int appn_clock_timezone_offset(void);
void appn_flush(tt_soc *);
char *appn_CLSReturnCodeToString(hword RetCode);
char *appn_string_getnext(void);
tr_vgrouptype *appn_vrfind_ringgroup(ushort vrn);
void appn_send_alert_to_host(uchar *buffer);


#define appn_ArgAndPlural ArgAndPlural

#endif /* __APPN_SYSUTILS_H__ */
