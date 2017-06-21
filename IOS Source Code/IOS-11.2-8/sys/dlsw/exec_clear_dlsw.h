/* $Id: exec_clear_dlsw.h,v 3.3.20.2 1996/08/12 16:01:13 widmer Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/exec_clear_dlsw.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ D L S W . H
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_clear_dlsw.h,v $
 * Revision 3.3.20.2  1996/08/12  16:01:13  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.1  1996/07/19  03:55:34  athippes
 * CSCdi60758:  dlsw : duplicate circuit identifier
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/29  07:08:34  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  09:04:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:25:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/04  01:52:52  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:15:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * clear dlsw {circuits | reachability | fastcache | local-circuit"}
 * (no options implies all)
 *
 * OBJ(int,1) = command type DLSW_CLEAR_{CIRCUITS,CACHE,FAST}  
 *
 * If command type "clear dlsw fastcache" 
 * -----------------------------------------
 * options:
 * none at this time
 *
 * If command type "clear dlsw reachability" 
 * -----------------------------------------
 * options:
 * none at this time
 *
 * If command type is "clear dlsw circuit" 
 * --------------------------------------
 * options :
 * { <Ckt ID> | <cr> for all}
 *
 * OBJ(int,3) = <Ckt ID>
 *
 * If command type is "clear dlsw local-circuit"
 * --------------------------------------
 * options:
 * { <Ckt ID> | <cr> for all}
 *
 * OBJ(int,3) = <Ckt ID>
 */

EOLNS	(clear_dlsw_eol, clear_dlsw);

NUMBER(clear_local_major, clear_dlsw_eol, clear_dlsw_eol,
       OBJ(int,3), 0, 0xFFFFFFFF,
       "Circuit ID for a specific local circuit");

SET     (clear_local_init1, clear_local_major, OBJ(int,3), -1);

KEYWORD_ID(clear_local_ckt, clear_local_init1, no_alt,
           OBJ(int,1), CLEAR_DLSW_LOC_CKT, "local-circuit",
           "Display DLSw local circuits", PRIV_USER);

KEYWORD_ID(clear_dlsw_fast, clear_dlsw_eol, clear_local_ckt,
           OBJ(int,1), CLEAR_DLSW_FAST, "fastcache",
           "Clear DLSw FST cache - causes user sessions to disconnect",
           PRIV_USER | PRIV_HIDDEN);

KEYWORD_ID(clear_dlsw_cache, clear_dlsw_eol, clear_dlsw_fast,
           OBJ(int,1), CLEAR_DLSW_CACHE, "reachability", 
           "Clear DLSw reachability cache", 
           PRIV_USER);

NUMBER(clear_ckt_major, clear_dlsw_eol, clear_dlsw_eol,
       OBJ(int,3), 0, 0xFFFFFFFF,
       "Circuit ID for a specific remote circuit");
 
SET     (clear_ckt_init1, clear_ckt_major, OBJ(int,3), -1);

KEYWORD_ID(clear_dlsw_circuits, clear_ckt_init1, clear_dlsw_cache,
           OBJ(int,1), CLEAR_DLSW_CIRCUITS, "circuits", 
           "Clear DLSw circuits", PRIV_USER);


/******************************************************************
 * clear dlsw
 */
KEYWORD (clear_dlsw_def, clear_dlsw_circuits, NONE,
	 "dlsw", "Data Link Switching (DLSw) ",
	 PRIV_OPR);

TEST_BOOLEAN(clear_dlsw_test, clear_dlsw_def, NONE, ALTERNATE, bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	clear_dlsw_test


