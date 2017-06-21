/* $Id: msg_s4t68360.h,v 3.7.18.4 1996/08/28 13:07:34 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/msg_s4t68360.h,v $
 *------------------------------------------------------------------
 * msg_s4t68360.h - Declaration of VIP 4T Serial Port Adapter messages
 *
 * 03/17/95     H.M. Robison     Created.
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_s4t68360.h,v $
 * Revision 3.7.18.4  1996/08/28  13:07:34  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.18.3  1996/08/22  18:45:39  dujihara
 * CSCdi65825:  Error messages do not specify slot/subunit
 * Branch: California_branch
 * Added slot (bay) and unit to error messages.
 *
 * Revision 3.7.18.2  1996/06/18  06:45:17  amcrae
 * CSCdi60053:  Ping on 4T fails with MTU 8000 on vip2
 * Branch: California_branch
 * VIP2 transmit ring enlarged. Sanity check for many particled packets
 *
 * Revision 3.7.18.1  1996/06/07  04:08:24  amcrae
 * CSCdi56758:  4T does not recover after power surge - should be restarted
 * Branch: California_branch
 * Added structure to carry 4T PA data. Added periodic watchdog check
 * and restart handling. Nuked some unused counters.
 *
 * Revision 3.7  1996/02/11  20:24:17  rharagan
 * CSCdi48311:  4T PA will hang rvip if download fails
 * Read back cksm after '360 down load, purge magic numbers from init block.
 *
 * Revision 3.6  1996/02/09  01:54:00  rharagan
 * CSCdi48311:  4T PA will hang rvip if download fails
 * Use variable delay to bound timeout for '360 download and init.
 *
 * Revision 3.5  1996/02/08  02:16:47  rharagan
 * CSCdi48368:  4T driver should throw away one byte tx pkts
 * Throw'em away, don't propagate them.
 *
 * Revision 3.4  1996/01/20  04:59:32  amcrae
 * CSCdi47215:  4T exception processing doesnt signal host
 * CSCdi44907:  Fixed macros overlooked in previous commit
 *
 * Revision 3.3  1995/12/02  05:50:31  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.2  1995/11/17  18:55:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:54  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
