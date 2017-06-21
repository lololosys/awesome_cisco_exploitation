/* $Id: exec_debug_nasi.h,v 3.2.62.1 1996/07/25 09:58:45 snyder Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_debug_nasi.h,v $
 *------------------------------------------------------------------
 * exec_debug_nasi.h   NASI parser chain for "debug nasi" command
 *
 * Sept 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_nasi.h,v $
 * Revision 3.2.62.1  1996/07/25  09:58:45  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  19:21:32  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  12:13:07  irfan
 * Netware Asynchronous Services Interface: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ipx nasi { error | packets | activity }
 *
 */
EOLS	(debug_nasi_activity_eol, debug_command, DEBUG_NASIACTIVITY);

EOLS	(debug_nasi_packets_eol, debug_command, DEBUG_NASIPACKETS);

EOLS	(debug_nasi_error_eol, debug_command, DEBUG_NASIERROR);


KEYWORD (debug_nasi_activity,
	 debug_nasi_activity_eol,
	 no_alt,
	 "activity", "NASI protocol activity", PRIV_OPR);

KEYWORD (debug_nasi_packets,
	 debug_nasi_packets_eol,
	 debug_nasi_activity,
	 "packets", "NASI protocol packets sent or received", PRIV_OPR);

KEYWORD (debug_nasi_error,
	 debug_nasi_error_eol,
	 debug_nasi_packets,
	 "error", "Errors in NASI protocol activity", PRIV_OPR);

KEYWORD_DEBUG(debug_nasi_cmds, debug_nasi_error, ALTERNATE,
	      OBJ(pdb,1), nasi_debug_arr,
	      "nasi", "NASI server functionality", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_nasi_cmds
