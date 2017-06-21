/* $Id: cgmp.h,v 3.1.4.2 1996/04/20 01:50:56 dino Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/cgmp.h,v $
 *------------------------------------------------------------------
 * cgmp.h - Cisco Group Management Protocol (CGMP)
 *
 * January, 1996
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cgmp.h,v $
 * Revision 3.1.4.2  1996/04/20  01:50:56  dino
 * CSCdi54104:  CGMP must support source-only systems in switched Cat5000
 *              network
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/03  23:19:00  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CGMP_H__
#define __CGMP_H__

/*
 * Constants definitions.
 */
#define CGMP_VERSION1   1
#define CGMP_JOIN_TYPE  0
#define CGMP_LEAVE_TYPE 1
#define CGMP_BATCH_TIME (IGMP_MAX_QUERY_RESPONSE_INTERVAL + ONESEC)

/*
 * CGMP Header definitions.
 */
typedef struct cgmptype_ {
    uchar      cgmp_version: 4;
    uchar      cgmp_type: 4;
    uchar      cgmp_reserved[2];
    uchar      cgmp_count;
    ieee_addrs cgmp_mac_addrs[0];
} cgmptype;

#define CGMP_ENTRY_SIZE sizeof(ieee_addrs)

/*
 * Debug stuff.
 */
#define CGMP_BUGINF if (cgmp_debug) buginf

/*
 * External function definitions.
 */
extern void cgmp_commands(parseinfo *);
extern void cgmp_send_self(uchar, idbtype *, ipaddrtype);
extern void cgmp_send_join(idbtype *, paktype *, igmptype *, ipaddrtype, 
			   ipaddrtype);
extern void cgmp_send_proxy(paktype *, igmptype *, ipaddrtype, idbtype *);
extern void cgmp_send_leave(ipaddrtype, idbtype *, paktype *);
extern void cgmp_send_cleanup(idbtype *);
extern boolean cgmp_sources_exist (ipaddrtype, idbtype *);
extern void cgmp_process_igmp_report(idbtype *, igmpidbtype *, paktype *,
				     igmptype *, ipaddrtype, ipaddrtype);
extern void cgmp_send_batch_join(idbtype *, igmpidbtype *);
extern void cgmp_send_join_packet(paktype *, idbtype *, uchar);
extern boolean cgmp_find_entry(igmpidbtype *, uchar *, uchar *);
extern void cgmp_save_info(idbtype *, igmpidbtype *, paktype *, ipaddrtype);
extern uchar *cgmp_getbuffer(idbtype *, igmpidbtype *);

#endif __CGMP_H__
