/* $Id: exec_show_clns_sse.h,v 3.3 1996/02/01 23:33:16 widmer Exp $
 * $Source: /release/111/cvs/Xsys/hes/exec_show_clns_sse.h,v $
 *------------------------------------------------------------------
 * exec_show_clns_sse.h
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_clns_sse.h,v $
 * Revision 3.3  1996/02/01  23:33:16  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:15:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:22:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show clns sse address <address>
 * show clns sse memory
 * show clns sse rewrite
 * show clns sse shadow memory [<address>]
 * 
 * OBJ(int,1) = SSE_CMD_INTERNALS, SSE_CMD_MEMORY, SSE_CMD_REWRITE
 * OBJ(int,2) = <address>, -1 if no address
 * OBJ(paddr,3) = CLNS address
 *
 */

EOLS	(show_clns_sse_eol, show_sse, SSE_PROTOCOL_CLNS);

KEYWORD_ID(show_clns_sse_rewrite, show_clns_sse_eol, no_alt,
	   OBJ(int,1), SSE_CMD_REWRITE,
	   "rewrite", "SSE rewrite table", PRIV_USER);

KEYWORD_ID(show_clns_sse_memory, show_clns_sse_eol, show_clns_sse_rewrite,
	   OBJ(int,1), SSE_CMD_INTERNALS,
	   "memory", "SSE internal memory structures", PRIV_USER); 

NSAPADDR  (show_clns_sse_address_network, show_clns_sse_eol, no_alt,
	   OBJ(paddr,3), "Destination");
KEYWORD_ID(show_clns_sse_address, show_clns_sse_address_network,
	   show_clns_sse_memory,
	   OBJ(int,1), SSE_CMD_ADDRESS,
	   "address", "SSE data for address", PRIV_USER);

KEYWORD (show_clns_sse, show_clns_sse_address, ALTERNATE,
	 "sse", "SSE information", PRIV_USER|PRIV_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_clns_sse
