/* $Id: cfg_int_fr-arp.h,v 3.1.32.1 1996/04/25 23:23:04 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_fr-arp.h,v $
 *------------------------------------------------------------------
 * frame-relay Inverse ARP specific commands
 *
 * January 1996, Charles Poe
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved. 
 *------------------------------------------------------------------
 * $Log: cfg_int_fr-arp.h,v $
 * Revision 3.1.32.1  1996/04/25  23:23:04  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.38.1  1996/04/24  04:11:11  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1996/02/10  00:42:28  ccpoe
 * Frame Relay ARP subsystem
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/***************************************************************
 * frame-relay inverse-arp [<protocol> | <interval>] <number>
 * no frame-relay inverse-arp [ <protocol> | <interval> ] <number>
 */
EOLS	(ci_inarp_eol, fr_arp_commands, FR_IN_ARP);
pdecl(ci_inarp_opts);
NUMBER	(ci_inarp_num, ci_inarp_eol, no_alt,
	 OBJ(int,3), 16, 1007, "Set DLCI for inverse ARP");
NUMBER	(ci_inarp_intv, ci_inarp_eol, no_alt,
	 OBJ(int,2), 15, 300, "Set interval for inverse ARP");
KEYWORD_ID (ci_inarp_interval, ci_inarp_intv, no_alt,
	    OBJ(int, 1), FR_ARP_INTV,
	 "interval", "Set inarp time interval on an interface", PRIV_CONF);
PROTONAME (ci_inarp_protoname, ci_inarp_num, ci_inarp_interval,
	   OBJ(int,10), PMATCH_DGRAM PMATCH_EXCLUDE
	   PMATCH_AARP PMATCH_ARP PMATCH_CLNS PMATCH_SNAPSHOT);
NOP	(ci_inarp_opts, ci_inarp_eol, ci_inarp_protoname);

NVGENS	(ci_inarp_nvgen, ci_inarp_opts,
	 fr_arp_commands, FR_IN_ARP);

KEYWORD	(ci_FR_inarp, ci_inarp_nvgen, NONE,
	    "inverse-arp", "Enable/disable inverse ARP on a DLCI",
	 PRIV_CONF|PRIV_SUBIF);

    
#undef ALTERNATE
#define ALTERNATE	ci_FR_inarp
