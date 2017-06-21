/* $Id: vip_dbus_chain.h,v 3.2.62.1 1996/08/12 16:07:41 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_dbus_chain.h,v $
 *------------------------------------------------------------------
 * vip_dbus_chain.h : VIP and Checkers DBus parser chains
 *
 * March 1995, Michael Beesley 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: vip_dbus_chain.h,v $
 * Revision 3.2.62.1  1996/08/12  16:07:41  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:51:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Forward delarations
 */
static void show_vip_dbus (parseinfo *csb);

/*
 * show diagbus
 */

EOLNS	(show_vip_dbus_eol, show_vip_dbus);

KEYWORD (show_vip_dbus, show_vip_dbus_eol, no_alt,
	 "diagbus", "DBUS Software Interface information",
	 PRIV_USER);

LINK_POINT (show_vip_dbus_command, show_vip_dbus);

/* end of file */
