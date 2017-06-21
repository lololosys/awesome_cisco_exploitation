/* $Id: ipaccess.h,v 3.2.58.6 1996/08/16 08:02:01 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaccess.h,v $
 *------------------------------------------------------------------
 * ipaccess.h -- IP Access List Definitions
 *
 * 7-May-1987, Kirk Lougheed
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipaccess.h,v $
 * Revision 3.2.58.6  1996/08/16  08:02:01  dlobete
 * CSCdi59971:  DACL needs modularity work
 * Branch: California_branch
 * For the sake of future maintenance of IP access-lists, share as much
 * source code as possible between VIP and RSP.
 *
 * Revision 3.2.58.5  1996/07/30  23:42:54  ahh
 * CSCdi64604:  NAT: need support for multi-homing
 * Allow the use of route maps for choosing a global address pool.
 * Branch: California_branch
 *
 * Revision 3.2.58.4  1996/07/20  01:13:11  bew
 * CSCdi63411:  ACL encrypted/decrypted flags are not useful
 * Branch: California_branch
 * Remove encrypted/decrypted flags introduced early in 11.2
 * from ACL code.
 *
 * Revision 3.2.58.3  1996/06/18  20:43:38  sbelair
 * CSCdi42319:  Lock&Key idle timer does not work correctly
 * Branch: California_branch
 *
 * Add a timestamp to the slowtype structure for use with dynamic access
 * lists. Record timestamp whenever there is a hit on the access list and
 * base the idle time expiration off  of  this time of last reference.
 *
 * Revision 3.2.58.2  1996/05/04  01:02:42  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.2.58.1  1996/04/19  15:41:16  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.5  1996/04/17  23:01:31  carrel
 * Branch: IosSec_branch
 * Final commit from IP code review comments
 *
 * Revision 2.1.6.4  1996/03/30  05:18:56  carrel
 * cvs fixes
 *
 * Revision 2.1.6.3  1996/02/13  19:17:50  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.1.6.2  1995/11/17  17:00:22  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/09/26  04:12:51  carrel
 * Branch: IosSec_branch
 * Initial commit of encryption ACL code.
 *
 * Revision 3.2  1995/11/17  09:33:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:04:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:57:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IPACCESS_H__
#define __IPACCESS_H__

/*
 * Definitions for per-user ACLs.
 */
#define IP_ACL_USER_RESET 0


/*
 * Prototypes definitions for
 *    ipaccess1.c
 *    ipaccess2.c
 */

/*
 * ipaccess1.c
 */
extern boolean check_access(tt_soc *, ipaddrtype, ulong, ulong, boolean);
extern boolean nfast_check(acl_headertype *, ipaddrtype);
extern boolean fast_check(int, ipaddrtype);
extern boolean network_check(acl_headertype *, ipaddrtype, ipaddrtype);
extern boolean route_check(acl_headertype *, ipaddrtype, ipaddrtype);
extern void ip1access_nvgen(parseinfo *, acl_headertype *);
extern void ip1access_parselist(parseinfo *, acl_headertype *);
extern void ip1access_deletelist(acl_headertype *);
extern void ip1access_command(parseinfo *csb);
extern void show_ip1access(acl_headertype *);
extern void ipaccess_init(void);
extern void clear_ip_access_stats(acl_headertype *);

/*
 * ipaccess2.c
 */

extern accessptr *ip_getacl(int);
extern boolean ip_old_accesscheck(paktype *, int);
extern boolean ip_accesscheck_wrapper(paktype *, acl_headertype *,
				      hwaddrtype *src_snpa, boolean);
extern boolean ip_accesscheck(paktype *, acl_headertype *);
extern boolean ip_accesscheck_snpa(paktype *, acl_headertype *, boolean);
extern boolean slow_check(acl_headertype *, boolean, idbtype *, uint,
			  ipaddrtype, ipaddrtype, ulong, ulong,
			  boolean, uchar); 
extern void ip2access_deletelist(acl_headertype *);
extern void ip2access_parselist(parseinfo *, acl_headertype *);
extern boolean ip_acc_ck_count_violations(paktype *, acl_headertype *);
extern void show_ip2access(acl_headertype *);
extern boolean ip_access_list_configured(int);
extern void accesslog_cache(acl_headertype *, boolean, idbtype *,
			    hwaddrtype *, ipaddrtype, ipaddrtype,
			    uchar, ushort, ushort);
extern void accesslog_cache_flush(acl_headertype *);
extern void accesslog_cache_print(void);
extern void clear_ip2_access_stats(acl_headertype *);
extern void ip2access_nvgen(parseinfo *, acl_headertype *);
extern boolean ipaccess_emptylist(acl_headertype *);
extern boolean ipaccess_numeric_acl(acl_headertype *);
extern boolean ipaccess_willpassfrags(acl_headertype *);
extern void ip2access_command(parseinfo *csb);
#endif __IPACCESS_H__
