/* $Id: cfg_int_ignore_dcd.h,v 3.2.60.3 1996/08/12 16:09:28 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_ignore_dcd.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ I G N O R E _ D C D . H 
 *
 * Oct 1994, Mohsen Mortazavi
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_ignore_dcd.h,v $
 * Revision 3.2.60.3  1996/08/12  16:09:28  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.2  1996/03/22  00:15:16  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.60.1  1996/03/18  22:46:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.1  1996/01/26  02:51:27  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.2  1995/11/17  18:01:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/31  22:37:46  izhu
 * CSCdi32813:  SDLC Multidrops need router to ignore DCD for High-End
 * Platform
 * and
 * CSCdi38317  STUN: cannot IPL a 3725 using FSIP
 *
 * Convert to midrange signal model for line state (RTS/CTS state not
 * used),
 * permit DCD to be ignored.
 *
 * Revision 2.1  1995/07/02  01:45:53  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:05:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] ignore-dcd
 *
 */
EOLNS    (ci_igndcd_eol, serial_ignore_dcd_command);
KEYWORD (ci_igndcd, ci_igndcd_eol, NONE,
         "ignore-dcd", "ignore dcd", PRIV_CONF);

ASSERT (ci_igndcd_test, ci_igndcd, ALTERNATE,
        (csb->interface->hwptr->status & IDB_SERIAL) &&
        (csb->interface->hwptr->serial_flags & SERIAL_FLAGS_CAN_IGNORE_DCD) &&
        !is_g703(csb->interface->hwptr));

#undef  ALTERNATE
#define ALTERNATE       ci_igndcd_test


