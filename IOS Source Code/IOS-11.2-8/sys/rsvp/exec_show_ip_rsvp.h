/* $Id: exec_show_ip_rsvp.h,v 1.1.4.4 1996/08/12 16:07:04 widmer Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/exec_show_ip_rsvp.h,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ip_rsvp.h,v $
 * Revision 1.1.4.4  1996/08/12  16:07:04  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 1.1.4.3  1996/08/09  00:18:01  asastry
 * CSCdi65504:  Allow <debug ip rsvp detail> command to be RESV-only or
 * PATH-only
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/05/21  10:01:14  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.4.1  1996/04/17  13:57:15  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip rsvp installed [<interface-unit>]
 *
 *	OBJ(idb1,1) = optional interface name.
 */
EOLNS(show_ip_install_eol, rsvp_show_install);
SET(show_ip_install_all, show_ip_install_eol, OBJ(idb,1), NULL);
INTERFACE(show_ip_install_idb, show_ip_install_eol, show_ip_install_all,
	  OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD(show_ip_install, show_ip_install_idb, no_alt,
	"installed", "RSVP installed reservations", PRIV_USER);

/******************************************************************
 * show ip rsvp interface [<interface-unit>]
 *
 *	OBJ(idb1,1) = optional interface name.
 */
EOLNS(show_ip_int_eol, rsvp_show_interface);
SET(show_ip_int_all, show_ip_int_eol, OBJ(idb,1), NULL);
INTERFACE(show_ip_int_idb, show_ip_int_eol, show_ip_int_all,
	  OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD(show_ip_int, show_ip_int_idb, show_ip_install,
	common_str_interface, "RSVP interface information", PRIV_USER);

/******************************************************************
 * show ip rsvp reservation [<group-name> | <group-address>]
 *
 *	OBJ(paddr,1) = IP group address
 */
EOLNS(show_ip_downstream_eol, rsvp_show_reservation);
SET(show_ip_downstream_set, show_ip_downstream_eol, OBJ(paddr,1), 0);
IPADDR_NAME(show_ip_downstream_name, show_ip_downstream_eol,
	    show_ip_downstream_set, OBJ(paddr,1), "IP name or group address");
KEYWORD(show_ip_downstream, show_ip_downstream_name, show_ip_int,
	"reservation", "RSVP Reservation Requests from Downstream", PRIV_USER);

/******************************************************************
 * show ip rsvp request [<group-name> | <group-address>]
 *
 *	OBJ(paddr,1) = IP group address
 */
EOLNS(show_ip_upstream_eol, rsvp_show_request);
SET(show_ip_upstream_set, show_ip_upstream_eol, OBJ(paddr,1), 0);
IPADDR_NAME(show_ip_upstream_name, show_ip_upstream_eol,
	    show_ip_upstream_set, OBJ(paddr,1), "IP name or group address");
KEYWORD(show_ip_upstream, show_ip_upstream_name, show_ip_downstream,
	"request", "RSVP Reservations Upstream", PRIV_USER);

/******************************************************************
 * show ip rsvp sender [<group-name> | <group-address>]
 *
 *	OBJ(paddr,1) = IP group address
 */
EOLNS(show_ip_path_eol, rsvp_show_path);
SET(show_ip_path_set, show_ip_path_eol, OBJ(paddr,1), 0);
IPADDR_NAME(show_ip_path_name, show_ip_path_eol, show_ip_path_set,
	    OBJ(paddr,1), "IP name or group address");
KEYWORD(show_ip_path, show_ip_path_name, show_ip_upstream,
	"sender", "RSVP Path State information", PRIV_USER);

/******************************************************************
 * show ip rsvp neighbor [<interface-unit>]
 *
 *	OBJ(idb,1) = optional interface name.
 */
EOLNS(show_ip_nbr_eol, rsvp_show_neighbor);
SET(show_ip_nbr_set, show_ip_nbr_eol, OBJ(idb,1), 0);
INTERFACE(show_ip_nbr_idb, show_ip_nbr_eol, show_ip_nbr_set,
	  OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD(show_ip_nbr, show_ip_nbr_idb, show_ip_path,
	"neighbor", "RSVP neighbor information", PRIV_USER);

KEYWORD(show_ip_rsvp, show_ip_nbr, ALTERNATE,
	"rsvp", "RSVP information", PRIV_USER);

#undef ALTERNATE
#define ALTERNATE show_ip_rsvp
