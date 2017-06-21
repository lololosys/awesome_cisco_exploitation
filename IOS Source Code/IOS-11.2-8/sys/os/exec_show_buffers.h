/* $Id: exec_show_buffers.h,v 3.3.46.2 1996/08/12 16:05:08 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_buffers.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ B U F F E R S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_buffers.h,v $
 * Revision 3.3.46.2  1996/08/12  16:05:08  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.46.1  1996/07/23  13:27:03  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3  1995/12/14  13:07:27  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.2  1995/11/17  18:46:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/12  07:09:33  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.1  1995/07/02  10:51:09  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:22:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef enum show_buffers_search_ {
    SHOW_BUFFERS_SEARCH_NONE,
    SHOW_BUFFERS_SEARCH_ASSIGNED,
    SHOW_BUFFERS_SEARCH_FREE,
    SHOW_BUFFERS_SEARCH_IF_INPUT,
    SHOW_BUFFERS_SEARCH_OLD,
    SHOW_BUFFERS_SEARCH_ADDRESS,
    SHOW_BUFFERS_SEARCH_POOL,
    SHOW_BUFFERS_SEARCH_ALL,
} show_buffers_search;

typedef enum show_buffers_dump_ {
    SHOW_BUFFERS_DUMP_NONE,
    SHOW_BUFFERS_DUMP_PACKET,
    SHOW_BUFFERS_DUMP_HEADER,
    SHOW_BUFFERS_DUMP_ALL,
} show_buffers_dump;

/******************************************************************
 * show buffers [all | free | alloc | old | failures
 *               input-interface <interface> | pool <pool name> |
 *               address <address> [dump | packet | header]]
 *
 * OBJ(idb,1) = <interface>
 * OBJ(int,1) = search value 
 * OBJ(int,2) = dump value
 * OBJ(int,3) = buffer address
 * OBJ(string,1) = pool name
 */

EOLNS	(show_buffers_eol, show_buffers);

KEYWORD_ID (show_buffers_header, show_buffers_eol, NONE,
	    OBJ(int,2), SHOW_BUFFERS_DUMP_HEADER,
	    "header", "Show buffer header only", PRIV_ROOT);

KEYWORD_ID (show_buffers_packet, show_buffers_eol, show_buffers_header,
	    OBJ(int,2), SHOW_BUFFERS_DUMP_PACKET,
	    "packet", "Show buffer header and packet data", PRIV_ROOT);

KEYWORD_ID (show_buffers_dump, show_buffers_eol, show_buffers_packet,
	    OBJ(int,2), SHOW_BUFFERS_DUMP_ALL,
	    "dump", "Show buffer header and all data", PRIV_ROOT);

NOP (show_buffers_dump_nop, show_buffers_dump, show_buffers_eol);

KEYWORD_ID (show_buffers_all, show_buffers_dump_nop, NONE,
	    OBJ(int,1), SHOW_BUFFERS_SEARCH_ALL,
	    "all", "All buffers", PRIV_ROOT);

STRING  (show_buffers_pool_name, show_buffers_dump_nop, no_alt,
	 OBJ(string,1), "Name of pool to display buffers from");

KEYWORD_ID (show_buffers_pool, show_buffers_pool_name,
	    show_buffers_all,
	    OBJ(int,1), SHOW_BUFFERS_SEARCH_POOL,
	    "pool", "Buffers in a specified pool", PRIV_ROOT);

HEXNUM  (show_buffers_address_number, show_buffers_dump_nop, no_alt,
	 OBJ(int,3), "Address of buffer to display");

KEYWORD_ID (show_buffers_address, show_buffers_address_number,
	    show_buffers_pool,
	    OBJ(int,1), SHOW_BUFFERS_SEARCH_ADDRESS,
	    "address", "Buffer at a given address", PRIV_ROOT);

INTERFACE (show_buffers_if_input_interface, show_buffers_dump_nop, no_alt,
	   OBJ(idb,1), IFTYPE_ANYSUB);

KEYWORD_ID (show_buffers_if_input, show_buffers_if_input_interface,
	    show_buffers_address,
	    OBJ(int,1), SHOW_BUFFERS_SEARCH_IF_INPUT,
	    "input-interface",
	    "Buffers assigned to an input interface", PRIV_ROOT);

KEYWORD_ID (show_buffers_old, show_buffers_dump_nop, show_buffers_if_input,
	    OBJ(int,1), SHOW_BUFFERS_SEARCH_OLD,
	    "old", "Buffers older than one minute", PRIV_ROOT);

KEYWORD_ID (show_buffers_free, show_buffers_dump_nop, show_buffers_old,
	    OBJ(int,1), SHOW_BUFFERS_SEARCH_FREE,
	    "free", "Buffers available for use", PRIV_ROOT);

KEYWORD_ID (show_buffers_assigned, show_buffers_dump_nop, show_buffers_free,
	    OBJ(int,1), SHOW_BUFFERS_SEARCH_ASSIGNED,
	    "assigned", "Buffers in use", PRIV_ROOT);

PRIV_TEST (show_buffers_test, show_buffers_assigned, NONE,
	   show_buffers_eol, PRIV_ROOT);

SET (show_buffers_set_dump, show_buffers_test,
     OBJ(int,2), SHOW_BUFFERS_DUMP_NONE);
SET (show_buffers_set_type, show_buffers_set_dump,
     OBJ(int,1), SHOW_BUFFERS_SEARCH_NONE);

/******************************************************************
 * show buffers failures
 */
EOLNS	(show_buffers_fail_eol, show_getbuffer_fail);
KEYWORD	(show_buffers_fail, show_buffers_fail_eol,
	 show_buffers_set_type,
	 "failures", "Buffer allocation failures",
	 PRIV_USER|PRIV_USER_HIDDEN);

KEYWORD (show_buffers, show_buffers_fail, ALTERNATE,
	 "buffers", "Buffer pool statistics", PRIV_USER|PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_buffers
