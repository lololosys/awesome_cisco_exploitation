/* $Id: cfg_routemap_match_policy.h,v 3.3 1995/11/28 19:50:34 widmer Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/cfg_routemap_match_policy.h,v $
 *------------------------------------------------------------------
 * cfg_routemap_match_policy.h -- Route map match clauses for policy
 * routing.
 *
 * Mar 1995 Tony Li
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_routemap_match_policy.h,v $
 * Revision 3.3  1995/11/28  19:50:34  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  17:31:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/21  18:16:00  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/15  15:18:44  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  21:05:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * match length <min> <max>
 *
 * OBJ(int, 1) = <min>
 * OBJ(int, 2) = <max>
 */

LINK_EXIT(routemap_match_policy_exit, no_alt);

/*
 * match length <min> <max>
 */

FUNC	(routemap_match_len_func,
	 routemap_match_policy_exit,
	 route_map_match_length_command);
NUMBER	(routemap_match_len_max, routemap_match_len_func, no_alt,
	 OBJ(int, 2), 0, MAXINT, "Maximum packet length");
NUMBER	(routemap_match_len_min, routemap_match_len_max, no_alt,
	 OBJ(int, 1), 0, MAXINT, "Minimum packet length");
KEYWORD	(routemap_match_length, routemap_match_len_min, ALTERNATE,
	 "length", "Packet length", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	routemap_match_length
