/* $Id: exec_debug_buffer.h,v 3.2.62.2 1996/08/04 06:27:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_debug_buffer.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ B U F F E R . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_buffer.h,v $
 * Revision 3.2.62.2  1996/08/04  06:27:18  thille
 * CSCdi64520:  %SYS-2-GETBUFFFAIL: Memory allocation of 67 bytes failed
 * Branch: California_branch
 * Add "debug buffer failures" command and default to office
 *
 * -Nick
 *
 * Revision 3.2.62.1  1996/06/04  05:15:57  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Added initial debug sanity code for memory corruptions
 *
 * Revision 3.2  1995/11/17  18:46:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:06  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/19  14:47:29  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug sanity checking
 *
 */
EOLS    (debug_sanity_eol, debug_command, DEBUG_SANITY);
KEYWORD_DEBUG (debug_sanity, debug_sanity_eol, ALTERNATE,
 	       OBJ(pdb,1), os_debug_arr,
 	       "sanity", "", PRIV_OPR | PRIV_HIDDEN);

#undef ALTERNATE
#define ALTERNATE debug_sanity
  

/******************************************************************
 * debug buffer [ failures ]
 *
 */
EOLS	(debug_buffer_eol, debug_command, DEBUG_BUFFER);
EOLS	(debug_buffer_fail_eol, debug_command, DEBUG_BUFFER_FAIL);
KEYWORD	(debug_buffer_fail, debug_buffer_fail_eol, debug_buffer_eol,
         "failures", NULL, PRIV_OPR | PRIV_HIDDEN);
KEYWORD_DEBUG(debug_buffer, debug_buffer_fail, ALTERNATE,
              OBJ(pdb,1), os_debug_arr,
              "buffer", NULL, PRIV_OPR | PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	debug_buffer
