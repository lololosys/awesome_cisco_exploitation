/* $Id: exec_show_ip_policy.h,v 3.1.64.1 1996/08/12 16:04:10 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_show_ip_policy.h,v $
 *------------------------------------------------------------------
 * exec_show_ip_policy.h -- Show IP policy configuration
 *
 * Sep 1995 Tony Li
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ip_policy.h,v $
 * Revision 3.1.64.1  1996/08/12  16:04:10  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1995/11/09  12:07:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/19  08:05:57  tli
 * CSCdi41042:  IP policy: local policy fails, debugging obtuse
 * Local policy packets weren't being checksummed correctly.
 * Give debugging its own command.
 *
 * Revision 2.1  1995/09/26  07:53:18  tli
 * Placeholder for Yet Another Exec Command
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip policy
 *
 */
EOLNS	(show_ip_policy_eol, show_ip_policy_command);
KEYWORD (show_ip_policy, show_ip_policy_eol, no_alt,
	 "policy", "Policy routing", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_policy
