/* $Id: cfg_int_fair-queue.h,v 3.3.6.1 1996/04/17 13:40:04 fred Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_fair-queue.h,v $
 *------------------------------------------------------------------
 * Interface specific configuration commands for fair queueing
 *
 * March 1995, Fred Baker
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_fair-queue.h,v $
 * Revision 3.3.6.1  1996/04/17  13:40:04  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/04  10:49:43  fred
 * CSCdi47191:  MBRI cannot disable WFQ - screws up multilink
 *         enable WFQ use by any link that cares t
 *
 * Revision 3.2  1995/11/17  09:26:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/09/07  04:31:51  fred
 * CSCdi39816:  interfaces requiring secondary queuing are broken.
 *              This includes tunnels, LAPB, X.25, Loopback, and a few
 *              others
 *
 * Revision 2.5  1995/08/08  19:28:42  fred
 * CSCdi36704:  IP Fastswitching / WFQ cleanup for high-end
 *         enable WFQ/PQ/CQ of SMDS interfaces.
 *         adjust WFQ discard policy to permit one conversation to use the
 *         entire congestive discard threshold
 *
 * Revision 2.4  1995/08/01  03:50:45  fred
 * CSCdi36704:  disable use of fair, custom, priority queuing on SMDS,
 *              as the way the serial number of the message is generated
 *              precludes reordering messages in the output queue.
 *
 * Revision 2.3  1995/07/27  10:57:51  irfan
 * CSCdi37751:  Weighted Fair Queueing not configurable over Async
 * interfaces
 * Fix it. Relax the idb_ephemeral check that was barring async idbs out
 * of the chain
 *
 * Revision 2.2  1995/07/10  05:39:03  fred
 * CSCdi36851:  disable fancy queuing if using autonomous, sse, flow, or
 * optimal switching - ie, only use it if process switching or fast
 * switching. Applies to AGS/7000/7500 platforms.
 *
 * Revision 2.1  1995/07/02  10:53:08  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:04:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *      fair-queue [<discard threshold>
 *      		 [<dynamic queue count> [<reserved queue count>]]]
 *
 *	OBJ(int,1) = discard threshold
 *	OBJ(int,2) = dynamic queue count
 *	OBJ(int,3) = reserved queue count
 */
EOLS (ci_fairq_eol, prioritygroup_command, QUEUE_FAIR);

SET(ci_fairq_reserved_default, ci_fairq_eol, OBJ(int,3),
    FAIR_RSVP_QUEUES_DEFAULT);
NUMBER(ci_fairq_reserved, ci_fairq_eol, ci_fairq_reserved_default, 
       OBJ(int,3), 0, 1000, "Number Reservable Conversation Queues");

SET(ci_fairq_dynamic_default, ci_fairq_reserved_default, OBJ(int,2),
    FAIR_QUEUE_HASH_QUEUES_DEFAULT);
NUMBER(ci_fairq_dynamic, ci_fairq_reserved, ci_fairq_dynamic_default, 
       OBJ(int,2), 16, 4096, "Number Dynamic Conversation Queues");

SET(ci_fairq_threshold_default, ci_fairq_dynamic_default, OBJ(int,1),
    FAIR_QUEUE_DEFAULT_QDEPTH);
NUMBER(ci_fairq_threshold, ci_fairq_dynamic, ci_fairq_threshold_default, 
       OBJ(int,1), 1, FAIR_QUEUE_MAXIMUM_QDEPTH,
       "Congestive Discard Threshold");

NVGENS (ci_fairq_nvgen, ci_fairq_threshold, prioritygroup_command, QUEUE_FAIR);
KEYWORD (ci_fairq, ci_fairq_nvgen, no_alt,
	 "fair-queue", "Enable Fair Queuing on an Interface", PRIV_CONF);
ASSERT (ci_fairq_idb_check_x25, ci_fairq, ALTERNATE,
	fair_queue_supported(csb->interface->hwptr));

#undef	ALTERNATE
#define	ALTERNATE	ci_fairq_idb_check_x25
