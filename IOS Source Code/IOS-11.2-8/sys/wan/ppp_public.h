/* $Id: ppp_public.h,v 3.3.62.3 1996/07/30 21:01:51 dblair Exp $
 * $Source: /release/112/cvs/Xsys/wan/ppp_public.h,v $
 *------------------------------------------------------------------
 * Prototypes for public ppp.c routines
 *
 * January 1993, Dave katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ppp_public.h,v $
 * Revision 3.3.62.3  1996/07/30  21:01:51  dblair
 * CSCdi57980:  debug ppp multilink is too busy
 * Branch: California_branch
 * Clean up multilink debugs
 *
 * Revision 3.3.62.2  1996/05/01  14:33:52  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Make PPP NCPs into real subsystems.  Add them to all images where
 * PPP and the related network protocol are located.  NCPs are dependant
 * on PPP and the related network protocol subsystem but not vice versa.
 * Remove dependancies between PPP and the compression code.
 *
 * Revision 3.3.62.1  1996/04/27  06:38:05  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.20.1  1996/01/16  15:45:32  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.3  1995/11/17  18:04:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:25  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:50:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  22:39:24  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.1  1995/06/07  23:19:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__PPP_PUBLIC_H__
#define __PPP_PUBLIC_H__

extern boolean ppp_vencapsulate(paktype *, long);
extern boolean ppp_setup(parseinfo *, hwidbtype *);

/*
 * Debugging variables
 */

extern boolean ppp_negotiation_debug;
extern boolean ppp_packet_debug;
extern boolean ppp_error_debug;
extern boolean ppp_chap_debug;
extern boolean multilink_debug;
extern boolean multilink_detailed_debug;
extern boolean multilink_events_debug;
extern boolean multilink_data_debug;

#endif
