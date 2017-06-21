/* $Id: if_ags_c7000_trip.h,v 3.3 1995/11/17 09:17:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/if_ags_c7000_trip.h,v $
 *------------------------------------------------------------------
 * if_ags_c7000_trip.h -- defines for the cBus Token Ring Interface Card
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_ags_c7000_trip.h,v $
 * Revision 3.3  1995/11/17  09:17:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:40:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_AGS_C7000_TRIP_H__
#define __IF_AGS_C7000_TRIP_H__

/**********************************************************************
 *
 * Static inline functions used in TRIP related files.
 *
 **********************************************************************/

static inline void cbus_command_a_inline (
    mciregtype *regptr,
    ushort command,
    ushort argument)
{
    regptr->argreg = argument;
    regptr->cmdreg = command;
}


/**********************************************************************
 *
 * Prototypes for functions in if_ags_c7000_trip.c
 *
 **********************************************************************/

void cbus_command_a(mciregtype *, ushort, ushort);
void cbus_command_la(mciregtype *, ushort, ulong);
void ctr_srb_frame(hwidbtype *, mciregtype *, ushort, int);

#endif   /* __IF_AGS_C7000_TRIP_H__ */

