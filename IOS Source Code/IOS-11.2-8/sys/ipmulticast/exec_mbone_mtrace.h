/* $Id: exec_mbone_mtrace.h,v 3.1.62.3 1996/08/12 16:03:18 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/exec_mbone_mtrace.h,v $
 *------------------------------------------------------------------
 * exec_mbone_mtrace.h - mbone trace commands mtrace, mstat, and mrinfo
 *
 * August 1995, chwhite
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_mbone_mtrace.h,v $
 * Revision 3.1.62.3  1996/08/12  16:03:18  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.62.2  1996/07/26  22:53:48  chwhite
 * CSCdi63685:  Correct mtrace error handling, TTL behavior
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/06/07  15:08:59  chwhite
 * CSCdi51969:  Jhawk wants mrinfo source-interface option
 * Branch: California_branch
 *
 * Revision 3.1  1995/11/09  12:00:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/21  18:40:22  dino
 * CSCdi40833:  mrinfo with no address doesnt respond.
 *
 * Revision 2.2  1995/08/15  21:22:10  chwhite
 * CSCdi38823:  Code review comments and minor bug fixes
 *
 * Revision 2.1  1995/08/03  01:12:56  chwhite
 * CSCdi38104:  It would be nice if the router supported mtrace/mrinfo.
 *              Added support for mbone traceroute and mrinfo
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***********************************************************
 * mtrace <source> <destination> <group> [<ttl>]
 * 
 * OBJ(paddr,1) = source to trace route from
 * OBJ(paddr,2) = destination of route
 * OBJ(paddr,3) = group to trace route via
 * OBJ(int,1) = ttl for multicast packets
 *
 */
EOLNS (exec_mtrace_eol, mbone_mtrace_command);
SET (exec_mtrace_ttl_default, exec_mtrace_eol, OBJ(int,1), 
     MBONE_MTRACE_DEFAULT_TTL);
NUMBER (exec_mtrace_ttl, exec_mtrace_eol, exec_mtrace_ttl_default,	
	OBJ(int,1), 1 ,255, "Time-to-live for multicasted trace request");
IPADDR_NAME (exec_mtrace_group, exec_mtrace_ttl, exec_mtrace_eol,
	     OBJ(paddr,3), "Group to trace route via");
IPADDR_NAME (exec_mtrace_dest, exec_mtrace_group, exec_mtrace_eol,
	     OBJ(paddr,2), "Destination of route");
IPADDR_NAME (exec_mtrace_source, exec_mtrace_dest, exec_mtrace_eol,
	     OBJ(paddr,1), "Source to trace route from");
KEYWORD (exec_mtrace, exec_mtrace_source, ALTERNATE, "mtrace",
	 "Trace reverse multicast path from destination to source", PRIV_USER);

/***********************************************************
 * mstat <source> <destination> <group> [<tll>]
 *
 * OBJ(paddr,1) = source to trace route from
 * OBJ(paddr,2) = destination or route
 * OBJ(paddr,3) = group for statistics
 * OBJ(int,1) = ttl for multicast packets
 * OBJ(int,2) = interval (in seconds) for queries
 */
EOLNS (exec_mstat_eol, mbone_mstat_command);
SET  (exec_mstat_ttl_default, exec_mstat_eol, OBJ(int,1),
      MBONE_MTRACE_DEFAULT_TTL);
NUMBER (exec_mstat_ttl, exec_mstat_eol, exec_mstat_ttl_default,
	OBJ(int,1), 1, 255, "Time-to-live for multicasted trace request");
IPADDR_NAME (exec_mstat_group, exec_mstat_ttl, exec_mstat_eol,
	     OBJ(paddr,3), "Group for statistics");
IPADDR_NAME (exec_mstat_dest, exec_mstat_group, exec_mstat_eol,
	     OBJ(paddr,2), "Destination of route");
IPADDR_NAME (exec_mstat_source, exec_mstat_dest, exec_mstat_eol,
	     OBJ(paddr,1), "Source to trace route from");
KEYWORD (exec_mstat, exec_mstat_source, exec_mtrace, "mstat",
	 "Show statistics after multiple multicast traceroutes", PRIV_USER);

/***********************************************************
 * mrinfo <router> [<interface>|<source address>]
 *
 * OBJ(paddr,1) = router to request info from
 * OBJ(paddr,2) = source address to send request from
 * OBJ(idb,  1) = source interface to send request from 
 */
EOLNS (exec_mrinfo_eol, mrinfo_command);

IPADDR_NAME (exec_mrinfo_source, exec_mrinfo_eol, exec_mrinfo_eol,
	     OBJ(paddr,2), "Source address or interface for request");

INTERFACE (exec_mrinfo_interface, exec_mrinfo_eol, exec_mrinfo_source,
	   OBJ(idb, 1), IFTYPE_ANYSUB);

IPADDR_NAME (exec_mrinfo_router, exec_mrinfo_interface, exec_mrinfo_eol,
	     OBJ(paddr,1), "Router to request info from");

KEYWORD (exec_mrinfo, exec_mrinfo_router, exec_mstat, "mrinfo",
	 "Request neighbor and version information from a multicast router", 
	 PRIV_USER);

#undef ALTERNATE
#define ALTERNATE exec_mrinfo
