/* $Id: exec_debug_modem.h,v 3.2.62.1 1996/06/16 21:17:46 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_debug_modem.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ M O D E M . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_modem.h,v $
 * Revision 3.2.62.1  1996/06/16  21:17:46  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2  1995/11/17  18:46:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  14:48:26  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:19:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug modem
 *
 */

EOLS	(debug_modem_eol, debug_command, DEBUG_MODEM);
LINK_TRANS(debug_modem_extend_here, debug_modem_eol);
KEYWORD_DEBUG(debug_modem, debug_modem_extend_here, ALTERNATE,
	      OBJ(pdb,1), os_debug_arr,
	      "modem", "Modem control/process activation", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_modem
