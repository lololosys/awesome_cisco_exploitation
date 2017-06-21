/* $Id: buffers.h,v 3.4 1996/02/13 20:39:06 hampton Exp $
 * $Source: /release/111/cvs/Xsys/os/buffers.h,v $
 *------------------------------------------------------------------
 * buffers.h -- definitions for buffer management
 *
 * 26-April-1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: buffers.h,v $
 * Revision 3.4  1996/02/13  20:39:06  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.3  1996/01/29  20:24:56  sgannu
 * CSCdi47755:  Reclaim 45K of unused memory from 1000s
 *
 * Revision 3.2  1995/11/17  18:44:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:43:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  21:18:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/11  12:12:31  tli
 * CSCdi41857:  show buffers variables should be unsigned
 *
 * Revision 2.8  1995/09/13  23:49:40  smackie
 * Rework buffer pool resize logic to allow the "huge" pool to be resized
 * for a user even if buffers are outstanding. (CSCdi38912)
 *
 * Revision 2.7  1995/08/12  07:09:31  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.6  1995/08/08  16:48:23  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.5  1995/07/31 09:18:02  misko
 * CSCdi37919:  DDR needs to allow protocols additional control
 * Add additional controls on a per packet basis for upper layer
 * protocols.  This is to help solve CSCdi33368.  Add new flags
 * to allow the upper layers to override the dialer access lists, so
 * they can specify that a packet is interesting or uninteresting
 * regardless of what the access lists say.  Also, add a flag so that
 * the protocols can tell the dialer not to hold the packet in the holdq
 * if a call is in progress.  Lastly, have a flag to we can tell the
 * protocols if we are dropping the packet (failing encaps) because
 * a call is in progress but not connected.
 *
 * Revision 2.4  1995/07/13  22:18:33  shaker
 * CSCdi35990:  Need packet pool for fast switched packet structure
 * Create a pool that uses the public pools for pak_dup, etc, and put
 * the RSP fast pak structure in it.
 *
 * Revision 2.3  1995/07/06  22:41:11  carvalho
 * CSCdi35036:  X25 data packets are setting D-bit and cause reset
 * requests.  Q-bit can also be set unexpectedly, potentially causing
 * the remote side to ignore the contents of the packet (CSCdi36294).
 *
 * Revision 2.2  1995/06/27  00:47:51  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.1  1995/06/07  21:53:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Define DEBUG to 1 to debug buffers
 */
/* #define DEBUG 1 */

#ifndef __BUFFERS_H__
#define __BUFFERS_H__

/*
 * Set initial size of buffer pools
 */

#define INITELEMENTS 500

/*
 * We create each buffer with some extra bytes at the end.
 * The following is the current sad tale of kludges.
 *
 * The AMD LANCE chip appends on input a 4 byte FCS to the end of
 * each Ethernet packet.
 *
 * SMDS has trailers with a 0-3 byte pad, then 4 bytes.  The Kentrox kludge
 * adds 4 more, for a total of 11 bytes.  Note that the 4 LANCE bytes
 * may be considered a subset of the 11 SMDS bytes.
 *
 * And someone is always making a fencepost error, so add another 4 bytes,
 * and round up to the nearest multiple of 4.
 */
#define TRAILBYTES 68

/*
 * Buffer pool overhead. This is the amount of space that even a 
 * hypothetical zero length buffer would take up.
 */

#define BUFFEROVERHEADBYTES (sizeof(pakdata) + ENCAPBYTES + TRAILBYTES)


/*
 * Buffer pool limits.  These are the standard, default limits.
   Platforms with non-standard limits should add these definitions to
   the apprioriate machine/cisco_xxx.h file, and modifiy the calls to
   pool_adjust() in the appropriate src-xx-yyy/platform_zzz.c file.
 */
#define SMALL_INIT1 25		/* init buffers for terminal server */
#define MID_INIT1 15
#define BIG_INIT1 5
#define VERYBIG_INIT1 0
#define LARGE_INIT1 0
#define HUGE_INIT1 0

#define SMALL_INIT2 50		/* init buffers for routers */
#define MID_INIT2 25
#define BIG_INIT2 50
#define VERYBIG_INIT2 10
#define LARGE_INIT2 0
#define HUGE_INIT2 0

#define SMALL_INIT3 120		/* init buffers for router (CSC/>=3) */
#define MID_INIT3 90
#define BIG_INIT3 90
#define VERYBIG_INIT3 10
#define LARGE_INIT3 10
#define HUGE_INIT3 0

#define SMALL_INIT_BOOT 1	/* init buffers for secondary boot */
#define MID_INIT_BOOT 1
#define BIG_INIT_BOOT 0
#define VERYBIG_INIT_BOOT 0
#define LARGE_INIT_BOOT 0
#define HUGE_INIT_BOOT 0

#define SMALL_MAX1  60		/* maximum free buffers for TS */
#define MID_MAX1 30
#define BIG_MAX1  10
#define VERYBIG_MAX1 10
#define LARGE_MAX1 5
#define HUGE_MAX1 2

#define SMALL_MAX2 150		/* maximum free buffers for GS */
#define MID_MAX2 150
#define BIG_MAX2 150 
#define VERYBIG_MAX2 100
#define LARGE_MAX2 10
#define HUGE_MAX2 4

#define SMALL_MAX3 250		/* maximum free buffers for GS (CSC/>=3) */
#define MID_MAX3 200
#define BIG_MAX3 300
#define VERYBIG_MAX3 300
#define LARGE_MAX3 30
#define HUGE_MAX3 13

#define SMALL_MIN 20		/* minimum free buffers */
#define MID_MIN 10
#define BIG_MIN 5
#define VERYBIG_MIN 0
#define LARGE_MIN 0
#define HUGE_MIN 0

#define HEADER_MIN1  10
#define HEADER_MAX1  100
#define HEADER_INIT1 50



/* 
 * used for "show buffer" display 
 */
#define SHOW_BUF_STATS_LINES        6

/*
 * Prototypes and Externs for os/buffers.c can be found in h/packet.h
 * and h/globs.h
 */

#endif		/* __BUFFERS_H__ */
