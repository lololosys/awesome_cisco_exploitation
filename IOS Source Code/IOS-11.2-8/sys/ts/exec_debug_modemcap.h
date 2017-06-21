/* $Id: exec_debug_modemcap.h,v 3.3 1996/02/23 04:12:01 tkolar Exp $
 * $Source: /release/111/cvs/Xsys/ts/exec_debug_modemcap.h,v $
 *------------------------------------------------------------------
 * exec_debug_modemcap.h  --  parser stuff for debug modemcap
 *
 * June 1995, Tim Kolar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_modemcap.h,v $
 * Revision 3.3  1996/02/23  04:12:01  tkolar
 * CSCdi49648:  Modem autoconfig break modularity rules
 * Move confmodem_debug out of the commserver subsystem.
 *
 * Revision 3.2  1996/02/01  23:34:47  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.1  1996/02/01  04:59:37  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:46:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/26  01:07:51  tkolar
 * *** empty log message ***
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


EOLS	(debug_modemcap_eol, debug_command, DEBUG_MODEMCAP);

KEYWORD_DEBUG(debug_modemcap, debug_modemcap_eol, ALTERNATE,
	      OBJ(pdb,1), modem_config_debug_arr,
	      "confmodem", "Modem configuration database", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_modemcap
