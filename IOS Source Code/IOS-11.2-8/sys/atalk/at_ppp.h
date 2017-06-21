/* $Id: at_ppp.h,v 3.2.60.2 1996/05/01 14:26:16 fox Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_ppp.h,v $
 *------------------------------------------------------------------
 * atalk_ppp.h -- handle Appletalk PPP interfaces
 *
 * Januaray 1995, Tim Kolar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_ppp.h,v $
 * Revision 3.2.60.2  1996/05/01  14:26:16  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Make PPP NCPs into real subsystems.  Add them to all images where
 * PPP and the related network protocol are located.  NCPs are dependant
 * on PPP and the related network protocol subsystem but not vice versa.
 * Remove dependancies between Appletalk network code and ATCP code.
 *
 * Revision 3.2.60.1  1996/03/23  01:27:16  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  08:43:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/22  06:36:45  tkolar
 * CSCdi38902:  ATCP client mode wont work with ISDN
 * Change method of handing out virtual-net node addresses to accomodate
 * arbitrary interfaces, and prevent non P2P interfaces from being
 * configured for Appletalk client mode.
 *
 * Revision 2.1  1995/06/07  20:09:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

void at_atcp_info(void *, void *, hwidbtype *);
void init_atalk_ppp(void);

#define AT_PPP_ROUTER_NODE 199
#define ATALK_PPP_NODES 255
