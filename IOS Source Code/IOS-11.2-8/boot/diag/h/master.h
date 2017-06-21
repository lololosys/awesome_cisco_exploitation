/* $Id: master.h,v 3.1 1995/11/09 09:06:12 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/master.h,v $
 *------------------------------------------------------------------
 * master.h  -- master definitions
 *
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1992 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Processor specific include files only.
 * Look in defs_xxx.h for processor specific definitions.
 *------------------------------------------------------------------
 * $Log: master.h,v $
 * Revision 3.1  1995/11/09  09:06:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:46  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __GNUC__
#define	volatile
#endif

#ifdef lint
#define inline
#endif

#ifdef STS1
#include "defs_sts1.h"
#endif

#ifdef CSC2
#include "defs_csc2.h"
#endif

#ifdef CSC1
#include "defs_csc1.h"
#endif


/*
 * Feature defaults.
 * Unless defined by gcc, these software functions are not present.
 * These switches are NOT to be used to conditionally assemble code.
 * They are for initializing static variables in the main_xxx.c modules
 * "PRODUCTION" is defined if we are building a field version.
 */

#ifdef PRODUCTION

#ifndef X25_ENABLE
#define X25_ENABLE FALSE
#endif
#ifndef DDN_ENABLE
#define DDN_ENABLE FALSE
#endif
#ifndef BRIDGE_ENABLE
#define BRIDGE_ENABLE FALSE
#endif
#ifndef SRB_ENABLE
#define SRB_ENABLE FALSE
#endif

#endif /* PRODUCTION */

#ifndef PRODUCTION
#define X25_ENABLE TRUE
#define DDN_ENABLE TRUE
#define BRIDGE_ENABLE TRUE
#define SRB_ENABLE TRUE
#endif /* PRODUCTION */

/*
 * Common definitions and macros
 */
#define TRUE 1
#define FALSE 0

#define SYS_MAJORVERSION 7
#define SYS_MINORVERSION 1
#define SYS_BRANCH 0x8000

#define	GETSHORT(num)	    ((num[0] << 8) | num[1])
#define	DELAY(ms)	_delay((ms)*((CYCLETIME/ONESEC)/WAITCYCLES))

#define SLEEPING(then)	(((long)((then) - msclock)) > 0)
#define sleeping(then)	(((long)((then) - msclock)) > 0)
#define AWAKE(then)	(((long)((then) - msclock)) <= 0)

#define btow(n) (((n)+1) >> 1)		/* bytes to shorts */
#define wtob(n) ((n) << 1)		/* shorts to bytes */
#define ltob(n) ((n) << 2)		/* longs to bytes */
#define btol(n) ((n) >> 2)		/* bytes to longs */

#define max(a,b) ((a) >= (b)) ? (a) : (b) /* max function */
#define min(a,b) ((a) < (b)) ? (a) : (b) /* min function */

#define MAXINT 0x7FFFFFFFL		/* largest positive 32-bit number */

#define ROUTINGDEPTH 5			/* maximum levels of recursion */
#define NETHASHLEN 256			/* for hashing 32-bit quantities */


/*
 * Queuetype data structure
 */

#define	QUEUEFULL(q)	((q)->maximum != 0 && (q)->count >= (q)->maximum)
#define QUEUEEMPTY(q)	((q)->qhead == NULL)

typedef struct queuetype_ {
    mempointer qhead;			/* head of queue */
    mempointer qtail;			/* tail of queue */
    int count;				/* possible count */
    int maximum;			/* maximum entries */
} queuetype;


/*
 * charlong data structure
 * Used for manipulating longs, shorts, and bytes.
 */

typedef struct charlong_ {
	union {
	    uchar byte[4];
	    ulong lword;
	    ushort sword[2];
	} d;
    } charlong;

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
#define PUPADDRESS 13		/* hostname is a PUP address */
#define NAMEISTOOLONG 14	/* hostname string is too long */
#define BADCHARACTERS 15	/* hostname string had illegal characters */
#define UNKNOWN 16		/* unknown host name */
#define UNREACHABLE 17		/* destination is unreachable */
#define IDLE_TIMEDOUT 18	/* local timeout event */
#define OPTSTOOBIG 19		/* option bytes too big to use */


/*
 * ROM bootstrap function codes
 */

#include "./emt.h"

/*
 * CPU type codes
 * These match the cpu.h file for the system bootstrap program.
 */

#include "./cpu.h"

/*
 * System restart codes and shared data structure definitions
 */

#include "./reason.h"

/*
 * Command type flags.
 * These should really be part of comnd.c when it is installed.
 */

#define CMD_PRV	  0x1		/* privileged command */
#define CMD_INV	  0x2		/* never give help */
#define CMD_QUES  0x4		/* question mark arg means "help" */
#define CMD_HPRV  0x8		/* help only in privileged mode */
#define CMD_ASM	 0x10		/* ASM only */
#define CMD_AGS	 0x20		/* AGS only */
#define	CMD_X25	 0x40		/* X25 only */
#define CMD_SERIAL 0x80		/* only if serial network interface */
				/* fix uchar arrays for next bit! */

/*
 * Global Symbols Definitions
 * Do the include here after system types are defined.
 */

#include "globs.h"
