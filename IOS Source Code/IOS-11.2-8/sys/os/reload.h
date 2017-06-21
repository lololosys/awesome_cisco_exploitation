/* $Id: reload.h,v 3.1 1995/11/09 12:47:52 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/reload.h,v $
 *------------------------------------------------------------------
 * Definitions for the reload module
 *
 * September 1995, Dave Katz
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: reload.h,v $
 * Revision 3.1  1995/11/09  12:47:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/24  00:54:43  dkatz
 * CSCdi40898:  Synchronized reload needed
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* External declarations */

extern long reload_get_delay(void);
extern void reload_cancel(void);
extern boolean reload_in_progress(void);
extern void reload_schedule_with_message(clock_epoch *, ulong , int *, int,
					 int, char *, char *, boolean,
					 boolean);
