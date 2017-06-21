/* $Id: cfg_int_tr_speed.h,v 3.2.60.1 1996/08/12 16:02:16 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_tr_speed.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ T R _ S P E E D . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_tr_speed.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:16  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:26:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:11  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:07:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * ring-speed { 4 | 16 }
 * no ring-speed [{ 4 | 16 }]
 */
EOLNS	(cfg_int_ring_speed_eol, tr_set_ring_speed);

KEYWORD_ID(cfg_int_ring_speed_16, cfg_int_ring_speed_eol, no_alt,
	   OBJ(int,1), 16,
	   "16", "Set the speed to 16Mbps", PRIV_CONF);
KEYWORD_ID(cfg_int_ring_speed_4, cfg_int_ring_speed_eol, cfg_int_ring_speed_16,
	   OBJ(int,1), 4,
	   "4", "Set the speed to 4Mbps", PRIV_CONF);

NOPREFIX(cfg_int_ring_speed_no_prefix, cfg_int_ring_speed_4,
	 cfg_int_ring_speed_eol);

KEYWORD	(cfg_int_ring_speed, cfg_int_ring_speed_no_prefix, NONE,
	 "ring-speed", "Set the token ring speed", PRIV_CONF);

TEST_IDBSTATUS(cfg_int_ring_speed_test, cfg_int_ring_speed, NONE, ALTERNATE,
	       IDB_TR);

#undef	ALTERNATE
#define	ALTERNATE	cfg_int_ring_speed_test
