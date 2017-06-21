/* $Id: exec_debug_ip_policy.h,v 3.1 1995/11/09 12:07:06 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/exec_debug_ip_policy.h,v $
 *------------------------------------------------------------------
 * exec_debug_ip_policy.h -- Debugging for IP policy routing
 *
 * Sep 1995 Tony Li
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_policy.h,v $
 * Revision 3.1  1995/11/09  12:07:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/19  08:05:55  tli
 * CSCdi41042:  IP policy: local policy fails, debugging obtuse
 * Local policy packets weren't being checksummed correctly.
 * Give debugging its own command.
 *
 * Revision 2.1  1995/09/26  06:50:58  tli
 * Placeholder for Yet Another Parser File
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ip policy
 *
 */
EOLS	(debug_ip_policy_eol, iprouting_debug_command, DEBUG_POLICY);
KEYWORD (debug_ip_policy, debug_ip_policy_eol, no_alt,
	 "policy", "Policy routing", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip_policy
