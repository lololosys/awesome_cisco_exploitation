/* $Id: cfg_int_netbios_as.h,v 3.2.60.3 1996/08/14 20:55:16 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ibm/cfg_int_netbios_as.h,v $
 *------------------------------------------------------------------
 * cfg_int_netbios_as.h - NBF interface commands
 *
 * September 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_netbios_as.h,v $
 * Revision 3.2.60.3  1996/08/14  20:55:16  widmer
 * CSCdi66037:  Bogus no_alt usage in show nbf
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/07/11  19:19:12  syiu
 * CSCdi62549:  NBFCP disabled on Vaccess interfaces
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  20:11:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:23:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  14:20:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:21:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  00:21:11  syiu
 * Initial placeholder for NBF (NetBIOS Frames) support
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] netbios nbf
 *
 */
EOLS	(ci_netbios_netbeui_eol, netbios_as_if_command,
	 NETBIOS_ENABLE_NETBEUI);
KEYWORD (ci_netbios_netbeui, ci_netbios_netbeui_eol, NONE,
	 "nbf", "Enable NetBIOS Frames Protocol (NetBEUI) on this interface",
	 PRIV_CONF);
ASSERT  (ci_netbios_netbeui_check, ci_netbios_netbeui, ALTERNATE,
    (is_async(csb->interface->hwptr) ||
     is_isdn(csb->interface->hwptr) ||
     (csb->interface->hwptr->type == IDBTYPE_VACCESS) ||
     (csb->interface->hwptr->type == IDBTYPE_VTEMPLATE) || 
     is_ethernet(csb->interface->hwptr) ||
     is_tokenring(csb->interface->hwptr)));

#undef	ALTERNATE
#define	ALTERNATE	ci_netbios_netbeui_check
