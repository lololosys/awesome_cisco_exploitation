/* $Id: cfg_int_mtu.h,v 3.2.62.2 1996/08/12 16:05:45 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_int_mtu.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ M T U . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_mtu.h,v $
 * Revision 3.2.62.2  1996/08/12  16:05:45  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.1  1996/05/09  14:40:19  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:17:28  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.1  1996/04/08  02:00:11  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.1  1996/03/28  17:35:35  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.2  1995/11/17  18:43:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:06:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * mtu [<bytes>]
 * no mtu
 */
EOLNS	(ci_mtu_eol, max_mtu_command);

static void ci_mtu_range (parseinfo *csb, uint *lower, uint *upper)
{
    hwidbtype *hwidb = csb->interface->hwptr;

    *lower = 64;
    *upper = DEF_HUGEDATA - (hwidb->max_buffer_size - hwidb->maxmtu);
}

NUMBER_FUNC(ci_mtu_bytes, ci_mtu_eol, no_alt,
	    OBJ(int,1), ci_mtu_range, "MTU size in bytes");
NOPREFIX(ci_mtu_noprefix, ci_mtu_bytes, ci_mtu_eol);
NVGENNS	(ci_mtu_nvgen, ci_mtu_noprefix, max_mtu_command);
KEYWORD	(ci_mtu, ci_mtu_nvgen, ALTERNATE,
	 "mtu", "Set the interface Maximum Transmission Unit (MTU)",
	 PRIV_CONF | PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_mtu
