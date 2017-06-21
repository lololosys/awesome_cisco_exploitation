/* $Id: parser_defs_igrp2.h,v 3.1.40.2 1996/07/24 13:08:06 boyle Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/parser_defs_igrp2.h,v $
 *------------------------------------------------------------------
 * parser_defs_igrp2.c
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_igrp2.h,v $
 * Revision 3.1.40.2  1996/07/24  13:08:06  boyle
 * CSCdi19400:  show ipx eigrp topology should give an option for AS number
 * Branch: California_branch
 *
 * Revision 3.1.40.1  1996/04/02  05:37:18  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.1  1996/02/01  04:58:42  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:51:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _PARSER_DEFS_IGRP2_H_
#define _PARSER_DEFS_IGRP2_H_

/*
 * Used in show topology commands
 */
enum {IGRP2_TOP_ZERO, IGRP2_TOP_ALL, IGRP2_TOP_PASSIVE, IGRP2_TOP_ACTIVE,
	  IGRP2_TOP_FS, IGRP2_TOP_PENDING, IGRP2_TOP_SUMMARY, IGRP2_TOP_AS};

/*
 * Test types
 */
#define IGRP2_TEST_LOCAL1 0
#define IGRP2_TEST_LOCAL2 1
#define IGRP2_TEST_LOCAL3 2
#define IGRP2_TEST_LOCAL4 3
#define IGRP2_TEST_LOCAL5 4
#define IGRP2_TEST_LOCAL6 5
#define IGRP2_TEST_SUCC1  6
#define IGRP2_TEST_SUCC2  7
#define IGRP2_TEST_SUCC3  8
#define IGRP2_TEST_DELETE 9
#define IGRP2_TEST_ACK	  10

/*
 * For event log show commands
 */
#define IGRP2_EVENT_CUR 0
#define IGRP2_EVENT_SIA 1

/*
 * For debug packet filtering
 *
 * Note--these must be equivalent to a bit shift of the opcode types defined
 * in iprouting/dual.h.
 */
#define IGRP2_DEBUG_UPDATE  0x2
#define IGRP2_DEBUG_REQUEST 0x4
#define IGRP2_DEBUG_QUERY   0x8
#define IGRP2_DEBUG_REPLY   0x10
#define IGRP2_DEBUG_HELLO   0x20
#define IGRP2_DEBUG_IPXSAP  0x40
#define IGRP2_DEBUG_PROBE   0x80
#define IGRP2_DEBUG_ACK     0x100
#define IGRP2_DEBUG_VERBOSE 0x1fe	/* All of the above */
#define IGRP2_DEBUG_TERSE   (IGRP2_DEBUG_VERBOSE & (~(IGRP2_DEBUG_HELLO)))
#define IGRP2_DEBUG_RETRY   0x8000	/* Retransmissions only */

/*
 * Transmit debug flag definitions.  Used to control the detail of
 * debugging in the transmit state machines.
 */
#define XDBACK		0x1		/* Acknowledgements */
#define XDBPACKETIZE	0x2		/* Packetization */
#define XDBSTARTUP	0x4		/* Peer birth */
#define XDBPEERDOWN	0x8		/* Peer death */
#define XDBLINK		0x10		/* Thread linkage */
#define XDBBUILD	0x20		/* Packet building */
#define XDBSTRANGE	0x40		/* Unusual occurrences */
#define XDBDETAIL	0x80		/* Detailed versions of the above */
#define XDB_NUMFLAGS	8		/* Number of flags */

/* Symbols for EIGRP "show" commands */

enum {EIGRP_SHOW_NEIGHBORS, EIGRP_SHOW_INTERFACE};

#endif					/* _PARSER_DEFS_IGRP2_H_ */
