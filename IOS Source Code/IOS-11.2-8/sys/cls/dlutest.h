/* $Id: dlutest.h,v 3.2 1995/11/17 08:56:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cls/dlutest.h,v $
 *------------------------------------------------------------------
 * 
 * 11-Feb-1994  Leo Pereira
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dlutest.h,v $
 * Revision 3.2  1995/11/17  08:56:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:53:35  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/02  05:15:44  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.1  1995/06/07  20:20:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DLUTEST_H__
#define __DLUTEST_H__

struct dlutest_map 
{
   int connect_id;
   TCLSIQueue *dlutest_queue_p;
   TCLSIQueue *dme_queue_p;
};

enum {
    DLUSTART = 1,
    DLUSTOP,
};

enum {
    DLUPRIMARY =   1,
    DLUSECONDARY,
    DLUPPRIM,				/* PU2.0 primary */
    DLUPSEC,				/* PU2.0 secondary */
    DLUQCALL,
};

typedef enum _dluPortType	/* Choose port type for DLU test code */
{
    DLU_Rsrb = 1,
    DLU_TokenRing,
    DLU_EtherNet,
    DLU_Sdlc,
    DLU_FrameRelay,
    DLU_Fddi,
    DLU_Qllc,
    DLU_Smds,
    DLU_Ppp,
    DLU_Vdlc,
    DLU_ATM_1483
} dluPortType;



typedef enum _dluRputStateT
{
    DLUrput_NullXidSent,
    DLUrput_ConnectCnfWait,
    DLUrput_Connected,		/* don't send another connect from this state!!! */
    DLUrput_DiscCnfWait,	/* don't send another disconnect from this state */
    DLUrput_Disconnected

} dluRputStateT;



extern void dlutest_command (parseinfo *);
extern void dlutest_parser_init(void);

#endif /* __DLUTEST_H__ */
