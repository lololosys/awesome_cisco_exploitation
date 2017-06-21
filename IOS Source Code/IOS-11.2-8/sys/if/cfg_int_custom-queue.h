/* $Id: cfg_int_custom-queue.h,v 3.2.60.2 1996/06/19 00:25:59 carvalho Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_custom-queue.h,v $
 *------------------------------------------------------------------
 * Interface specific configuration commands for custom queueing
 *
 * February 1993, Bruce Cole
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_custom-queue.h,v $
 * Revision 3.2.60.2  1996/06/19  00:25:59  carvalho
 * CSCdi33844:  X25 over B-channel broken
 * Branch: California_branch
 *
 * When changing encapsulations, revert to default queueing method if
 * current queueing method is illegal for new encapsulation.
 *
 * Revision 3.2.60.1  1996/03/18  20:13:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:34:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  14:22:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:26:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/08  19:28:41  fred
 * CSCdi36704:  IP Fastswitching / WFQ cleanup for high-end
 *         enable WFQ/PQ/CQ of SMDS interfaces.
 *         adjust WFQ discard policy to permit one conversation to use the
 *         entire congestive discard threshold
 *
 * Revision 2.2  1995/08/01  03:50:44  fred
 * CSCdi36704:  disable use of fair, custom, priority queuing on SMDS,
 *              as the way the serial number of the message is generated
 *              precludes reordering messages in the output queue.
 *
 * Revision 2.1  1995/07/02  10:53:07  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:04:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *	custom-queue <1-10>
 */
PARAMS_KEYONLY (ci_customq, no_alt,
		"custom-queue-list", OBJ(int,1), 1, PRIORITY_MAXTYPE-1,
		prioritygroup_command, QUEUE_CUSTOM,
		"Assign a custom queue list to an interface",
		"Custom queue list number", 
		PRIV_CONF);

ASSERT (ci_customq_idb_check, ci_customq, ALTERNATE,
	custom_queue_supported(csb->interface->hwptr));

#undef	ALTERNATE
#define	ALTERNATE	ci_customq_idb_check
