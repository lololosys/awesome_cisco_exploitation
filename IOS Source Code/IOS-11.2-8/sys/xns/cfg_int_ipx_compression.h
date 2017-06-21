/* $Id: cfg_int_ipx_compression.h,v 3.3 1995/12/29 01:58:16 irfan Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/xns/cfg_int_ipx_compression.h,v $
 *------------------------------------------------------------------
 * Parse chain for IPX compression, interface subcommand
 *
 * Sept 1995, Syed Irfan, (Joel Bion)
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_ipx_compression.h,v $
 * Revision 3.3  1995/12/29  01:58:16  irfan
 * CSCdi46212:  misspelled command for ipx compression
 * change ipx -> cipx
 *
 * Revision 3.2  1995/11/17  19:20:57  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:37:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  08:18:24  irfan
 * First breath in the IOS world.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 *	ipx compression cipx <slotcount>
 *	[no] ipx compression cipx
 *      OBJ(int,1) = Slot count
 */
PARAMS_KEYONLY(ci_ipx_compression, no_alt, "cipx",
	       OBJ(int,1), IPX_COMPRESSION_MIN_SLOTS, IPX_COMPRESSION_MAX_SLOTS,
	       ipx_compression_if_command, 0,
	       "Enable CIPX style IPX compression on the interface",
	       "Number of slots for CIPX compression connections", PRIV_CONF);

/*
 * The start of the entire thing... the 'compression' keyword
 */

KEYWORD	(ci_ipx_compression_key, ci_ipx_compression, ALTERNATE,
	 "compression", "Select IPX compression commands",
	 PRIV_CONF|PRIV_SUBIF);

#undef  ALTERNATE
#define ALTERNATE ci_ipx_compression_key
