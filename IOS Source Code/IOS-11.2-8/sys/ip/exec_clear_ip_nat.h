/* $Id: exec_clear_ip_nat.h,v 3.1.6.3 1996/08/20 18:55:30 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_clear_ip_nat.h,v $
 *------------------------------------------------------------------
 * NAT clear commands
 *
 * October 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_ip_nat.h,v $
 * Revision 3.1.6.3  1996/08/20  18:55:30  ahh
 * CSCdi66560:  NAT: cannot clear statistics
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/04/24  00:05:52  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/04/16  18:53:01  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/07  05:58:56  ahh
 * Placeholder...
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ip nat statistics
 */

EOLS	(clear_ip_nat_stat_eol, ipnat_clear_command, IPNAT_STATS);
KEYWORD (clear_ip_nat_statistics, clear_ip_nat_stat_eol, no_alt,
	 "statistics", "Clear translation statistics", PRIV_OPR);

/******************************************************************
 * clear ip nat translation *
 * clear ip nat translation inside <ipaddr> <ipaddr>
 *	{ outside <ipaddr> <ipaddr> }
 * clear ip nat translation outside <ipaddr> <ipaddr>
 * clear ip nat translation <proto> inside <ipaddr> <port> <ipaddr> <port>
 *	{ outside <ipaddr> <port> <ipaddr> <port> }
 *
 * OBJ(int,1) = TRUE if '*' entered
 * OBJ(int,2) = TRUE if extended
 * OBJ(int,3) = if inside addresses specified
 * OBJ(int,4) = if outside addresses specified
 * OBJ(int,5) = protocol
 * OBJ(int,6) = inside global port
 * OBJ(int,7) = inside local port
 * OBJ(int,8) = outside local port
 * OBJ(int,9) = outside global port
 * OBJ(paddr,1) = inside global address
 * OBJ(paddr,2) = inside local address
 * OBJ(paddr,3) = outside local address
 * OBJ(paddr,4) = outside global address
 */

pdecl(clear_ip_nat_trans_inside);
pdecl(clear_ip_nat_trans_outside);

EOLS	(clear_ip_nat_trans_eol, ipnat_clear_command, IPNAT_DYNAMIC);

NUMBER	(clear_ip_nat_trans_ogport, clear_ip_nat_trans_eol, no_alt,
	 OBJ(int,9), 1, 65535,
	 "Global UDP/TCP port");
IFELSE	(clear_ip_nat_trans_ogtest, clear_ip_nat_trans_ogport,
	 clear_ip_nat_trans_eol, GETOBJ(int,2) == TRUE);
IPADDR	(clear_ip_nat_trans_ogaddr, clear_ip_nat_trans_ogtest, no_alt,
	 OBJ(paddr,4), "Global IP address");
NUMBER	(clear_ip_nat_trans_olport, clear_ip_nat_trans_ogaddr, no_alt,
	 OBJ(int,8), 1, 65535,
	 "Local UDP/TCP port");
IFELSE	(clear_ip_nat_trans_oltest, clear_ip_nat_trans_olport,
	 clear_ip_nat_trans_ogaddr, GETOBJ(int,2) == TRUE);
IPADDR	(clear_ip_nat_trans_oladdr, clear_ip_nat_trans_oltest, no_alt,
	 OBJ(paddr,3), "Local IP address");

NUMBER	(clear_ip_nat_trans_ilport, clear_ip_nat_trans_outside, no_alt,
	 OBJ(int,7), 1, 65535,
	 "Local UDP/TCP port");
IFELSE	(clear_ip_nat_trans_iltest, clear_ip_nat_trans_ilport,
	 clear_ip_nat_trans_outside, GETOBJ(int,2) == TRUE);
IPADDR	(clear_ip_nat_trans_iladdr, clear_ip_nat_trans_iltest, no_alt,
	 OBJ(paddr,2), "Local IP address");
NUMBER	(clear_ip_nat_trans_igport, clear_ip_nat_trans_iladdr, no_alt,
	 OBJ(int,6), 1, 65535,
	 "Global UDP/TCP port");
IFELSE	(clear_ip_nat_trans_igtest, clear_ip_nat_trans_igport,
	 clear_ip_nat_trans_iladdr, GETOBJ(int,2) == TRUE);
IPADDR	(clear_ip_nat_trans_igaddr, clear_ip_nat_trans_igtest, no_alt,
	 OBJ(paddr,1), "Global IP address");

/* { * | inside | outside | tcp | udp } */

SET     (clear_ip_nat_trans_extended, clear_ip_nat_trans_inside,
	 OBJ(int,2), TRUE);
KEYWORD_ID(clear_ip_nat_trans_udp, clear_ip_nat_trans_extended, no_alt,
	 OBJ(int,5), UDP_PROT,
	 UDP_PROT_NAME, "User Datagram Protocol", PRIV_USER);
KEYWORD_ID(clear_ip_nat_trans_tcp, clear_ip_nat_trans_extended,
	 clear_ip_nat_trans_udp,
	 OBJ(int,5), TCP_PROT,
	 TCP_PROT_NAME, "Transmission Control Protocol", PRIV_USER);
IFELSE	(clear_ip_nat_trans_test0, clear_ip_nat_trans_eol, no_alt,
	 GETOBJ(int,3) == TRUE);
IFELSE	(clear_ip_nat_trans_test1, clear_ip_nat_trans_test0,
	 clear_ip_nat_trans_tcp,
	 GETOBJ(int,2) == TRUE || GETOBJ(int,3) == TRUE ||
	 GETOBJ(int,4) == TRUE);

KEYWORD_ID(clear_ip_nat_trans_outside, clear_ip_nat_trans_oladdr,
	 clear_ip_nat_trans_test1,
	 OBJ(int,4), TRUE,
	 "outside", "Outside addresses (and ports)", PRIV_USER);

IFELSE	(clear_ip_nat_trans_test2, no_alt /* ??? */, clear_ip_nat_trans_outside,
	 GETOBJ(int,2) == TRUE);

KEYWORD_ID(clear_ip_nat_trans_inside, clear_ip_nat_trans_igaddr,
	 clear_ip_nat_trans_test2,
	 OBJ(int,3), TRUE,
	 "inside", "Inside addresses (and ports)", PRIV_USER);

KEYWORD_ID(clear_ip_nat_trans_all, clear_ip_nat_trans_eol,
	 clear_ip_nat_trans_inside,
	 OBJ(int,1), TRUE,
	 "*", "Delete all dynamic translations", PRIV_USER);

KEYWORD (clear_ip_nat_dynamic, clear_ip_nat_trans_all, clear_ip_nat_statistics,
	 "translation", "Clear dynamic translation", PRIV_OPR);

KEYWORD (clear_ip_nat, clear_ip_nat_dynamic, ALTERNATE,
	 "nat", "Clear NAT", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_ip_nat
