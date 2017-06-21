/* $Id: exec_debug_udp.h,v 3.1.2.1 1996/04/15 21:15:30 bcole Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_debug_udp.h,v $
 *------------------------------------------------------------------
 * UDP debugging commands
 *
 * January 1996, Bruce Cole
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_udp.h,v $
 * Revision 3.1.2.1  1996/04/15  21:15:30  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  01:40:20  bcole
 * Placeholders for 11.2 features.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ip udp
 *
 */
EOLS	(debug_ip_udp_eol, debug_command, DEBUG_UDP);
KEYWORD_DEBUG(debug_ip_udp, debug_ip_udp_eol, ALTERNATE,
	      OBJ(pdb,1), udp_debug_arr,
	      "udp", "UDP based transactions", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip_udp
