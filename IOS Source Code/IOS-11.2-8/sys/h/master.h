/* $Id: master.h,v 3.5.46.3 1996/05/09 14:13:27 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/h/master.h,v $
 *------------------------------------------------------------------
 * master.h  -- master definitions
 *
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: master.h,v $
 * Revision 3.5.46.3  1996/05/09  14:13:27  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.46.2.16.1  1996/04/27  06:35:27  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.46.2.6.1  1996/04/08  01:45:59  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.46.2  1996/03/24  02:44:46  hochan
 * CSCdi52457:  Modify MINOR Version Number for 112
 * Branch: California_branch
 *
 * Revision 3.5.46.2  1996/03/23  18:40:00  hochan
 * Branch: California_branch
 * Modify the MINOR VERSION NUMBER to 2 for California.
 *
 * Revision 3.5.46.1  1996/03/18  19:37:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.50.1  1996/03/25  02:19:05  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.28.1  1996/03/20  23:46:43  cakyol
 * The 11.2 LECS parser can now FULLY recognize pre 11.2 LECS
 * commands .  As a result of this, we also have a new systemwide
 * global uint variable named "global_nvconfig_version", which
 * reflects the nvram version number in it.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1995/12/08  19:25:54  smackie
 * Move memory size defines into a central place. (CSCdi45214)
 *
 * Revision 3.4  1995/12/03  17:56:30  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.3  1995/11/22  05:41:48  eschaffe
 * CSCdi42241:  PROTOCONVERT is TRUE for images that don't include the PT
 * option.
 * Obsolete PROTOCONVERT flag in makefiles.  The control for defining
 * protocolconversion for an image is now platform_specific.
 *
 * Revision 3.2  1995/11/17  09:12:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:58:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/30  05:01:46  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 * Revision 2.4  1995/07/06  06:46:37  tli
 * CSCdi36749:  Route lookups insufficiently recursive
 * Increase recursion limit to 8.
 *
 * Revision 2.3  1995/06/16  17:07:27  dkatz
 * CSCdi35956:  Timer declarations should be in a separate file
 * Move passive timer declarations to a new file.  Eliminate some obsolete
 * macros.
 *
 * Revision 2.2  1995/06/16  04:27:01  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  20:35:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__MASTER_H__)
#define __MASTER_H__

#include "types.h"		/* Define the major types first */

#ifndef __GNUC__
#define	volatile
#endif

#ifdef lint
#define inline
#endif


/*
 * Target platform definitions
 */

#ifdef TARGET_CISCO
#include "target_cisco.h"
#endif

#ifdef TARGET_UNIX
#include "target_unix.h"
#endif


/*
 * Global platform definitions
 */
#include "platform.h"


/*
 * Feature defaults.
 * Unless defined by gcc, these software functions are not present.
 * These switches are NOT to be used to conditionally assemble code.
 * They are for initializing static variables in the main_xxx.c modules
 * "GLOBAL_DEBUG" is not defined if we are building a field version.
 */

#ifndef	SMDS_ENABLE
#define	SMDS_ENABLE	FALSE
#endif
#ifndef X25_ENABLE
#define X25_ENABLE FALSE
#endif
#ifndef DDN_ENABLE
#define DDN_ENABLE FALSE
#endif
#ifndef BRIDGE_ENABLE
#define BRIDGE_ENABLE FALSE
#endif
#ifndef SRSWITCH_ENABLE
#define SRSWITCH_ENABLE FALSE
#endif
#ifndef LAT_ENABLE
#define LAT_ENABLE FALSE
#endif
#ifndef TROUTER_ENABLE
#define TROUTER_ENABLE FALSE
#endif
#ifndef ROM_ENABLE
#define ROM_ENABLE FALSE
#endif
#ifndef CISCOPRO_IMAGE
#define CISCOPRO_IMAGE FALSE
#endif


/*
 * Common definitions and macros
 */
#define SYS_MAJORVERSION	11
#define SYS_MINORVERSION	2
#define SYSTEM_VERSION		(SYS_MAJORVERSION*256 + SYS_MINORVERSION)
#define SYS_BRANCH 0x8000

/*
 * Basic constants.
 */
#define MAXCHAR   0x7F			/* largest positive 8-bit signed num */
#define MAXUCHAR  0xFF			/* largest positive 8-bit unsigned */
#define	MAXSHORT  0x7FFF		/* largest positive 16-bit number */
#define	MAXUSHORT 0xFFFF		/* largest positive 16-bit number */
#define MAXINT    0x7FFFFFFFL		/* largest positive 32-bit number */
#define MAXUINT   0xFFFFFFFFL		/* largest unsigned 32-bit number */
#define MAXLONG   MAXINT		/* largest positive 32-bit number */
#define MAXULONG  MAXUINT		/* largest unsigned 32-bit number */
#define MINCHAR   0x80			/* smallest possible signed 8-bit */
#define	MINSHORT  0x8000		/* smallest possible signed 16-bit */
#define	MININT	  0x80000000L		/* smallest possible signed 32-bit */
#define	MINLONG	  MININT		/* smallest possible signed 32-bit */

/*
 * Basic memory sizes
 */

#define ONE_K            0x00000400
#define TWO_K            0x00000800
#define FOUR_K           0x00001000
#define EIGHT_K          0x00002000
#define SIXTEEN_K        0x00004000
#define THIRTYTWO_K      0x00008000
#define SIXTYFOUR_K      0x00010000

#define HALF_MEG         0x00080000
#define ONE_MEG          0x00100000
#define TWO_MEG          0x00200000
#define FOUR_MEG         0x00400000
#define EIGHT_MEG        0x00800000
#define SIXTEEN_MEG      0x01000000
#define THIRTYTWO_MEG    0x02000000
#define SIXTYFOUR_MEG    0x04000000

#define ONE_GIG          0x40000000

/*
 * Miscellaneous Macros
 */
#define btow(n) (((n)+1) >> 1)		/* bytes to shorts */
#define wtob(n) ((n) << 1)		/* shorts to bytes */
#define ltob(n) ((n) << 2)		/* longs to bytes */
#define btol(n) ((n) >> 2)		/* bytes to longs */

#define max(a,b) (((a) > (b)) ? (a) : (b)) /* max function */
#define min(a,b) (((a) < (b)) ? (a) : (b)) /* min function */

#define offsetof(str,sym) ((int)&(((str *)0)->sym)) /* Offset into structure */

#define ROUTINGDEPTH 8			/* maximum levels of recursion */
#define NETHASHLEN 256			/* for hashing 32-bit quantities */


/*
 * Doubly-linked queue structure. To use, simply insert this
 * structure into the structure to enqueue/dequeue, then use
 * insque/remque routines in os/util.c.  For an ordered queue,
 * use the routines insque_o/remque_o.
 */
typedef struct dqueue_ {
    struct dqueue_	*flink;
    struct dqueue_	*blink;
    void *		parent;
    sys_timestamp	value;
} dqueue_t;

/*
 * Queuetype data structure
 *
 * - defined in types.h
 */
static inline int queuesize_inline (queuetype *q)
{
    return(q->count);
}
static inline boolean queuefull_inline (queuetype *q)
{
    return((q->maximum != 0) && (q->count >= q->maximum));
}
static inline boolean queuefull_reserve_inline (queuetype *q, int reserve)
{
    return((q->maximum != 0) && ((q->count + reserve) >= q->maximum));
}
static inline boolean queueempty_inline (queuetype *q)
{
    return(q->qhead == NULL);
}
static inline int queuemax_inline(queuetype *q)
{
  return(q->maximum);
}

#define QUEUESIZE(q)	queuesize_inline(q)
#define	QUEUEFULL(q)	queuefull_inline(q)
#define	QUEUEFULL_RESERVE(q, reserve) \
			queuefull_reserve_inline(q, reserve)
#define QUEUEEMPTY(q)	queueempty_inline(q)
#define QUEUEMAXIMUM(q)     queuemax_inline(q)

/*
 * Status codes
 */ 

#define ABORTED 0		/* something awful happened */
#define OK 1			/* normal status */
#define NO_DATA 2		/* no input data available */
#define END_OF_FILE 3		/* connection terminated normally */
#define URGENT_DATA 4		/* normal status, but data is urgent */
#define NO_MEMORY 5		/* insufficient local storage */
#define RETRY 6			/* try again */
#define FOREIGN_RESET 7		/* foreign host reset connection */
#define LOCAL_RESET 8		/* local user (manually) reset connection */
#define REJECTED 9		/* foreign host reject SYN */
#define TIMEDOUT 10		/* connection timed out */
#define CARRIER_DROP 11		/* carrier drop by modem or VTY */
#define IPADDRESS 12		/* hostname is an IP address */
#define NAMEISTOOLONG 14	/* hostname string is too long */
#define BADCHARACTERS 15	/* hostname string had illegal characters */
#define UNKNOWN_HOSTNAME 16	/* unknown host name */
#define UNREACHABLE 17		/* destination is unreachable */
#define IDLE_TIMEDOUT 18	/* local timeout event */
#define OPTSTOOBIG 19		/* option bytes too big to use */
#define INUSE 20		/* element exists and is busy */

/*
 * Command type flags.
 * These should really be part of comnd.c when it is installed.
 */

#define CMD_PRV	       0x0001     /* privileged command */
#define CMD_INV	       0x0002     /* never give help */
#define CMD_QUES       0x0004     /* question mark arg means "help" */
#define CMD_HPRV       0x0008     /* help only in privileged mode */
#define CMD_ASM	       0x0010     /* Terminal Server only */
#define CMD_AGS	       0x0020     /* Gateway Server only */
#define	CMD_X25	       0x0040     /* X25 only */
#define CMD_SERIAL     0x0080     /* only if serial network interface */
#define CMD_TOKENRING  0x0100     /* only if token ring interface */
#define CMD_ISDN       0x0200	  /* Only if ISDN interface */

/*
 * config_flags
 * Privilege bits for configuration commands
 */

#define CM_TTY 0x80			/* user settable TTY parameter */ 
#define CM_RNG 0x40			/* a range of lines required */
#define CM_COM 0x20			/* a comment command */
#define CM_INT 0x10			/* interface required */
#define CM_RTR 0x08			/* router required */
#define	CM_BTH 0x04			/* both interface and global */
#define CM_MULT 0x02			/* argument is multiline */

/*
 * The standard time reference for network management applications.
 */
#define SYSUPTIME_UNITS 100	/* Units in Hz */
#define SYSUPTIME_SCALE (ONESEC/SYSUPTIME_UNITS)  /* Units in msclock ticks */


/*********************************************************************
 *
 *			     IOS Includes
 *
 *********************************************************************/

#include "../os/list.h"
#include "sched.h"
#include "passive_timers.h"


/*
 * Global Symbols Definitions
 * Do the include here after system types are defined.
 */

#ifndef GROVEL
#include "globs.h"
#include "version.h"
#endif

#endif /* !defined(__MASTER_H__) */
