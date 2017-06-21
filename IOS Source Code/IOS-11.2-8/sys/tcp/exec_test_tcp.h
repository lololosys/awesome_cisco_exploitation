/* $Id: exec_test_tcp.h,v 3.2 1995/11/17 18:47:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tcp/exec_test_tcp.h,v $
 *------------------------------------------------------------------
 * TCP test commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_test_tcp.h,v $
 * Revision 3.2  1995/11/17  18:47:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:33  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:27:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * test tcp line|drop|delay|random <number>
 */
PARAMS	(priv_test_tcp_random, no_alt, "random",
	 OBJ(int, 1), 0, MAXINT,
	 test_tcp, TEST_TCP_RANDOM,
	 "Drop every Nth packet", "number", PRIV_OPR);
PARAMS	(priv_test_tcp_line, priv_test_tcp_random, "line",
	 OBJ(int, 1), 0, MAXINT,
	 test_tcp, TEST_TCP_LINE,
	 "Which connections", "number", PRIV_OPR);
PARAMS	(priv_test_tcp_delay, priv_test_tcp_line, "delay",
	 OBJ(int, 1), 0, MAXINT,
	 test_tcp, TEST_TCP_DELAY,
	 "Delay packets", "number", PRIV_OPR);
PARAMS	(priv_test_tcpdrop, priv_test_tcp_delay, "drop",
	 OBJ(int, 1), 0, MAXINT,
	 test_tcp, TEST_TCP_DROP,
	 "Drop packets", "number", PRIV_OPR);
KEYWORD	(priv_test_tcp, priv_test_tcpdrop, ALTERNATE,
	 "tcp", "TCP test commands", PRIV_OPR | PRIV_INTERNAL);

#undef	ALTERNATE
#define	ALTERNATE	priv_test_tcp
