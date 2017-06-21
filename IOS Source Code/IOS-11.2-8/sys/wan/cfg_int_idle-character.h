/* $Id: cfg_int_idle-character.h,v 3.3.62.1 1996/08/12 16:09:27 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_idle-character.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ I D L E - C H A R A C T E R . H
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_idle-character.h,v $
 * Revision 3.3.62.1  1996/08/12  16:09:27  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/11/17  18:01:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:48:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:52  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:05:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *	[no] idle-character {flags | marks}
 *
 * Valid for SYNC SERIAL excluding serial interfaces using MK5025, MCI
 * and SCI serial interfaces.
 */

EOLNS	(ci_idle_eol, serial_idle_command);

KEYWORD_ID (ci_idle_mark, ci_idle_eol, NONE,
	    OBJ(int,1), IDLE_MARKS, "marks",
	    "Send mark characters between packets", PRIV_CONF);

KEYWORD_ID (ci_idle_flag, ci_idle_eol, ci_idle_mark, 
	    OBJ(int,1), IDLE_FLAGS, "flags", 
	    "Send hdlc flag characters between packets", PRIV_CONF);

NOPREFIX(ci_idle_noprefix, ci_idle_flag, ci_idle_eol);

NVGENNS (ci_idle_nvgen, ci_idle_noprefix, serial_idle_command);

KEYWORD (ci_idle, ci_idle_nvgen, NONE,
	 "idle-character", "Set idle character type", PRIV_CONF);

ASSERT	(ci_idle_test, ci_idle, ALTERNATE,
	 csb->interface->hwptr && 
	 (csb->interface->hwptr->status & IDB_SYNC) &&
	 (csb->interface->hwptr->serial_flags & SERIAL_FLAGS_CAN_ALT_FLAG));

#undef	ALTERNATE
#define	ALTERNATE	ci_idle_test

