/* $Id: cfg_int_clockrate.h,v 3.3.30.3 1996/09/09 01:33:01 xtang Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_clockrate.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ C L O C K R A T E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_clockrate.h,v $
 * Revision 3.3.30.3  1996/09/09  01:33:01  xtang
 * CSCdi56632:  unknown clock rates on 4T PA, FSIP
 * Branch: California_branch
 *         add ALT_HELPS to use csb->which in different
 *         driver procedures, and print the
 *         driver specific info. In this case, the clockrate
 *         option table, and csb->which = CFG_INT_CLOCKRATE
 *
 * Revision 3.3.30.2  1996/08/12  16:09:21  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.30.1  1996/06/17  08:36:43  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.3  1996/01/15  19:38:36  widmer
 * CSCdi12645:  clockrate help command incorrect
 *
 * Revision 3.2  1995/11/17  18:00:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:47  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:04:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *	clockrate <speed>	(Deprecated)
 *	no clockrate [<speed>]
 *
 * OBJ(int,1) = <speed>
 */

EOLNS	(config_clockrate_eol, serial_clockrate_command);
ALT_HELPS(config_clockrate_list, serial_clockrate_command, CFG_INT_CLOCKRATE);
NUMBER(config_clockrate_options, config_clockrate_eol, 
       config_clockrate_list,
       OBJ(int,1), 300, 8000000, 
       "  Choose clockrate from list above");

NOPREFIX(config_clockrate_noprefix, config_clockrate_options,
	 config_clockrate_eol);
NVGENNS	(config_clockrate_nvgen, config_clockrate_noprefix,
	 serial_clockrate_command);

KEYWORD_MM(config_clockrate, config_clockrate_nvgen, NONE,
	   "clockrate", NULL, PRIV_CONF | PRIV_HIDDEN, 6);

/***************************************************************
 *	clock rate <speed>
 *	no clock rate [<speed>]
 *
 * OBJ(int,1) = <speed>
 */

KEYWORD	(config_clock_rate, config_clockrate_nvgen, no_alt,
	 "rate", "Configure serial interface clock speed",
	 PRIV_CONF | PRIV_NONVGEN);

KEYWORD	(config_clock, config_clock_rate, config_clockrate,
	 "clock", "Configure serial interface clock",
	 (PRIV_CONF | PRIV_NONVGEN));

ASSERT (config_clock_test, config_clock, ALTERNATE,
	((csb->interface->hwptr->status & IDB_SERIAL) &&
	 !(csb->interface->hwptr->serial_flags & SERIAL_FLAGS_NO_CLOCKRATE)));

#undef	ALTERNATE
#define	ALTERNATE	config_clock_test
