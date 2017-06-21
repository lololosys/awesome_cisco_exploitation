/* $Id: cfg_int_transmit-interface.h,v 3.3.28.1 1996/08/12 16:05:47 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_int_transmit-interface.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ T R A N S M I T - I N T E R F A C E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_transmit-interface.h,v $
 * Revision 3.3.28.1  1996/08/12  16:05:47  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/18  02:14:16  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  18:43:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:07:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * transmit-interface <interface><unit>
 * no transmit-interface [<interface><unit>]
 *
 * OBJ(idb,1) = <interface>
 */
EOLNS	(ci_transif_eol, transmitif_command);

FORWARD_INTERFACE (ci_transif_interface, ci_transif_eol, no_alt,
		   OBJ(idb,1), IFTYPE_HWIDB      & ~IFTYPE_VTY_ASYNC);

NOPREFIX (ci_transif_noprefix, ci_transif_interface, ci_transif_eol);
NVGENNS	(ci_transif_nvgen, ci_transif_noprefix, transmitif_command);
KEYWORD	(ci_transif, ci_transif_nvgen, ALTERNATE,
	 "transmit-interface", 
	 "Assign a transmit interface to a receive-only interface", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	ci_transif
