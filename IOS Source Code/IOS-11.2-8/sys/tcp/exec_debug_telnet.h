/* $Id: exec_debug_telnet.h,v 3.1.2.1 1996/04/15 21:23:18 bcole Exp $
 * $Source: /release/112/cvs/Xsys/tcp/exec_debug_telnet.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ T E L N E T . H
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_telnet.h,v $
 * Revision 3.1.2.1  1996/04/15  21:23:18  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  01:40:27  bcole
 * Placeholders for 11.2 features.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug telnet
 *
 */
EOLS	(debug_telnet_eol, debug_command, DEBUG_TELNET);
KEYWORD_DEBUG(debug_telnet, debug_telnet_eol, ALTERNATE,
	      OBJ(pdb,1), telnet_debug_arr,
	      "telnet", "Incoming telnet connections", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_telnet
