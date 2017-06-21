/* $Id: mon_defs.h,v 3.3.54.1 1996/03/24 03:12:23 hochan Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/mon_defs.h,v $
 *------------------------------------------------------------------
 * mon_defs.h
 *
 * December 1994, Rob Clevenger
 *
 * Copyright (c) 1994 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Miscellaneous common monitor defines.
 *------------------------------------------------------------------
 * $Log: mon_defs.h,v $
 * Revision 3.3.54.1  1996/03/24  03:12:23  hochan
 * CSCdi52457:  Modify MINOR Version Number for 112
 * Branch: California_branch
 *
 * Revision 3.4  1996/03/23  19:00:00  hochan
 * Branch: California_branch
 * CSCdi52457:  Changed ROM_MINORVERSION to 2 for California.
 *
 * Revision 3.3  1995/12/01  02:34:04  clev
 * CSCdi44652:  mkpromsrec script missing in these releases
 *
 * Adjust the version number.
 *
 * Revision 3.2  1995/11/17  18:42:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/29  05:04:08  clev
 * CSCdi37286:  Boot version reported by mon_version() is incorrect
 *
 * Set the version to 11.0.
 *
 * Revision 2.1  1995/06/07  22:37:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __MON_DEFS_H__
#define __MON_DEFS_H__
/*
** General defines.
*/
#define ERRLOG_SIZE     (32 * 1024)

#define ESCAPE_CHAR '\033'                      /* escape key code */

/*
 * Version information
 */

#define	ROM_MAJORVERSION 11
#define	ROM_MINORVERSION 2

/*
** The following defines are used by a variety of routines to define
** bus width or number of bytes in a unit.
*/
#define BW_8BITS     1
#define BW_16BITS    2
#define BW_24BITS    3
#define BW_32BITS    4
#define BW_40BITS    5
#define BW_48BITS    6
#define BW_56BITS    7
#define BW_64BITS    8

/*
 * Definitions for possible system states
 */
#define SYSTEM_RUNNING   1              /* running the system code */
#define MONITOR_RUNNING  2              /* in the Monitor          */

#endif /* __MON_DEFS_H__ */


