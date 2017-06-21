/* $Id: cfg_int_ip_rsvp.h,v 1.1.4.2 1996/08/12 16:07:02 widmer Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/cfg_int_ip_rsvp.h,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright(c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_rsvp.h,v $
 * Revision 1.1.4.2  1996/08/12  16:07:02  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 1.1.4.1  1996/04/17  13:57:13  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 * ip rsvp neighbor <access-list>
 * no ip rsvp neighbor <access-list>
 *
 * OBJ(int,1) = Access List
 */

EOLNS	(ci_ip_nbr_eol, rsvp_nbr_command);
NUMBER	(ci_ip_nbr_acl, ci_ip_nbr_eol, ci_ip_nbr_eol,
	 OBJ(int,1), 1, 199, "Access list");
NVGENNS	(ci_ip_nbr_nvgen, ci_ip_nbr_acl, rsvp_nbr_command);
KEYWORD	(ci_ip_nbr, ci_ip_nbr_nvgen, no_alt, "neighbor",
	 "Select Permissible RSVP Neighbors", PRIV_CONF | PRIV_SUBIF);

/************************************************************************
 * ip rsvp udp-multicasts [<multicast-address>]
 * no ip rsvp udp-multicasts [<multicast-address>]
 *
 * OBJ(paddr,1) = UDP Multicast Address
 */

EOLNS	(ci_ip_udp_rsvp_eol, rsvp_udp_rsvp_command);
IPADDR_NAME(ci_ip_udp_rsvp_group, ci_ip_udp_rsvp_eol, ci_ip_udp_rsvp_eol,
	    OBJ(paddr, 1), "UDP Multicast Address");
NVGENNS	(ci_ip_udp_rsvp_nvgen, ci_ip_udp_rsvp_group, rsvp_udp_rsvp_command);
KEYWORD(ci_ip_udp_rsvp, ci_ip_udp_rsvp_nvgen, ci_ip_nbr,
	"udp-multicasts", "Use RSVP UDP Multicasts",
	PRIV_CONF | PRIV_SUBIF);

/************************************************************************
 * ip rsvp bandwidth [<available-bandwidth> [<largest-flow>]]
 * no ip rsvp bandwidth [<available-bandwidth> [<largest-flow>]]
 * no ip rsvp
 *
 * OBJ(int,1) = available bandwidth
 * OBJ(int,2) = available bandwidth to one flow
 */
EOLNS	(ci_ip_bandwidth_eol, rsvp_bandwidth_command);
NUMBER	(ci_ip_bandwidth_flow, ci_ip_bandwidth_eol, ci_ip_bandwidth_eol,
	 OBJ(int, 2), 1, BANDWIDTH_SCALE, "Largest Reservable Flow(KBPS)");
NUMBER	(ci_ip_bandwidth_num, ci_ip_bandwidth_flow, ci_ip_bandwidth_eol,
	 OBJ(int, 1), 1, BANDWIDTH_SCALE, "Reservable Bandwidth(KBPS)");
NVGENNS	(ci_ip_bandwidth_nvgen, ci_ip_bandwidth_num, rsvp_bandwidth_command);
KEYWORD	(ci_ip_bandwidth, ci_ip_bandwidth_nvgen, ci_ip_udp_rsvp,
	 "bandwidth", "RSVP reservable bandwidth(KBPS)",
	 PRIV_CONF | PRIV_SUBIF);

KEYWORD(ci_ip_rsvp, ci_ip_bandwidth, ALTERNATE,
	"rsvp", "RSVP interface commands", PRIV_CONF | PRIV_SUBIF);

#undef ALTERNATE
#define ALTERNATE ci_ip_rsvp
