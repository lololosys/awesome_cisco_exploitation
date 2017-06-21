/* $Id: smrp_globals.h,v 3.2 1995/11/17 18:53:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_globals.h,v $
 *------------------------------------------------------------------
 * Globals for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_globals.h,v $
 * Revision 3.2  1995/11/17  18:53:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/11  19:02:15  mleelani
 * CSCdi41471:  Add support for pruning duplicate paths
 * o Check if we are the right parent when we receive member
 * responses.
 * o Assign PortParentaddresses on other ports correctly, when
 * better routes are received.
 * o Negotiate portparents using PortParentUnreachable Vector,
 * when a new port comes up.
 *
 * Revision 2.4  1995/08/30  22:43:30  mleelani
 * CSCdi38436:  SMRP: Duplicate paths when the previous parent poisons the
 * route.
 * Wrong Port Parent addresses get assigned during race conditions; these
 * lead to multiple data paths.
 *
 * Revision 2.3  1995/07/24  22:30:50  hampton
 * Further work to parts of Appletalk to make it use the passive timer
 * macros for all its timers.  It no longer have any direct references to
 * the system clock.  [CSCdi37581]
 *
 * Revision 2.2  1995/06/14 22:54:59  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_GLOBALS_H__
#define __SMRP_GLOBALS_H__

/*
 *------------------------------------------------------------------
 * SMRP Global Data
 *------------------------------------------------------------------
 */
extern watched_queue *smrpNodeQ;
extern watched_queue *smrpRouteQ;
extern watched_queue *smrpbgQ;

extern tinybool smrp_running;
/* kleung - testing purposes */
extern tinybool smrp_test_running;

extern ulong *smrp_stat;

/*
 *------------------------------------------------------------------
 * SMRP Protocol Variables
 *------------------------------------------------------------------
 */
extern ulong smrp_requestRetries;
extern ulong smrp_requestInterval;
extern ulong smrp_responseInterval;
extern ulong smrp_creatorInterval;
extern ulong smrp_memberInterval;
extern ulong smrp_helloInterval;
extern ulong smrp_nodeAgeInterval;
extern ulong smrp_purgeRouteInterval;

void smrp_start(void);
void smrp_stop(void);

#endif __SMRP_GLOBALS_H__
