/* $Id: exec_debug_tacacs.h,v 3.1.22.1 1996/04/19 15:29:19 che Exp $
 * $Source: /release/112/cvs/Xsys/servers/exec_debug_tacacs.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ T A C A C S . H
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_tacacs.h,v $
 * Revision 3.1.22.1  1996/04/19  15:29:19  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.1  1996/02/28  06:14:56  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 3.1  1996/02/16  00:43:23  hampton
 * Migrate more files out of the parser directory.  [CSCdi49139]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:46:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  14:49:12  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug tacacs [ events ]
 *
 */
EOLS	(debug_tacacs_eol, debug_command, DEBUG_TACACS);

EOLS	(debug_tacacs_events_eol, debug_command, DEBUG_TACACS_EVENTS);

KEYWORD(debug_tacacs_events, debug_tacacs_events_eol, debug_tacacs_eol,
	"events", "TACACS+ protocol events", PRIV_OPR);

KEYWORD_DEBUG(debug_tacacs, debug_tacacs_events, ALTERNATE,
	      OBJ(pdb,1), os_debug_arr,
	      "tacacs", "TACACS authentication and authorization", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_tacacs
