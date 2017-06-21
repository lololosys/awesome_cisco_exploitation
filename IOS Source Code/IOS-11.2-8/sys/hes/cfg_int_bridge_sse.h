/* $Id: cfg_int_bridge_sse.h,v 3.1 1996/02/01 04:57:54 hampton Exp $
 * $Source: /release/111/cvs/Xsys/hes/cfg_int_bridge_sse.h,v $
 *------------------------------------------------------------------
 * SSE Bridging commands
 *
 * December 1995, Heidi Ou
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_bridge_sse.h,v $
 * Revision 3.1  1996/02/01  04:57:54  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.1  1995/12/27  18:56:39  hou
 * CSCdi45767:  Attempting to configure bridge-gr X sse causes
 *              %SYS-3-REGNONE:Registry 8 service 12 doesn't exist
 *              error message
 * - bgroup_sse parser node is removed from bgroup tree and dynamically
 *   added to the 7000 parser chain.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/**********************************************************************
 * bridge-group <group> sse
 * no bridge-group <group> sse
 */
EOLS	(bgroup_sse_eol, bridgegroup_sse_command, BGROUP_SSE);
KEYWORD	(bgroup_sse, bgroup_sse_eol, ALTERNATE,
	 "sse", "Enable SSE forwarding", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	bgroup_sse
