/* $Id: cfg_int_cons.h,v 3.2 1995/11/17 18:05:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/cfg_int_cons.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ C O N S . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_cons.h,v $
 * Revision 3.2  1995/11/17  18:05:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:00  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:04:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************
 * [no] cmns enable
 *
 */
EOLS	(cfg_cons_eol, cons_intfc_command, CONS_ENABLE);
KEYWORD	(cfg_cons_enable, cfg_cons_eol, no_alt,
	 "enable", "Enable CMNS", PRIV_CONF);
KEYWORD(cfg_cons, cfg_cons_enable, NONE,
	 "cmns", "OSI CMNS", PRIV_CONF);

ASSERT (cfg_cons_test, cfg_cons, ALTERNATE, ISIEEEMSK(csb->interface->hwptr));

#undef	ALTERNATE
#define	ALTERNATE	cfg_cons_test
