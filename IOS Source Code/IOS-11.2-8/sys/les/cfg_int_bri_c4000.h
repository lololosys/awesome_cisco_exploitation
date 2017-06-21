/* $Id: cfg_int_bri_c4000.h,v 3.1.64.1 1996/08/12 16:04:26 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/cfg_int_bri_c4000.h,v $
 *------------------------------------------------------------------
 * cfg_int_bri_c4000.h  -- [no] bri master-clock
 *
 * March 1994, Manoj Leelanivas.
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************
 * [no] bri master-clock
 *
 */
EOLNS (ci_bri_master_clock_eol, mb_set_master_clock);

KEYWORD (ci_bri_master_clock, ci_bri_master_clock_eol, ALTERNATE,
	 "master-clock",
	 NULL /*"master clock setting"*/, PRIV_CONF | PRIV_HIDDEN);

#undef ALTERNATE
#define ALTERNATE  ci_bri_master_clock


