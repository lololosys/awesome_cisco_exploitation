/* $Id: exec_tarp_originate.h,v 3.3 1996/01/30 20:34:45 asastry Exp $
 * $Source: /release/111/cvs/Xsys/tarp/exec_tarp_originate.h,v $
 *------------------------------------------------------------------
 * TARP (Target ID address Resolution Protocol)
 *
 * October 1995, Arun Sastry
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_tarp_originate.h,v $
 * Revision 3.3  1996/01/30  20:34:45  asastry
 * CSCdi47719:  Add help entry for TARP exec command. Also make other
 * trivial UI fixes.
 *
 * Revision 3.2  1995/11/17  18:44:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * tarp query <NSAP>
 *
 */

EOLS    (exec_tarp_query_eol, tarp_originate_command, CMD_TARP_QUERY);

NSAPADDR(exec_tarp_query_cmd, exec_tarp_query_eol, NONE, OBJ(paddr,1),
	 "Target host's NSAP");
KEYWORD	(exec_tarp_query_nsap, exec_tarp_query_cmd, no_alt,
	 "query", "Obtain host's Target ID (TID) using its NSAP", PRIV_CONF);


/***************************************************************
 * tarp resolve <TID string> [1 | 2]
 *
 */

EOLS	(exec_tarp_resolve_eol, tarp_originate_command, CMD_TARP_RESOLVE);

NUMBER  (exec_tarp_pdu_type, exec_tarp_resolve_eol, exec_tarp_resolve_eol,
	 OBJ(int,1), 1, 2, "TARP Request PDU type");

STRING  (exec_tarp_resolve_tid, exec_tarp_pdu_type, no_alt,
	 OBJ(string,1), "Target ID to resolve");

KEYWORD	(exec_tarp_resolve_cmd, exec_tarp_resolve_tid,
	 exec_tarp_query_nsap, "resolve", "Resolve a TARP Target ID (TID)",
	 PRIV_CONF);
KEYWORD	(exec_tarp_resolve, exec_tarp_resolve_cmd, ALTERNATE,
	 "tarp", "TARP (Target ID Resolution Protocol) commands", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	exec_tarp_resolve

