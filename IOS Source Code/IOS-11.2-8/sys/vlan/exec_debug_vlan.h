/*-------------------------------------------------------------------------
 *
 * $Id: exec_debug_vlan.h,v 3.3 1996/02/01 23:34:56 widmer Exp $
 *
 * vlan/exec_debug_vlan.h
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Parser support for Virtual LAN debug commands.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: exec_debug_vlan.h,v $
 * Revision 3.3  1996/02/01  23:34:56  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1996/01/23  08:55:16  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


/******************************************************************
 *
 * debug vlan packets
 *
 ******************************************************************/

EOLS	(debug_vlan_packets_eol, debug_command, DEBUG_VLAN_PACKETS);

KEYWORD (debug_vlan_packets, debug_vlan_packets_eol, no_alt,
	 "packets", "vLAN packet information", PRIV_OPR);

/******************************************************************
 *
 * debug vlan
 *
 ******************************************************************/

KEYWORD_DEBUG(debug_vlan, debug_vlan_packets, ALTERNATE,
	      OBJ(pdb,1), vlan_debug_arr,
	      "vlan", "vLAN information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_vlan
