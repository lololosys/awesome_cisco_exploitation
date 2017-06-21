/*
 *	P A R S E R _ D E F S _ P R I O R I T Y . H
 *
 * $Id: parser_defs_priority.h,v 3.2.62.2 1996/07/02 08:12:57 agt Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_defs_priority.h,v $
 * 
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_priority.h,v $
 * Revision 3.2.62.2  1996/07/02  08:12:57  agt
 * CSCdi46873:  queue <n> lowest-custom <m> command not saved to NVRAM
 * Branch: California_branch
 * Fix NV generation of 'lowest-custom' command.
 *
 * Revision 3.2.62.1  1996/04/17  13:46:31  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:52:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef QUEUE_FCFS
/*
 * priority-list arguments
 */
#define PROTO_APPLETALK          1
#define PROTO_APOLLO             2
#define PROTO_BRIDGE             3
#define PROTO_CLNS               4
#define PROTO_DECNET             5
#define PROTO_IP                 6
#define PROTO_NOVELL             7
#define PROTO_VINES              8
#define PROTO_XNS                9
#define PROTO_STUN              10

#define PRIORITY_UNSPEC		 0
#define PRIORITY_FRAGS		 4
#define PRIORITY_GT              5
#define PRIORITY_LT              6
#define PRIORITY_LIST            7
#define PRIORITY_TCP             8
#define PRIORITY_UDP             9
#define PRIORITY_ADDRESS	10

/*
 * Note: these must NOT start at 0 in order for the "priority list changed"
 *       callback mechanism to work.  See priority_mark_list_changed.
 */
#define PRICMD_START_SUBFN	 1
#define PRICMD_PROTOCOL          1
#define PRICMD_DEFAULT           2
#define PRICMD_QLIMIT            3
#define PRICMD_INTERFACE         4
#define PRICMD_STUN              5
#define PRICMD_QCUSTOM		 6
#define PRICMD_LOWEST		 7
#define PRICMD_FINISH_SUBFN	 7

/*
 * queueing types
 */
#define QUEUE_FCFS		0x00
#define QUEUE_CUSTOM		0x01
#define QUEUE_PRIORITY		0x02
#define QUEUE_FAIR		0x04 /*Fair Queuing enabled by Config */
#define QUEUE_RED		0x08
#define QUEUE_FAIR_DEF		0x10 /*Fair Queuing enabled by default*/
#define QUEUE_ALL		(QUEUE_CUSTOM|QUEUE_PRIORITY|QUEUE_FAIR|QUEUE_RED|QUEUE_FAIR_DEF)
#endif
