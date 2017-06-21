/* $Id: service.h,v 3.3.48.1 1996/03/18 22:29:24 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/ui/service.h,v $
 *------------------------------------------------------------------
 * Prototypes for service.c
 *
 * January 1993, Dave Katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: service.h,v $
 * Revision 3.3.48.1  1996/03/18  22:29:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.3  1996/03/14  01:36:23  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.16.2  1996/03/09  05:14:16  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.16.1  1996/02/20  21:26:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/10  21:02:43  smackie
 * Next on a very special Geraldo. Unused global booleans and the files
 * that loved them. (CSCdi45340)
 *
 * Revision 3.2  1995/11/17  17:49:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/11  23:46:08  billw
 * CSCdi35800:  Need knob to supress user view of IP addresses
 *
 * Revision 2.1  1995/06/07  23:11:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SERVICE_H__
#define __SERVICE_H__

extern void service_init(void);

extern boolean internal_cmd_enable;
extern boolean log_backtrace;

extern boolean tcp_keepalivesin;
extern boolean tcp_keepalivesout;
extern boolean old_slip_prompts;
extern boolean service_hideaddr;
extern boolean nagle_enable;
extern boolean finger_enable;
extern boolean telnet_idle0window;
extern boolean tcpservers_enable;
extern boolean udpservers_enable;

#endif __SERVICE_H__
