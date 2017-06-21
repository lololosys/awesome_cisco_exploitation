/* $Id: exec_debug_dnsix.h,v 3.2 1995/11/17 18:51:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/servers/exec_debug_dnsix.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ D N S I X . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_dnsix.h,v $
 * Revision 3.2  1995/11/17  18:51:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:45  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.3  1995/06/19  14:47:51  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:39  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:18:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug dnsix nat
 *
 */
EOLS	(debug_dnsix_nat_eol, debug_command, DEBUG_DNSIX_NAT);
KEYWORD (debug_dnsix_nat, debug_dnsix_nat_eol, no_alt,
	 "nat", "Debug DNSIX/NAT protocol", PRIV_OPR);

/******************************************************************
 * debug dnsix dmdp
 *
 */
EOLS	(debug_dnsix_dmdp_eol, debug_command, DEBUG_DNSIX_DMDP);
KEYWORD (debug_dnsix_dmdp, debug_dnsix_dmdp_eol, debug_dnsix_nat,
	 "dmdp", "Debug DNSIX/DMDP protocol", PRIV_OPR);
/******************************************************************
 * debug dnsix
 *
 */
KEYWORD_DEBUG(debug_dnsix, debug_dnsix_dmdp, ALTERNATE,
	      OBJ(pdb,1), dnsix_debug_arr,
	      "dnsix", "Dnsix information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_dnsix
