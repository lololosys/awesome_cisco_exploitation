/* $Id: cfg_int_load-interval.h,v 3.3 1996/02/08 03:07:04 anke Exp $
 * $Source: /release/111/cvs/Xsys/parser/cfg_int_load-interval.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ L O A D - I N T E R V A L . H
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_load-interval.h,v $
 * Revision 3.3  1996/02/08  03:07:04  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.2  1995/11/17  18:43:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:06:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/************************************************************
 *	load-interval <seconds>
 *	no load-interval [<seconds>]
 * Note: don't set the load-interval to allow > 600 - it's used
 * as a index into an array in compute_idb_loads.  The upper value
 * is  MAX_DATARATE_MULT_INDEX.
 */
PARAMS_KEYONLY (ci_load_interval, ALTERNATE,
		"load-interval", OBJ(int,1), 30, 600, 
		load_interval_command, 0,
		"Specify interval for load calculation for an interface",
		"Load interval delay in seconds",
		PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	ci_load_interval
