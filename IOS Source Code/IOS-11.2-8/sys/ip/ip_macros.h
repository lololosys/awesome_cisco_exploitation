/* $Id: ip_macros.h,v 3.2.58.1 1996/03/18 20:16:14 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/ip/ip_macros.h,v $
 *------------------------------------------------------------------
 * ip_macros.h -- Parser macros for IP
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_macros.h,v $
 * Revision 3.2.58.1  1996/03/18  20:16:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  09:45:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  14:25:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:33:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/15  15:18:36  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  20:57:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * An OSPF area ID
 */
#define AREAID(name, acc, alt, var, tmpaddr)\
  EVAL(P2 ## name, acc,\
       csb->var=(ulong)csb->tmpaddr.ip_addr;\
       memset(csb->tmpaddr, 0, sizeof(addrtype)));\
  DECIMAL(P1 ## name, acc, alt, var, 0, -1,\
	  "OSPF area ID as a decimal value");\
  IPADDR(name, P2 ## name, P1 ## name, tmpaddr,\
	 "OSPF area ID in IP address format")

/*
 * TCP and UDP port numbers
 */

#define TCP_PORT(name, acc, alt, var)\
    static portparse_struct L ## name = {CSBOFF(var)};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
				  (trans_func) tcpport_action, & L ## name}

#define UDP_PORT(name, acc, alt, var)\
    static portparse_struct L ## name = {CSBOFF(var)};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
				  (trans_func) udpport_action, & L ## name}
