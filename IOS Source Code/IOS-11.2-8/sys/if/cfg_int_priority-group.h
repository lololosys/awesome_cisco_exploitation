/* $Id: cfg_int_priority-group.h,v 3.2.60.2 1996/06/19 00:26:00 carvalho Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_priority-group.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ P R I O R I T Y G R O U P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_priority-group.h,v $
 * Revision 3.2.60.2  1996/06/19  00:26:00  carvalho
 * CSCdi33844:  X25 over B-channel broken
 * Branch: California_branch
 *
 * When changing encapsulations, revert to default queueing method if
 * current queueing method is illegal for new encapsulation.
 *
 * Revision 3.2.60.1  1996/03/18  20:13:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:35:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  14:22:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:26:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:02:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/08  19:28:43  fred
 * CSCdi36704:  IP Fastswitching / WFQ cleanup for high-end
 *         enable WFQ/PQ/CQ of SMDS interfaces.
 *         adjust WFQ discard policy to permit one conversation to use the
 *         entire congestive discard threshold
 *
 * Revision 2.2  1995/08/01  03:50:45  fred
 * CSCdi36704:  disable use of fair, custom, priority queuing on SMDS,
 *              as the way the serial number of the message is generated
 *              precludes reordering messages in the output queue.
 *
 * Revision 2.1  1995/07/02  10:53:10  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/21  18:16:55  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:06:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *	priority-group <1-16>
 */
PARAMS_KEYONLY (ci_prioritygroup, NONE,
		"priority-group", OBJ(int,1), 1, PRIORITY_MAXTYPE-1,
		prioritygroup_command, QUEUE_PRIORITY,
		"Assign a priority group to an interface", "Priority group", 
		PRIV_CONF);

ASSERT (ci_priorityg_idb_check, ci_prioritygroup, ALTERNATE,
	priority_queue_supported(csb->interface->hwptr));

#undef	ALTERNATE
#define	ALTERNATE	ci_priorityg_idb_check
