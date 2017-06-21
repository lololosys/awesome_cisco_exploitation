/* $Id: cfg_int_arp.h,v 3.2 1995/11/17 09:32:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/cfg_int_arp.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ A R P . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_arp.h,v $
 * Revision 3.2  1995/11/17  09:32:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:53:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/09  20:58:58  pst
 * CSCdi40821:  nagative values in arp timeout configuration
 *
 * Revision 2.1  1995/07/01  12:55:34  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:03:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************
 * arp { arpa | probe | snap | frame-relay | timeout <seconds> } 
 * no arp { arpa | probe | snap | frame-relay | timeout [<seconds>] }
 *
 * csb->which = ARP_ARPA, ARP_PROBE, ARP_SNAP, ARP_FR or ARP_ENTRY_TIME
 * OBJ(int,1) = seconds for ARP_ENTRY_TIME
 *
 */

/* arp timeout <seconds> */
PARAMS_KEYONLY (ci_arp_timeout, no_alt,
	"timeout", OBJ(int,1), 0, (MAXUINT / ONESEC),
	ip_arp_ifcommand, ARP_ENTRY_TIME, 
	"Set ARP cache timeout", "Seconds", PRIV_CONF|PRIV_SUBIF);

/* arp snap */
EOLS	(ci_arp_snap_eol, ip_arp_ifcommand, ARP_SNAP);
KEYWORD	(ci_arp_snap, ci_arp_snap_eol, ci_arp_timeout,
	 "snap", "IEEE 802.3 style arp", PRIV_CONF);

/* arp frame-relay */
EOLS	(ci_arp_fr_eol, ip_arp_ifcommand, ARP_FR);
KEYWORD	(ci_arp_fr, ci_arp_fr_eol, ci_arp_snap, 
	 "frame-relay", "Enable ARP for a frame relay interface",
	 PRIV_CONF|PRIV_SUBIF);

/* arp probe */
EOLS	(ci_arp_probe_eol, ip_arp_ifcommand, ARP_PROBE);
KEYWORD	(ci_arp_probe, ci_arp_probe_eol, ci_arp_fr, 
	 "probe", "HP style arp protocol", PRIV_CONF);

/* arp arpa */
EOLS	(ci_arp_arpa_eol, ip_arp_ifcommand, ARP_ARPA);
KEYWORD	(ci_arp_arpa, ci_arp_arpa_eol, ci_arp_probe, 
	 "arpa", "Standard arp protocol", PRIV_CONF);

KEYWORD	(ci_arp, ci_arp_arpa, ALTERNATE, "arp",
	 "Set arp type (arpa, probe, snap) or timeout", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_arp
