/* $Id: cfg_int_clocksource.h,v 3.3.62.2 1996/08/12 16:09:22 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_clocksource.h,v $
 *------------------------------------------------------------------
 * cfg_int_clocksource.h
 *
 * 08-June-1994, Rob Haragan
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cfg_int_clocksource.h,v $
 * Revision 3.3.62.2  1996/08/12  16:09:22  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.62.1  1996/03/18  22:46:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  02:11:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  21:49:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:00:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:47:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:48  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:04:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *	clock source line|internal
 *	no clock source [line]|[internal]
 *
 * OBJ(int,1) = <speed>
 */


EOLNS    (ci_clock_source_eol, serial_clocksource_command);

KEYWORD_ID (ci_clock_source_line, ci_clock_source_eol,
	 NONE, OBJ(int,1), FALSE, "line",
	 "Recover G.703 clock from line",
	 PRIV_CONF);

KEYWORD_ID (ci_clock_source_internal, ci_clock_source_eol,
	 ci_clock_source_line, OBJ(int,1), TRUE, "internal",
	 "Use G.703 port adapter internal clock",
	 PRIV_CONF);

NVGENNS	(ci_clock_source_nvgen, ci_clock_source_internal, 
	 serial_clocksource_command);

KEYWORD (ci_clock_source, ci_clock_source_nvgen,
	 NONE,"source",
	 "Configure G.703 interface clock source",
	 PRIV_CONF);

NOPREFIX (ci_clock_source_nocheck, ci_clock_source,
	  ci_clock_source_eol);

KEYWORD	(config_clocksource, ci_clock_source_nocheck, NONE,
	 "clock", "Configure serial interface clock source",
	 PRIV_CONF);

ASSERT (config_clocksource_test, config_clocksource, ALTERNATE, 
	is_g703(csb->interface->hwptr)); 

#undef	ALTERNATE
#define	ALTERNATE	config_clocksource_test

