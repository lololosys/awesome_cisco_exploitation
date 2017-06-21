/* $Id: ipaccount.h,v 3.2 1995/11/17 09:34:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/ipaccount.h,v $
 *------------------------------------------------------------------
 * IP accounting public definitions
 *
 * August 1994, Paul Traina
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ipaccount.h,v $
 * Revision 3.2  1995/11/17  09:34:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/11  23:49:18  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.1  1995/06/07 20:58:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IPACCOUNT_H__
#define __IPACCOUNT_H__

#define IPACCTGENTRIESPERBLOCK 32
#define IPACCTGDEFAULTLIMIT    IPACCTGENTRIESPERBLOCK * 16
#define IPACCTGARRAYSIZE 256	/* hash array size */
#define IPACCTGDATAARRAYSIZE 2
#define	NO_VIOLATION 0

/*
 * "Accounting" record used to maintain information on all ip
 * packets. One for each source - destination ip combination.
 */
typedef struct ip_acctg_entry_type_ {
    struct ip_acctg_entry_type_ *next;
    ipaddrtype srcadr;
    ipaddrtype dstadr;
    ulong bytes;
    ulong packets;
    ulong *memory;
    ulong access_violation;
} ip_acctg_entry_type;

typedef struct ip_acctg_data_type_ {
    ulong packets_tossed;
    ulong bytes_tossed;
    ulong violations_tossed;
    sys_timestamp when;
    ip_acctg_entry_type *store[IPACCTGARRAYSIZE];
} ip_acctg_data_type;

/*
 * IP accounting control record -- controls whether we accumulate
 * accounting information -- and the limits that we will go to i.e.
 * number of entries that will be saved. We also will record the
 * number of packets tossed when this threshold has been exceeded.
 */
typedef struct ip_acctg_info_type_ {
    ulong count;	
    ulong threshold;	
    ulong generation;
    ip_acctg_data_type *current;
    ip_acctg_data_type *backup;
    ip_acctg_data_type data[IPACCTGDATAARRAYSIZE]; /* just two (2) */
} ip_acctg_info_type;

/*
 * The following structure type is used when we request memory.
 */
typedef struct ip_acctg_mem_type_ {
    struct ip_acctg_mem_type_ *next;
    ulong fragments;
    ip_acctg_entry_type entry[IPACCTGENTRIESPERBLOCK];
} ip_acctg_mem_type;

/*
 * This structure is used to hold what networks we consider source/sink
 * for IP accounting data.
 */

typedef struct ip_acctg_acc_entry_type_ {
    ipaddrtype addr;
    ipaddrtype mask;
} ip_acctg_acc_entry_type;

typedef struct ip_acctg_acc_type_ {
    int count;
    int transits_def;
    int transits;
    ip_acctg_acc_entry_type list[0];
} ip_acctg_acc_type;

/*
 * External structure access
 */
extern queuetype ipacctQ;
extern ip_acctg_info_type ip_acctg_info;
extern ip_acctg_acc_type *ip_acctg_acc;
extern boolean ip_acctg_on;

#endif /* __IPACCOUNT_H__ */
