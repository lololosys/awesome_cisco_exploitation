/* $Id: exec_show_ip_sse.h,v 3.3.20.1 1996/08/12 16:01:35 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/exec_show_ip_sse.h,v $
 *------------------------------------------------------------------
 * exec_show_ip_sse.h
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_ip_sse.h,v $
 * Revision 3.3.20.1  1996/08/12  16:01:35  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/01  23:33:17  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:15:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  08:33:25  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.1  1995/07/02  10:51:56  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:24:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip sse access-list [<number> | input]
 * show ip sse address <address>
 * show ip sse consistency
 * show ip sse memory
 * show ip sse rewrite
 * 
 * OBJ(int,1) = SSE_CMD_INTERNALS, SSE_CMD_MEMORY, SSE_CMD_REWRITE,
 *              SSE_CMD_ACCESS_INPUT, SSE_CMD_ACCESS_LIST
 * OBJ(int,2) = <address>, -1 if no address, <list number>
 * OBJ(paddr,3) = IP address
 *
 */

EOLNS	(show_ip_sse_access_eol, ipaccess_sse_show_command);
KEYWORD_ID(show_ip_sse_access_input, show_ip_sse_access_eol, no_alt,
	   OBJ(int,1), SSE_CMD_ACCESS_INPUT,
	   "input", "SSE internal memory structures", PRIV_CONF); 
NUMBER  (show_ip_sse_access_list, show_ip_sse_access_eol,
	 show_ip_sse_access_input, OBJ(int,2), 1, 199, "List number");
KEYWORD_ID(show_ip_sse_access, show_ip_sse_access_list, no_alt,
	   OBJ(int,1), SSE_CMD_ACCESS_LIST,
	   "access-list", "SSE access list information", PRIV_CONF);

EOLS	(show_ip_sse_eol, show_sse, SSE_PROTOCOL_IP);

KEYWORD_ID(show_ip_sse_rewrite, show_ip_sse_eol, show_ip_sse_access,
	   OBJ(int,1), SSE_CMD_REWRITE,
	   "rewrite", "SSE rewrite table", PRIV_USER);

KEYWORD_ID(show_ip_sse_memory, show_ip_sse_eol, show_ip_sse_rewrite,
	   OBJ(int,1), SSE_CMD_INTERNALS,
	   "memory", "SSE internal memory structures", PRIV_CONF); 

KEYWORD_ID (show_ip_sse_consistency, show_ip_sse_eol, show_ip_sse_memory,
	    OBJ(int,1), SSE_CMD_CONSISTENCY, "consistency",
	    "SSE internal program consistency", PRIV_CONF);

IPADDR  (show_ip_sse_address_network, show_ip_sse_eol, no_alt,
	 OBJ(paddr,3), "Destination");
KEYWORD_ID(show_ip_sse_address, show_ip_sse_address_network,
	   show_ip_sse_consistency,
	   OBJ(int,1), SSE_CMD_ADDRESS,
	   "address", "SSE data for address", PRIV_CONF);

KEYWORD	(show_ip_sse, show_ip_sse_address, ALTERNATE,
	 "sse", "SSE information", PRIV_USER|PRIV_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_ip_sse
