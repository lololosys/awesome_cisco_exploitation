/* $Id: exec_debug_ipx_compression.h,v 3.2 1995/11/17 19:21:28 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/exec_debug_ipx_compression.h,v $
 *------------------------------------------------------------------
 * Parser chain for debug ipx compression command
 *
 * Sept 1995, Syed Irfan, (Joel Bion)
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_ipx_compression.h,v $
 * Revision 3.2  1995/11/17  19:21:28  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  08:18:25  irfan
 * First breath in the IOS world.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ipx compression events
 * debug ipx compression activity
 *
 */
EOLS    (debug_novell_compression_events_eol, debug_command,
	 DEBUG_NOVELLCOMPRESSIONEVT);
EOLS    (debug_novell_compression_activity_eol, debug_command,
	 DEBUG_NOVELLCOMPRESSIONACT);
KEYWORD (debug_novell_compression_activity,
	 debug_novell_compression_activity_eol,
	 no_alt,
	 "activity", "IPX compression activity", PRIV_OPR);
KEYWORD (debug_novell_compression_events,
	 debug_novell_compression_events_eol,
	 debug_novell_compression_activity,
	 "events", "IPX compression events", PRIV_OPR);
KEYWORD (debug_novell_compression, debug_novell_compression_events, ALTERNATE,
	 "compression", "IPX compression", PRIV_OPR);

#undef ALTERNATE
#define ALTERNATE debug_novell_compression
