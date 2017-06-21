/* $Id: exec_test_ip.h,v 3.2 1995/11/17 09:32:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/exec_test_ip.h,v $
 *------------------------------------------------------------------
 * IP test commands
 *
 * February 1994, Bob Albrightson
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_test_ip.h,v $
 * Revision 3.2  1995/11/17  09:32:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:46  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:27:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(test_ip_extend_here, no_alt);

/******************************************************************
 * test ip
 */
KEYWORD_MM (priv_test_ip, test_ip_extend_here, ALTERNATE,
	    "ip", "IP test commands", PRIV_OPR | PRIV_INTERNAL, 2);

#undef  ALTERNATE
#define ALTERNATE priv_test_ip

