/* $Id: exec_debug_netbios_as.h,v 3.2.60.2 1996/08/14 20:55:17 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ibm/exec_debug_netbios_as.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ N E T B I O S _ A S . H
 *
 * September 1995, Shoou Yiu
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_netbios_as.h,v $
 * Revision 3.2.60.2  1996/08/14  20:55:17  widmer
 * CSCdi66037:  Bogus no_alt usage in show nbf
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/06/21  00:02:04  syiu
 * CSCdi60949:  NBFCP debugs not working in -j
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:22:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  00:22:27  syiu
 * Initial placeholder for NBF (NetBIOS Frames) support.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * debug nbf error
 */

EOLS    (debug_netbios_error_eol, debug_command, DEBUG_NETBIOS_ERROR);
KEYWORD (debug_netbios_error, debug_netbios_error_eol, no_alt,
	 "error", "NetBIOS Protocol errors", PRIV_OPR);
/*
 * debug nbf packet
 */

EOLS    (debug_netbios_packet_eol, debug_command, DEBUG_NETBIOS_PACKET);
KEYWORD (debug_netbios_packet, debug_netbios_packet_eol, debug_netbios_error,
	 "packet", "NetBIOS packet", PRIV_OPR);

/*
 * debug nbf name-cache
 */

EOLS    (debug_netbios_namecache_eol, debug_command, DEBUG_NETBIOS_NAMEC);
KEYWORD (debug_netbios_namec, debug_netbios_namecache_eol,debug_netbios_packet,
	 "name-cache", NULL /*"NetBIOS name cache tracing"*/,
	 PRIV_OPR|PRIV_HIDDEN);


/******************************************************************
 * debug nbf
 */
KEYWORD_DEBUG(debug_netbios_as, debug_netbios_namec, ALTERNATE,
	      OBJ(pdb,1), netbios_as_debug_arr,
	      "nbf", "NetBIOS information", PRIV_OPR);


/******************************************************************
 * debug netbios-name-cache
 */
/*
EOLS	(debug_netbios_eol, debug_command, DEBUG_NETBIOS_NAMEC);
KEYWORD_DEBUG(debug_netbios, debug_netbios_eol, ALTERNATE,
	      OBJ(pdb,1), netbios_as_debug_arr,
	      "netbios-name-cache", "NetBIOS name cache tracing", PRIV_OPR);
*/

#undef	ALTERNATE
#define	ALTERNATE	debug_netbios_as
