/* $Id: exec_debug_arap.h,v 3.2 1995/11/17 08:39:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/arap/exec_debug_arap.h,v $
 *------------------------------------------------------------------
 * ARAP debug commands
 *
 * October 1993, Steven Lin
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_arap.h,v $
 * Revision 3.2  1995/11/17  08:39:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:34:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.2  1995/06/19  14:47:10  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug arap
 *
 */

#include "arap_public.h"

EOLS	(debug_arap_v42bis_eol, ara_debug_command, DEBUG_ARAP_V42BIS);
SET     (debug_arap_v42bis_tty, debug_arap_v42bis_eol, OBJ(int,1), NOTTY_DEBUG);
TTYLINE (debug_arap_v42bis_line, debug_arap_v42bis_eol,
	debug_arap_v42bis_tty, OBJ(int,1), "Line number");
KEYWORD (debug_arap_v42bis, debug_arap_v42bis_line, no_alt,
	"v42bis", "V.42bis compression", PRIV_OPR);

EOLS    (debug_arap_mnp4_eol, ara_debug_command, DEBUG_ARAP_MNP4);
SET     (debug_arap_mnp4_tty, debug_arap_mnp4_eol, OBJ(int,1), NOTTY_DEBUG);
TTYLINE (debug_arap_mnp4_line, debug_arap_mnp4_eol,
	debug_arap_mnp4_tty, OBJ(int,1), "Line number");
KEYWORD (debug_arap_mnp4, debug_arap_mnp4_line, debug_arap_v42bis,
	"mnp4", "MNP4", PRIV_OPR);

EOLS    (debug_arap_memory_eol, ara_debug_command, DEBUG_ARAP_MEMORY);
SET     (debug_arap_memory_tty, debug_arap_memory_eol, OBJ(int,1), NOTTY_DEBUG);
TTYLINE (debug_arap_memory_line, debug_arap_memory_eol,
	debug_arap_memory_tty, OBJ(int,1), "Line number");
KEYWORD (debug_arap_memory, debug_arap_memory_line, debug_arap_mnp4,
	"memory", "Memory handling", PRIV_OPR);

EOLS    (debug_arap_internal_eol, ara_debug_command, DEBUG_ARAP_INTERNAL);
SET     (debug_arap_internal_tty, debug_arap_internal_eol, OBJ(int,1),
	NOTTY_DEBUG);
TTYLINE (debug_arap_internal_line, debug_arap_internal_eol,
	debug_arap_internal_tty, OBJ(int,1), "Line number");
KEYWORD (debug_arap_internal, debug_arap_internal_line, debug_arap_memory,
	"internal", "Internal packets", PRIV_OPR);

KEYWORD_DEBUG(debug_arap, debug_arap_internal, ALTERNATE,
	      OBJ(pdb,1), ara_debug_arr,
	      "arap", "Appletalk Remote Access", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_arap
