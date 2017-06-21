/* $Id: cfg_int_rxsync.h,v 3.3.62.1 1996/08/12 16:09:31 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_rxsync.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ R X S Y N C . H 
 *
 * May 1994, Mohsen Mortazavi
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_rxsync.h,v $
 * Revision 3.3.62.1  1996/08/12  16:09:31  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/11/17  18:01:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:48:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:56  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:07:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] rx-sync-disable
 *
 */
EOLNS    (ci_rxsync_eol, serial_rx_sync_disable_command);
KEYWORD (ci_rxsync, ci_rxsync_eol, NONE,
         "rx-sync-disable", "rx sync disable", PRIV_CONF);

ASSERT (ci_rxsync_test, ci_rxsync, ALTERNATE,
        (csb->interface->hwptr->serial_flags & SERIAL_FLAGS_CAN_DIS_RX_SYNC));

#undef  ALTERNATE
#define ALTERNATE       ci_rxsync_test


