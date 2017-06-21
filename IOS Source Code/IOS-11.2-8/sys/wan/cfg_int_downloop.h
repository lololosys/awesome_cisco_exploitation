/* $Id: cfg_int_downloop.h,v 3.3.62.1 1996/08/12 16:09:25 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_downloop.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ D O W N L O O P . H
 *
 * February 1993, Lionel Bustini
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *
 * Command to force HDLC serial lines down when loopback is detected.
 *------------------------------------------------------------------
 * $Log: cfg_int_downloop.h,v $
 * Revision 3.3.62.1  1996/08/12  16:09:25  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/11/17  18:01:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:47:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:50  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:04:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] down-when-looped
 */

EOLNS	(ci_downloop_eol, serial_downloop_command);
KEYWORD (ci_downloop, ci_downloop_eol, NONE,
	 "down-when-looped", "Force looped serial interface down", 
	 PRIV_CONF);
  
TEST_IDBSTATUS(ci_downloop_test, ci_downloop, NONE, ALTERNATE, IDB_SERIAL);

#undef	ALTERNATE
#define	ALTERNATE	ci_downloop_test
