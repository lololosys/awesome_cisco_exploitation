/* $Id: vsvc_ss.h,v 3.2.62.1 1996/07/16 10:34:33 ehk Exp $
 * $Source: /release/112/cvs/Xsys/vines/vsvc_ss.h,v $
 *------------------------------------------------------------------
 * vsvc_ss.h - Support for the `Server' Service.
 *
 * April 1994, David Hampton
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vsvc_ss.h,v $
 * Revision 3.2.62.1  1996/07/16  10:34:33  ehk
 * CSCdi58105:  Vines time sync not updating starting epoch
 * Branch: California_branch
 * the original time sync problem reported does not exist in this
 * release. added new vines time sync related debug trace. fixed
 * minor vines time epoch display problem.
 *
 * Revision 3.2  1995/11/17  17:59:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:46:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:16:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**************************************************
 *
 *	      Server Service Definitions
 *
 **************************************************/

#define VSS_ERR_NOTIMP		5015	/* not implemented yet */
#define VSS_ERR_SERVER_INIT	5019	/* Server Initializing */
#define VSS_ERR_READ_FAILED	5021	/* file read failed */

/*
 * Length of various strings that may be returned by the server
 * service.
 */
#define VSS_VERSION_LENGTH	20	/* Printed Version number */
#define VSS_TIME_STRING_LENGTH	64	/* For printing time values */

/*
 * Periodic Timers
 *
 * The normal time interval is that of the time service updates.  The
 * initial number is to get through system startup and hopefully learn
 * some routes before attempting to crank up the time service.
 */
#define VSS_PERIOD		(30 * ONEMIN)
#define VSS_PERIOD_INIT		(30 * ONESEC)

/*
 * Definitions for the Time Service support.
 *
 * VSS_TIME_MAX_DEST is the limit on the number of stations to which we can send
 * explicit time messages.  This number was pulled out of a hat.
 */
#define VSS_TIME_MAX_DEST	20

/*
 * Definitions for the Get Service Summary Statistics transaction.
 *
 * VSS_MAX_SVC_STATS is the maximum number of service statitics included
 * in a single reply packet.  This is an empirical number.
 */
#define VSS_MAX_SVC_STATS	15

/*
 * Definitions for the Get LAN Information transaction.
 *
 * VSS_MAX_LAN_ENTRIES is the maximum number of lan entries included in a
 * single reply packet.  This is an number pulled out of a hat.
 */
#define VSS_MAX_LAN_ENTRIES	10

/*
 * Definitions for the Get Multiplexer Information transaction.
 *
 * VSS_MAX_MUX_ENTRIES is the maximum number of lan entries included in a
 * single reply packet.  This is an number pulled out of a hat.
 */
#define VSS_MAX_MUX_ENTRIES	10

/**************************************************
 *
 *	      Externs
 *
 **************************************************/

extern void vss_set_time_adjustment(ulong);
extern void vss_clear_time_adjustment(void);
extern ulong vss_get_time(void);
extern ulong vss_get_start_time(void);
extern int   vss_sprint_secs_in_dhms(char *text, ulong seconds);
